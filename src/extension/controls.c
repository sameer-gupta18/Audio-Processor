#include "controls.h"

#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <stdatomic.h>

#define NUM_ENCODERS 7
#define TICKS_PER_DETENT 25
#define POLL_US 1000
#define BUTTON_DEBOUNCE_POLLS 20

typedef struct {
    unsigned int clk;
    unsigned int dt;
    unsigned int sw;
} encoder_pins;

static encoder_pins pins[NUM_ENCODERS] = {
    {17, 27, 22}, // volume
    {2,  3,  4}, //reverb
    // {19, 26, 21},
    // {20, 16, 12},
    // {23, 24, 25},
    // {4,  18, 15},
    // {14, 8,  7}
};

typedef struct {
    int last_state;
    int acc;

    int logical_button;
    int last_raw_button;
    int stable_count;
} encoder_state;

static encoder_state encoders[NUM_EFFECTS];


static const int8_t quad_table[16] = {
    0, -1, +1,  0,
   +1,  0,  0, -1,
   -1,  0,  0, +1,
    0, +1, -1,  0
};
