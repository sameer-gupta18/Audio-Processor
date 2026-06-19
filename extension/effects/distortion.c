#include "effect.h"
#include <math.h>
#include <stdlib.h>

#define DRIVE_C 1.0f
#define DRIVE_M 19.0f

typedef struct { int rate; } distortion_state;

static void distortion_reset(fx *self) { (void)self;}

static void distortion_process(fx *self, float *buf, int n, float amt){
    
    (void)self;

    //setting a "crunch" level using the drive
    float drive = DRIVE_C + DRIVE_M * amt;

    //define the frequency modulation using a normalizing function over the drive
    float norm = tanhf(drive);
    for (int i = 0; i < n; i++){
        float x = buf[i];
        
        //ensures clean and controlled distortion

        float wet = tanhf(drive * x) / norm;
        buf[i] = (1.0f - amt) * x + amt * wet;
    } 
}

//generate distortion effect struct
fx *fx_distortion_create(int sample_rate){
    fx *f = calloc(1, sizeof *f);
    distortion_state *s = calloc(1, sizeof *s);

    if (!f || !s){
        free(f);
        free(s);
        return NULL;
    }

    s -> rate = sample_rate;

    f -> name = "DISTORTION";
    f -> reset = distortion_reset;
    f -> process = distortion_process;
    f -> state = s;
    return f;

}
