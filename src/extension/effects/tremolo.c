#include "effect.h"
#include <math.h>
#include <stdlib.h>

#define TREMOLO_FACTOR 1.5

typedef struct { 
    float curr_phase; // phase accumulator based on number of samples processed. 
    int rate; 
    } tremolo_state;

//the phase accumulator wraps back down to 0

static void tremolo_reset(fx *self) {
     (void)self;
}

#define PHASE_INC 0.0001f

static void tremolo_process(fx *self, float *buf, int n, float amt){
    // we consider amt as the depth of the sin wave
    tremolo_state *s = self->state; 
    for(int i = 0; i <  n; i++){
        // 0.5 + 0.5 * sinf(..) shifts LCO back to [0,1] 
        
        float res = 0.5 + 0.5 * sinf(2.0f * (float)M_PI * s->curr_phase);
        buf[i] *= 1.0f - (amt * TREMOLO_FACTOR * res);  // 0 depth produces no effect
        s->curr_phase += PHASE_INC; 
        if(s->curr_phase >= 1.0f){
            s->curr_phase = 0.0f; // wrap back to start of sine wave. 
        }
    }
}
// generate tremelo fx struct 
fx *fx_tremolo_create(int sample_rate){
    fx *f = calloc(1, sizeof *f);
    tremolo_state *s = calloc(1, sizeof(tremolo_state));

    if (!f || !s){
        free(f);
        free(s);
        return NULL;
    }

    s -> rate = sample_rate;

    f -> name = "TREMOLO";
    f -> reset = tremolo_reset;
    f -> process = tremolo_process;
    f -> state = s;
    return f;

}
