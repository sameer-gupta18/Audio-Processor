#include "effect.h"
#define INPUT_GAIN 0.015f;
#define ALLPASS_COEFFICIENT 0.5f;

//comb structure
typedef struct {
    float *buf;
    int len, ind;
    float store;
}comb ;

//allpass structure
typedef struct {
    float *buf;
    int len, ind;
}allpass ;

//reverb state
typedef struct { 
    comb c[8];
    allpass a[4];
}reverb_state ; 


static inline float comb_tick(comb *c, float damp, float in, float fb) {
    float out = c->buf[c->ind];
    c->store = out * (1.0f - damp) + (c->store * damp);
    c->buf[c->ind] = in + (c->store * fb);
    if(++c->ind >= c->len) c->ind = 0;
    return out;
}

static inline float allpass_tick(allpass *a, float in) {
    float bufout = a->buf[a->ind];
    a->buf[a->ind] = in + bufout * ALLPASS_COEFFICIENT;
    if(++a->ind >= a->len) a->ind = 0;
    return  bufout - in;
}

//reset sets all the buffers to 0
static void reverb_resest(fx *self) {
    reverb_state *s = self->state;
    for(int i = 0; i < 8; i++) {
        s->c->buf[i] = 0.0f;
    }
    for(int i = 0; i < 4; i++) {
        s->a->buf[i] = 0.0f;
    }
    s->c->store = 0.0f;
}

static void reverb_process(fx *self, float *buf, int n, const float p[]) {
    reverb_state *s = self->state;
    float fb = 0.7f + 0.28f * (p[0] / 100.0f); 
    float damp = 0.4f * (p[1] / 100.0f); 
    float mix = p[2] / 100.0f;
    for (int i = 0; i < n; i++) {
        float in = buf[i] * INPUT_GAIN;
        float wet = 0.0f;
        for (int j = 0; j < 8; j++) {
            //comb runs in parallel
            wet += comb_tick(&s->c[j], damp, in, fb);
        }
        for (int j = 0; j < 4; j++) {
            //allpass runs in series
            wet = allpass_tick(&s->a[j], in);
        }
        buf[i] = (1.0f - mix) * buf[i] + mix * wet;
    }
}

