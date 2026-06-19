#include "alsa_setup.h"
#include <alsa/error.h>
#include <alsa/pcm.h>
#include <stdio.h>

// configure the current pcm (main audio) stream for input or output 
int open_and_configure(
    snd_pcm_t **out, 
    const char *device, 
    snd_pcm_stream_t stream, 
    unsigned int channels,
    unsigned rate,
    snd_pcm_uframes_t period
){
    snd_pcm_t *res; 
    // hardware params
    snd_pcm_hw_params_t *hw;
    int err = snd_pcm_open(&res, device, stream, 0);
    if(err < 0){
        fprintf(stderr, "could not open device\n");
    
        return err; 
    }
    // configure and apply hardware params:
    snd_pcm_hw_params_alloca(&hw); 
    snd_pcm_hw_params_any(res, hw);
    snd_pcm_hw_params_set_access(res, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(res, hw, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(res, hw, channels); 
    unsigned temp_r = rate; 
    snd_pcm_hw_params_set_rate_near(res, hw, &temp_r, 0);
    snd_pcm_uframes_t temp_p = period; 
    snd_pcm_hw_params_set_period_size_near(res, hw,&temp_p, 0); 
    snd_pcm_uframes_t bufsz = 3 * period;
    snd_pcm_hw_params_set_buffer_size_near(res, hw, &bufsz);
    err = snd_pcm_hw_params(res, hw); 
    if(err < 0){
        fprintf(stderr, "hardware params: %s\n",snd_strerror(err));
        snd_pcm_close(res);
        return err; 
    }
    // check returned params 
    snd_pcm_uframes_t actual_p; 
    snd_pcm_hw_params_get_period_size(hw,&actual_p, 0);
    if(actual_p != period){
        fprintf(stderr, "Period mismatch. Init: %lu Curr: %lu\n",
                period, actual_p);
        snd_pcm_close(res);
        return -1; 
    }    
    if(temp_r!=rate){
        fprintf(stderr, "Rate mismatch. Init: %u, Curr: %u\n", rate, temp_r);
        snd_pcm_close(res);
        return -1;
    }
    *out = res;
    return 0; 
}

// configuring software params
int set_playback_startup(snd_pcm_t *pcm, snd_pcm_uframes_t period){
    snd_pcm_sw_params_t *sw; 
    snd_pcm_sw_params_alloca(&sw);
    snd_pcm_sw_params_current(pcm, sw); 
    snd_pcm_sw_params_set_start_threshold(pcm, sw, period*2); 
    snd_pcm_sw_params(pcm, sw);
    return 0; 
}

