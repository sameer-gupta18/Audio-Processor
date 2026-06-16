#include "audio.h"
#include "alsa_setup.h"
#include "controls.h"
#include "state.h"
#include <alsa/pcm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <pthread.h>
#include <unistd.h>

#define RATE 44100

int main(int argc, char **argv){
    const char *capture_dev = "plughw:CARD=SF558";
    const char *playback_dev = "plughw:CARD=Headphones";

    audio_ctx_t ctx;
    ctx_init(&ctx); 

    shared_state state; 
    share_state_init(&state); 
    ctx.shared = &state;

    snd_pcm_uframes_t cap_period = PERIOD;
    if(open_and_configure(&ctx.capture, capture_dev, SND_PCM_STREAM_CAPTURE,1,RATE,cap_period) < 0){
        return EXIT_FAILURE;
    }

    snd_pcm_uframes_t pb_period = PERIOD;
    if (open_and_configure(&ctx.playback, playback_dev, SND_PCM_STREAM_PLAYBACK, 2, RATE,pb_period) < 0) {
        return EXIT_FAILURE;
    }

    ctx.period = cap_period;
    if (set_playback_startup(ctx.playback, ctx.period) < 0) {
        return EXIT_FAILURE;
    }
    start_playback(&ctx);

    int err = snd_pcm_start(ctx.capture);

    if (err < 0){
        fprintf(stderr, "snd_pcm_start(capture) failed %s\n",snd_strerror(err));
        return EXIT_FAILURE;
    }
    err = controls_init(); 
    if(err != 0){
        fprintf(stderr, "GPIO initilaisation failed!");
        return EXIT_FAILURE; 
    }
    pthread_t audio_thread; 
    err = pthread_create(&audio_thread, NULL, audio_thread_set_up, &ctx); 
    if(err != 0){
        fprintf(stderr, "Audio thread failed!");
        return EXIT_FAILURE; 
    }
    for(;;){
        controls_poll(&state); 
        usleep(1000); // sleep for one thousandth second
    }
    snd_pcm_close(ctx.capture);
    snd_pcm_close(ctx.playback);
    return EXIT_SUCCESS;
}