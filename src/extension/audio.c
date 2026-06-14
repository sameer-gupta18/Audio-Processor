#include "audio.h"
#include "recover.h"
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <errno.h>



void rt_setup(void){
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0 ){
        fprintf(stderr,"FATAL: mlockall failed %s\n",strerror(errno));
        exit(1);
    }

    struct sched_param sp;
    memset(&sp, 0, sizeof(sp));
    sp.sched_priority = SCHED_PRIORITY; //Tells kernel to run the thread whenever it is ready

    int rc = pthread_setschedparam(pthread_self(),SCHED_FIFO,&sp);
    if (rc != 0){
        fprintf(stderr, "FATAL: pthread_setschedparam(SCHED_FIFO, 80) failed %s\n",strerror(rc));
        exit(1);
    }
}

void prime_playback(audio_ctx_t *ctx){
    int16_t silence[PERIOD_DEFAULT * 2] = {0};  //adds silence to the buffer
    snd_pcm_writei(ctx->playback,silence,ctx->period);
    snd_pcm_writei(ctx->playback,silence,ctx->period);
}

void audio_loop(audio_ctx_t *ctx){
    const snd_pcm_uframes_t period = ctx->period;

    for(;;){
        snd_pcm_sframes_t r = snd_pcm_readi(ctx -> capture, ctx->in_buf, period);
        if (r < 0){
            recover(ctx,ctx->capture,(int)r,0);
            continue;
        }

        for (snd_pcm_uframes_t i = 0; i < period; i++){
            ctx -> work_buf[i] = ctx -> in_buf[i] * SCALE_FACTOR;
        }

        //The processing will be called here


        for(snd_pcm_uframes_t i = 0; i < period; i++){
            float s = ctx -> work_buf[i];
            if (s > 1.0f) s = 1.0f;
            else if (s < -1.0f) s = 1.0f;

            int16_t v = (int16_t)(s / SCALE_FACTOR);
            ctx -> out_buf[2*i] = v;            //left
            ctx -> out_buf[2 * i + 1] = v;      //right
        }

        snd_pcm_sframes_t w = snd_pcm_writei(ctx->playback, ctx->out_buf, period);
        if (w < 0){
            recover(ctx, ctx->playback, (int)w, 1);
            continue;
        }
    }
}