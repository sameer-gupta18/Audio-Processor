#ifndef STATE_H
#define STATE_H
#include <stdatomic.h>

#define NUM_EFFECTS 1   //change later with more effects
#define VOLUME_IDX 0
typedef struct {
    _Atomic int intensity[NUM_EFFECTS];
    _Atomic int muted[NUM_EFFECTS];
    _Atomic unsigned xruns;
} shared_state;

void share_state_init(shared_state *s);

#endif