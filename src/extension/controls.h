#ifndef CONTROLS_H
#define CONTROLS_H

#include "state.h"

int controls_init(void);

void controls_poll(shared_state *s); 
void controls_close(void);

#endif
