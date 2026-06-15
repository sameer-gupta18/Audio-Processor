#ifndef EFFECT_H
#define EFFECT_H

#include "../state.h"
typedef struct fx fx;

struct fx {
    const char *name;
    void (*reset)(fx *self);
    void (*process)(fx *self, float *buf, int n, float amt);
    void *state;
};

fx *fx_volume_create(int sample_rate);

#endif