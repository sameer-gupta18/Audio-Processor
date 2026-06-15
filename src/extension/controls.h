#ifndef CONTROLS_H
#define CONTROLS_H

#include "state.h"

int controls_init(void);

void controls_run(shared_state *shared);

void controls_close(void);

#endif