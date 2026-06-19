#ifndef STATE_H
#define STATE_H
#include <stdatomic.h>

#define NUM_EFFECTS 5
typedef struct {
    _Atomic int intensity[NUM_EFFECTS];
    _Atomic int muted[NUM_EFFECTS];
    _Atomic unsigned xruns;

} shared_state;

void share_state_init(shared_state *s);

#endif