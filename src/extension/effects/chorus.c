#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "effect.h"

#define CHORUS_LFO_RATE 0.8f
#define CHORUS_MODULATION_DEPTH 0.004f
#define CHORUS_BASE_DELAY 0.015f
#define CHORUS_SCALAR 2

typedef struct {
    float  *buf;
    int     buf_len;
    int     write_idx;
    float   phase;
    float   sample_rate;
} ChorusState;

// reset chorus 
static void chorus_reset(fx *self) {
    ChorusState *s = self->state;
    memset(s->buf, 0, s->buf_len * sizeof(float));
    s->write_idx = 0;
    s->phase = 0.0f;
}


static void chorus_process(fx *self, float *buf, int n, float amt) {
    ChorusState *s = self->state;

    float sample_rate = s->sample_rate;
    float base_delay = CHORUS_BASE_DELAY * sample_rate;
    float depth = CHORUS_MODULATION_DEPTH * sample_rate;
    float phase_inc = CHORUS_LFO_RATE / sample_rate;

    for (int i = 0; i < n; i++) {
        float x = buf[i];

        s->buf[s->write_idx] = x;

        float delay_samples = base_delay + depth * sinf(2.0f * (float)M_PI * s->phase);

        float read_pos = (float)s->write_idx - delay_samples;
        if (read_pos < 0.0f) {
            read_pos += (float)s->buf_len;
        }

        int idx0 = (int)read_pos;
        float frac = read_pos - (float)idx0;
        int idx1 = (idx0 + 1) % s->buf_len;
        
        float r = s->buf[idx0] * (1.0f - frac) + s->buf[idx1] * frac;

        // linear interpolation 
        buf[i] = x + amt * r * CHORUS_SCALAR;
        
        s->phase += phase_inc;
        if (s->phase >= 1.0f) s->phase -= 1.0f;

        s->write_idx = (s->write_idx + 1) % s->buf_len;
    }

}

fx *fx_chorus_create(int sample_rate){
    fx *self = calloc(1, sizeof(fx));
    if (!self) return NULL;

    ChorusState *s = calloc(1, sizeof(ChorusState)); 
    if (!s) {
        free(self);
        return NULL;
    }

    // worst case delay 
    float max_delay_s = CHORUS_BASE_DELAY + CHORUS_MODULATION_DEPTH + 0.001f;
    s->buf_len = (int)(max_delay_s * (float)sample_rate) + 2;
    s->buf = calloc(s->buf_len, sizeof(float));
    if (!s->buf) {
        free(s);
        free(self);
        return NULL;
    }
    
    s->write_idx = 0;
    s->phase = 0.0f;
    s->sample_rate = (float)sample_rate;

    self->name = "chorus";
    self->reset = chorus_reset;
    self->process = chorus_process;
    self->state = s;
    return self;
}
