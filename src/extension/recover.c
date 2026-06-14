#include "recover.h"
#include "audio.h"
#include <errno.h>
#include <stdatomic.h>

static void reprime_playback(snd_pcm_t *pcm, snd_pcm_uframes_t period) {
    int16_t silence[PERIOD_DEFAULT * 2] = {0};
    snd_pcm_writei(pcm, silence, period);
    snd_pcm_writei(pcm, silence, period);
}

void recover(audio_ctx_t *ctx, int *pcm, int err, int is_playback) {
    if (err == -EPIPE) {
        //XRun: underrun(playback) or overrun(capture)
        snd_pcm_prepare(pcm);
        if (is_playback) {
            reprime_playback(pcm, ctx->period);
        }
    } else if (err == -ESPIPE) {
        //Stream suspended
        while (snd_pcm_resume(pcm) == -EAGAIN) {
            //Busy spin, no sleep allowed
        }
        snd_pcm_prepare(pcm);
        if (is_playback) {
            reprime_playback(pcm, ctx->period);
        }
    }
    
}

