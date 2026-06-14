#ifndef AUDIO_H
#define AUDIO_H

#include <alsa/asoundlib.h>
#include <alsa/pcm.h>
#include <stdatomic.h>
#include <stdint.h>

#define PERIOD_DEFAULT 256
#define IN_SCALE (1.0f / 32768.0f)
#define OUT_SCALE 32767.0f
#define SCHED_PRIORITY 80

typedef struct {
    snd_pcm_t *capture;
    snd_pcm_t *playback;

    snd_pcm_uframes_t period;

    int16_t in_buf[PERIOD_DEFAULT];
    float work_buf[PERIOD_DEFAULT];
    int16_t out_buf[PERIOD_DEFAULT * 2];

    atomic_int xruns;
} audio_ctx_t;

void rt_setup(void);

void prime_playback(audio_ctx_t *ctx);

void audio_loop(audio_ctx_t *ctx);


#endif