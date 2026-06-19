#ifndef AUDIO_H
#define AUDIO_H

#include "state.h"
#include "effects/effect.h"

#include <alsa/asoundlib.h>
#include <alsa/pcm.h>
#include <stdatomic.h>
#include <stdint.h>

#define PERIOD 512
#define IN_SCALE (1.0f / 32768.0f)
#define OUT_SCALE 32767.0f
#define SCHED_PRIORITY 80

typedef struct {
    snd_pcm_t *capture;
    snd_pcm_t *playback;

    snd_pcm_uframes_t period;

    int16_t in_buf[PERIOD];
    float work_buf[PERIOD];
    int16_t out_buf[PERIOD * 2];
    shared_state *shared;
    fx *effects[NUM_EFFECTS];
    int num_effects;
    int prev_muted[NUM_EFFECTS];

} audio_ctx_t;

void *audio_thread_set_up(void* ctx);

void ctx_init(audio_ctx_t *ctx);
void start_playback(audio_ctx_t *ctx);

void audio_loop(audio_ctx_t *ctx);


#endif
