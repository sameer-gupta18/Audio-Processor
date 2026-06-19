#ifndef ALSA_SETUP_H
#define ALSA_SETUP_H
#include <alsa/asoundlib.h>
#include <alsa/pcm.h>

extern int open_and_configure(
    snd_pcm_t **pcm, 
    const char *device, 
    snd_pcm_stream_t stream, 
    unsigned channels,
    unsigned rate,
    snd_pcm_uframes_t period
);

extern int set_playback_startup(snd_pcm_t *pcm, snd_pcm_uframes_t period);

#endif 
