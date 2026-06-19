#include "effect.h"

#include <stdlib.h>
#include <stdio.h>

#define INPUT_GAIN 0.015f
#define ALLPASS_COEFFICIENT 0.5f
#define NUM_COMBS 8
#define NUM_ALLPASS 4

// random numbers that are non multiples, non powers of two, and not evenly spaced
static const int comb_lengths[NUM_COMBS] = {1116,1188,1277,1356,1422,1491,1557, 1617};
static const int all_pass_lengths[NUM_ALLPASS] = {556,441,341,225};

//comb structure
typedef struct {
    float *buf;
    int len, ind;
    float store;
} comb ;

//allpass structure
typedef struct {
    float *buf;
    int len, ind;
} allpass ;

//reverb state
typedef struct { 
    comb c[NUM_COMBS];
    allpass a[NUM_ALLPASS];
    int rate;
} reverb_state ; 

// ensures each comb tick is affected by the previous comb tick
static inline float comb_tick(comb *c, float damp, float in, float fb) {
    float out = c->buf[c->ind];
    c->store = out * (1.0f - damp) + (c->store * damp);
    c->buf[c->ind] = in + (c->store * fb);
    if(++c->ind >= c->len) c->ind = 0;
    return out;
}
// serial processing of each tick - blends different waves together
static inline float allpass_tick(allpass *a, float in) {
    float bufout = a->buf[a->ind];
    a->buf[a->ind] = in + bufout * ALLPASS_COEFFICIENT;
    if(++a->ind >= a->len) a->ind = 0;
    return  bufout - in;
}

//reset sets all the buffers to 0
static void reverb_reset(fx *self) {
    reverb_state *s = self->state;
    for(int i = 0; i < NUM_COMBS; i++) {
        s->c[i].store = 0.0f;
        s->c[i].ind = 0;
        for (int j = 0; j < s->c[i].len; j++){
                  s->c[i].buf[j] = 0.0f;
        }
    }
    for(int i = 0; i < NUM_ALLPASS; i++) {
        s->a[i].ind = 0;
        for (int j = 0; j < s->a[i].len; j++){
            s->a[i].buf[j] = 0.0f;
        }
    }
    
}

static void reverb_process(fx *self, float *buf, int n, float amt) {
    reverb_state *s = self->state;
    float fb = 0.7f + 0.28f * ALLPASS_COEFFICIENT; 
    float damp = 0.4f * ALLPASS_COEFFICIENT; 
    float mix = amt;
    for (int i = 0; i < n; i++) {
        float in = buf[i] * INPUT_GAIN;
        float wet = 0.0f;
        for (int j = 0; j < 8; j++) {
            //comb runs in parallel
            wet += comb_tick(&s->c[j], damp, in, fb);
        }
        for (int j = 0; j < 4; j++) {
            //allpass runs in series
            wet = allpass_tick(&s->a[j], wet);
        }
        buf[i] = (1.0f - mix) * buf[i] + mix * wet;
    }
}

fx *fx_reverb_create(int sample_rate){
    fx *f = calloc(1, sizeof *f);
    reverb_state *s = calloc(1, sizeof *s);

    if (!f || !s){
        free(f);
        free(s);
        return NULL;
    }

    s -> rate = sample_rate;
    for (int i = 0; i < NUM_COMBS; i++){
        s->c[i].len = comb_lengths[i];
        s->c[i].buf = calloc(s->c[i].len,sizeof(float));
        s->c[i].ind = 0;
        s->c[i].store = 0.0f;
        if (s->c[i].buf == NULL){
            fprintf(stderr, "Could not allocate memory to comb buffer");
        }
    }

     for (int i = 0; i < NUM_ALLPASS; i++){
        s->a[i].len = all_pass_lengths[i];
        s->a[i].buf = calloc(s->a[i].len,sizeof(float));
        s->a[i].ind = 0;
        if (s->a[i].buf == NULL){
            fprintf(stderr, "Could not allocate memory to all pass buffer");
        }
    }

    f -> name = "REVERB";
    f -> reset = reverb_reset;
    f -> process = reverb_process;
    f -> state = s;
    return f;
}
