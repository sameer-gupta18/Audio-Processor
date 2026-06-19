#include "recover.h"
#include "audio.h"
#include <errno.h>
#include <stdatomic.h>

// tweo periods of silence at the beginning 
static void reprime_playback(snd_pcm_t *pcm, snd_pcm_uframes_t period) {
    int16_t silence[PERIOD * 2] = {0};
    snd_pcm_writei(pcm, silence, period);
    snd_pcm_writei(pcm, silence, period);
}

void recover(audio_ctx_t *ctx, snd_pcm_t *pcm, int err, int is_playback) {
    atomic_fetch_add(&ctx->shared->xruns, 1);
    
    if (err == -EPIPE) {
        snd_pcm_prepare(pcm);
        if (is_playback) {
            reprime_playback(pcm, ctx->period);
        }
    } else if (err == -ESTRPIPE) {
        //stream suspended
        while (snd_pcm_resume(pcm) == -EAGAIN) {
            //Busy spin, no sleep allowed
        }
        snd_pcm_prepare(pcm);
        if (is_playback) {
            reprime_playback(pcm, ctx->period);
        }
    }
    
}

