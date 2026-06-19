#include "audio.h"
#include "alsa_setup.h"
#include "controls.h"
#include "display.h"
#include "state.h"
#include <alsa/pcm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <pthread.h>
#include <unistd.h>

#define RATE 44100

int main(void){
    // set up I/O devices
    const char *capture_dev = "plughw:CARD=SF558";
    const char *playback_dev = "plughw:CARD=Headphones";

    audio_ctx_t ctx;
    ctx_init(&ctx); 

    shared_state state; 
    share_state_init(&state); 
    ctx.shared = &state;

    snd_pcm_uframes_t cap_period = PERIOD;
    if(
        open_and_configure(
        &ctx.capture, 
        capture_dev,
         SND_PCM_STREAM_CAPTURE,
         1,
         RATE,
         cap_period) < 0
        )
    {
        return EXIT_FAILURE;
    }

    snd_pcm_uframes_t pb_period = PERIOD;
    if (
        open_and_configure(
            &ctx.playback,
             playback_dev, 
             SND_PCM_STREAM_PLAYBACK, 
             2, 
             RATE,
             pb_period) < 0
        ) 
    {
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
    int ticks = 0; 
    display_start(); 
    static const char *names[5] = {"VOL", "REV", "DIS", "TRM", "CHO"};
    // main control loop
        for(;;){
        controls_poll(&state); 
        if (ticks % 100 == 0){
            for(int i = 0; i < NUM_EFFECTS; i++){
                int curr_intensity = atomic_load(&state.intensity[i]) / 10;
                int bar_intensity = curr_intensity / 10;
                int curr_muted = atomic_load(&state.muted[i]); 
                char res[11]; 
                for(int j = 0; j < 10; j++){
                    if(j < bar_intensity){
                        res[j] = '#';
                    } else{
                        res[j] = '-'; 
                    }
                }
                res[10] = '\0';
                printf("%s [%s] %3d%% %s", names[i], res, curr_intensity, curr_muted ? "M " : "  ");
            }
            putchar('\n');
            display_update(&state); 
        }

        ticks++; 
        // prevent ticks overflow
        if (ticks >= 10001){
            ticks = 0; 
        }
        usleep(1000); // sleep for one thousandth second
    }

    snd_pcm_close(ctx.capture);
    snd_pcm_close(ctx.playback);
    display_close(); 
    return EXIT_SUCCESS;
}
