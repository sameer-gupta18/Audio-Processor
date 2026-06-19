#include "controls.h"
#include "state.h"

#include <gpiod.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdatomic.h>

#define NUM_ENCODERS 5
#define TICKS_PER_DETENT 25
#define POLL_US 1000
#define BUTTON_DEBOUNCE_POLLS 20
typedef struct {
    unsigned int clk;
    unsigned int dt;
    unsigned int sw;
} encoder_pins;

static encoder_pins pins[NUM_ENCODERS] = {
    {17, 27, 22}, // volume 
    {8,  9,  11}, //reverb
    {23,24,25}, //distortion
    {16,20,21}, //tremolo
    {4,18,7} //chorus
};

// state of each individual encoder 
typedef struct {
    int last_state;
    int acc;

    int btn_stable_state;
    int btn_last_raw;
    int btn_curr_count;
} encoder_state;

static encoder_state encoders[NUM_EFFECTS];

// handle quadrature encoding for the rotary encodedrs
// n.b. 4 changes is a detent, i.e. a click
static const int8_t quad_table[4][4] = {
    {0, 1, -1,  0}, 
    {-1,  0,  0, +1},
   {1,  0,  0, -1},
    {0, -1, +1,  0}, 
};

static int encode_step(encoder_state *e, int clk, int dt){
    uint8_t curr = (uint8_t)(clk<<1|dt); 
    int index1 = e->last_state;
    int index2 = curr;

    e->acc += quad_table[index1][index2];
    e->last_state = curr; 
    if (e->acc >= 4 || e->acc <= -4){
        int res = e->acc > 0 ? 1 : -1;
        e->acc = 0;
        return res;
    }
    return 0;
}

static int btn_press(encoder_state *e, int raw){
    if (raw != e->btn_last_raw){
        e->btn_last_raw = raw; 
        e->btn_curr_count = 0;
    } else if(e->btn_curr_count<19){
        e->btn_curr_count++; 
    } else{
        if (e->btn_curr_count == 19 && raw != e->btn_stable_state){
        e->btn_stable_state = raw; 
        return (e->btn_stable_state == 0 ? 1 : 0); // returns true if pressed else false  
        }
    }
    return 0;   
}

static struct gpiod_chip* chip;
static struct gpiod_line_request* req; 

int controls_init(void){
    chip = gpiod_chip_open("/dev/gpiochip0"); 
    if (!chip){
        return -1; 
    }

    struct gpiod_line_settings *i_settings= gpiod_line_settings_new(); 
    gpiod_line_settings_set_direction(i_settings, GPIOD_LINE_DIRECTION_INPUT); 
    
    struct gpiod_line_settings *p_settings = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(p_settings, GPIOD_LINE_DIRECTION_INPUT); 
    // so SW can idle at 1
    gpiod_line_settings_set_bias(p_settings, GPIOD_LINE_BIAS_PULL_UP);

    struct gpiod_line_config *line_cnfg = gpiod_line_config_new();
    for(int i = 0; i < NUM_ENCODERS; i++){
        unsigned clk[1] = {pins[i].clk};
        unsigned dt[1] = {pins[i].dt};
        unsigned sw[1] = {pins[i].sw}; 
        gpiod_line_config_add_line_settings(line_cnfg, clk, 1, i_settings); 
        gpiod_line_config_add_line_settings(line_cnfg, dt, 1, i_settings); 
        gpiod_line_config_add_line_settings(line_cnfg, sw, 1, p_settings); 
    }

    struct gpiod_request_config *request_cfg = gpiod_request_config_new(); 
    gpiod_request_config_set_consumer(request_cfg, "audiofx"); 

    req = gpiod_chip_request_lines(chip, request_cfg, line_cnfg); 
    gpiod_request_config_free(request_cfg);
    gpiod_line_config_free(line_cnfg);
    gpiod_line_settings_free(i_settings);
    gpiod_line_settings_free(p_settings);
    if(req == 0){
        gpiod_chip_close(chip);
        fprintf(stderr, "Could not request gpio pins successfully.");
        return -1; 
    }
    enum gpiod_line_value vals[NUM_EFFECTS*3]; 
    gpiod_line_request_get_values(req, vals); 
    // initialise all encoders 
    for(int i =0; i < NUM_ENCODERS; i++){
        int clk_init = (vals[3*i]==GPIOD_LINE_VALUE_ACTIVE) ? 1 : 0;
        int dt_init = (vals[3*i+1]==GPIOD_LINE_VALUE_ACTIVE) ? 1 : 0;
        int sw_init = (vals[3*i+2]==GPIOD_LINE_VALUE_ACTIVE) ? 1 : 0;
        encoders[i].last_state =(uint8_t) (clk_init << 1 | dt_init);
        encoders[i].acc = 0; 
        encoders[i].btn_curr_count = 0; 
        encoders[i].btn_stable_state = sw_init; 
        encoders[i].btn_last_raw = sw_init; 
    }
    return 0; 
}

void controls_poll(shared_state *s){
    enum gpiod_line_value vals[NUM_EFFECTS*3]; 
    gpiod_line_request_get_values(req, vals); 
    // loop over all encoders
    for(int i =0; i < NUM_ENCODERS; i++){
        int clk_new = (vals[3*i]==GPIOD_LINE_VALUE_ACTIVE) ? 1 : 0;
        int dt_new = (vals[3*i+1]==GPIOD_LINE_VALUE_ACTIVE) ? 1 : 0;
        int sw_new = (vals[3*i+2]==GPIOD_LINE_VALUE_ACTIVE) ? 1 : 0;
        int res = encode_step(&encoders[i], clk_new, dt_new);
        if (res != 0){
            int curr_intensity = atomic_load(&s->intensity[i]);
            int new_intensity = curr_intensity + res*25;
            if(new_intensity < 0){
                new_intensity = 0;
            }
            if (new_intensity > 1000){
                new_intensity = 1000; 
            }
            atomic_store(&s->intensity[i], new_intensity );
        }
        if (btn_press(&encoders[i],sw_new)){
            int curr_muted = atomic_load(&s->muted[i]);
            // reverse current state
            atomic_store(&s->muted[i], !curr_muted); 
        }
    }
}

// release malloced gpio structs 
void controls_close(void){
    if(req){
        gpiod_line_request_release(req);
    }
    if(chip){
        gpiod_chip_close(chip); 
    }

}