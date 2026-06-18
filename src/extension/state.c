#include "state.h"

// initialise shared state
void share_state_init(shared_state *s){
    for (int i = 0; i < NUM_EFFECTS; i++){
        atomic_store(&s->intensity[i],0);
        atomic_store(&s->muted[i],0);
    }
    atomic_store(&s->xruns,0);
}