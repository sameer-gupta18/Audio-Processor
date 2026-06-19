#include "audio.h"
#include "effects/effect.h"
#include "recover.h"
#include "state.h"
#include <sched.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#define RATE 44100


// initialise audio context
void ctx_init(audio_ctx_t *ctx){
    memset(ctx, 0, sizeof(*ctx));
    ctx->effects[0] = fx_volume_create(RATE); 
    ctx->effects[1] = fx_reverb_create(RATE);
    ctx->effects[2] = fx_distortion_create(RATE); 
    ctx->effects[3] = fx_tremolo_create(RATE);  
    ctx->effects[4] = fx_chorus_create(RATE); 
    ctx->num_effects = 5;     
}

// set up audio thread
void *audio_thread_set_up(void* args){
    audio_ctx_t *ctx = args;
    // make sure program pages cannot be swapped out
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0 ){
        fprintf(stderr,"mlockall failed");
        exit(1);
    }

    struct sched_param sp;
    memset(&sp, 0, sizeof(sp));
    sp.sched_priority = SCHED_PRIORITY; //Tells kernel to run the thread whenever it is ready

    int res = pthread_setschedparam(pthread_self(),SCHED_FIFO,&sp);
    if (res != 0){
        fprintf(stderr, "scheduling thread failed %s\n",strerror(res));
        exit(1);
    }

    audio_loop(ctx);
    return NULL; 
}

// adding two periods of silence
void start_playback(audio_ctx_t *ctx){
    int16_t silence[PERIOD * 2] = {0};  //adds silence to the buffer
    snd_pcm_writei(ctx->playback,silence,ctx->period);
    snd_pcm_writei(ctx->playback,silence,ctx->period);
}

// main audio loop 
void audio_loop(audio_ctx_t *ctx){
    const snd_pcm_uframes_t period = ctx->period;
    for(;;){
        snd_pcm_sframes_t r = snd_pcm_readi(ctx -> capture, ctx->in_buf, period);
        if (r < 0){
            recover(ctx,ctx->capture,(int)r,0);
            continue;
        }

        for (snd_pcm_uframes_t i = 0; i < period; i++){
            ctx -> work_buf[i] = ctx -> in_buf[i] * IN_SCALE;
        }
        // apply effects -- additional smoothing unecessary 
        for (int e = 0; e < ctx->num_effects; e++){
            if (ctx->effects[e]){
                int ticks = atomic_load(&ctx->shared->intensity[e]);
                int muted = atomic_load(&ctx->shared->muted[e]);
                float intensity = ticks/1000.0;
                if(ctx->prev_muted[e] && !muted){
                    ctx->effects[e]->reset(ctx->effects[e]);
                } 
                ctx->prev_muted[e] = muted; 
                if(!muted){
                    ctx->effects[e]->process(ctx->effects[e],ctx->work_buf,period,intensity);
                } else{
                    ctx->effects[e]->process(ctx->effects[e],ctx->work_buf,period,0);
                }
            }
        }
        for(snd_pcm_uframes_t i = 0; i < period; i++){
            float s = ctx -> work_buf[i];
            if (s > 1.0f) s = 1.0f;
            else if (s < -1.0f) s = -1.0f;
            int16_t v = (int16_t)(s * OUT_SCALE);
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
