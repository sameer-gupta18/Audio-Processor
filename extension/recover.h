#ifndef RECOVER_H
#define RECOVER_H

#include "audio.h"

void recover(audio_ctx_t *ctx, snd_pcm_t *pcm, int err, int is_playback);

#endif

