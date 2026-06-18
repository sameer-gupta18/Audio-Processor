#include "effect.h"
// #include <math.h>
#include <stdlib.h>

#define NOISE_THRESHOLD 0.05
#define MAX_VOL_MULT 3
typedef struct { int rate; } volume_state;

static void volume_reset(fx *self) {
    (void)self; 
}

static void volume_process(fx *self, float *buf, int n, float amt){
    
    (void)self;

    float g = MAX_VOL_MULT * amt * amt;  //max 3x volume
    for (int i = 0; i < n; i++){
            buf[i] *=  g;
    }
}

fx *fx_volume_create(int sample_rate){
    fx *f = calloc(1, sizeof *f);
    volume_state *s = calloc(1, sizeof *s);

    if (!f || !s){
        free(f);
        free(s);
        return NULL;
    }

    s -> rate = sample_rate;

    f -> name = "VOLUME";
    f -> reset = volume_reset;
    f -> process = volume_process;
    f -> state = s;
    return f;

}
