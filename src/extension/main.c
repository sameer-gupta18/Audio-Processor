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
    int intensity[NUM_EFFECTS] = {0};
    int muted[NUM_EFFECTS] = {0}; 
    unsigned xruns = 0; 
    int ticks = 0; 
    for(;;){
        controls_poll(&state); 
        for(int i = 0; i < NUM_EFFECTS; i++){
            intensity[i] = atomic_load(&state.intensity[i]);
            muted[i] = atomic_load(&state.muted[i]);
        }
        printf("VOL %3d. Muted %d\n", intensity[0]/10, muted[0]);
        printf("REV %3d. Muted %d\n", intensity[1]/10, muted[1]);
        printf("effect 3 %3d. Muted %d\n", intensity[2]/10, muted[2]);
        printf("effect 4 %3d. Muted %d\n", intensity[3]/10, muted[3]);
        printf("effect 5 %3d. Muted %d\n", intensity[4]/10, muted[4]);
        printf("effect 6 %3d. Muted %d\n", intensity[5]/10, muted[5]);
        printf("effect 7 %3d. Muted %d\n", intensity[6]/10, muted[6]);
        
        usleep(1000); // sleep for one thousandth second
    }
    snd_pcm_close(ctx.capture);
    snd_pcm_close(ctx.playback);
    return EXIT_SUCCESS;
}