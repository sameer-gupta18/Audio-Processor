#include "audio.h"
#include "alsa_setup.h"
#include <alsa/pcm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <pthread.h>
#include <unistd.h>

#define RATE 44100

typedef struct {
    audio_ctx_t *ctx;
} monitor_arg_t;

static void *monitor_thread(void *arg){
    monitor_arg_t *m = (monitor_arg_t *)arg;
    int last = 0;

    for (;;){
        sleep(5);
        int current = atomic_load(&m -> ctx ->xruns);
        if (current != last){
            fprintf(stderr, "[monitor] xruns total: %d\n",current);
            last = current;
        }
    }
    return NULL;
}

static void usage(const char *prog){
    fprintf(stderr, 
        "usage: %s [--capture-device NAME] [--playback-device NAME] [--period N]\n"
        "  defaults:\n"
        "    --capture-device  plughw:CARD=Device\n"
        "    --playback-device plughw:CARD=Headphones\n"
        "    --period          %d\n",
        prog, PERIOD_DEFAULT);      //change error message later on
}

int main(int argc, char **argv){
    const char *capture_dev = "plughw:CARD=Device";
    const char *playback_dev = "plughw:CARD=Headphones";
    snd_pcm_uframes_t period_req = PERIOD_DEFAULT;

    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], "--capture-device") == 0 && i + 1 < argc) {
            capture_dev = argv[++i];
        } else if (strcmp(argv[i], "--playback-device") == 0 && i + 1 < argc) {
            playback_dev = argv[++i];
        } else if (strcmp(argv[i], "--period") == 0 && i + 1 < argc) {
            long val = atoi(argv[++i]);
            if (val <= 0){
                fprintf(stderr, "FATAL: --period must be a positive integer, got '%s'\n",argv[i]);
                exit(1);
            }
            period_req = (snd_pcm_uframes_t)val;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "unknown argument: %s\n", argv[i]);
            usage(argv[0]);
            return 1;
        }
    }

    if (period_req > PERIOD_DEFAULT){
        fprintf(stderr, "FATAL: period %lu exceeds the compiled buffer size %d\n",(unsigned long)period_req,PERIOD_DEFAULT);
        exit(1);
    }

    audio_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    atomic_init(&ctx.xruns, 0);

    snd_pcm_uframes_t cap_period = period_req;
    if(open_and_configure(&ctx.capture, capture_dev, SND_PCM_STREAM_CAPTURE,1,RATE,cap_period) < 0){
        fprintf(stderr, "FATAL: capture device setup failed (%s)\n", capture_dev);
        return 1;
    }

    snd_pcm_uframes_t pb_period = period_req;
    if (open_and_configure(&ctx.playback, playback_dev, SND_PCM_STREAM_PLAYBACK, 2, RATE,pb_period) < 0) {
        fprintf(stderr, "FATAL: playback device setup failed (%s)\n", playback_dev);
        return 1;
    }

    ctx.period = cap_period;

    /* Software params: start_threshold = 2 periods on playback */
    if (set_playback_startup(ctx.playback, ctx.period) < 0) {
        fprintf(stderr, "FATAL: failed to set playback start_threshold\n");
        return 1;
    }

    rt_setup();

    prime_playback(&ctx);

    int err = snd_pcm_start(ctx.capture);
    if (err < 0){
        fprintf(stderr, "FATAL: snd_pcm_start(capture) failed: %s\n",snd_strerror(err));
        exit(1);
    }

    pthread_t mon_tid;
    monitor_arg_t mon_arg = {.ctx = &ctx};
    int prc = pthread_create(&mon_tid, NULL, monitor_thread, &mon_arg);

    if (prc != 0) {
        fprintf(stderr, "warning: failed to start monitor thread: %s\n", strerror(prc));
    }

    fprintf(stderr, "running: period=%lu, capture=%s, playback=%s\n",(unsigned long)ctx.period,capture_dev,playback_dev);

    audio_loop(&ctx);

    snd_pcm_close(ctx.capture);
    snd_pcm_close(ctx.playback);

    return 0;
}