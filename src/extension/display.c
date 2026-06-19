#include "display.h"
#include "./lib-ssd1306/ssd1306.h"
#include "state.h"
#include <stdatomic.h>
#include <stdio.h>

#define NUM_CHARS 32
// start display
int display_start(void){
    if(ssd1306_init(1)!=0){
        return -1; 
    }
    ssd1306_oled_default_config(64, 128);
    ssd1306_oled_clear_screen();
    return 0; 
}
// names of each effect
static const char* names[5] = {"VOL","REV","DIS", "TRE", "CHO"};

// update the display
void display_update(shared_state *s){
    for(int i = 0; i < NUM_EFFECTS; i++){
        int intensity = atomic_load(&s->intensity[i]) / 10;
        int bar_intensity = intensity / 10;
        int muted = atomic_load(&s->muted[i]); 
        // 21 chars fit per row
        char out[NUM_CHARS]; 
        char res[11]; 
        for(int j = 0; j < 10; j++){
            if(j < bar_intensity){
                res[j] = '%';
            } else{
                res[j] = ' '; 
            }
        }
        res[10] = '\0';
        snprintf(out, sizeof(out), "%s %s %3d %s", names[i], res, intensity, muted ? "M" : " ");
        ssd1306_oled_set_XY(0, i); 
        ssd1306_oled_write_string(0, out); 
    }
}

// close display
void display_close(void){
    ssd1306_oled_clear_screen();
    ssd1306_end(); 
}
