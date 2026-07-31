# audiofx


<p align="center">
  <img width="571" height="514" alt="pi" src="https://github.com/user-attachments/assets/250e7cfb-406b-4316-9814-7ef913fb5a50" />
  <br />
  <em>Complete physical configuration</em>
</p>


*AudioFx* is a real-time, programmable audio effects processor written in C that runs locally on a Raspberry Pi, and interacts directly with the Linux ALSA subsystem. Audio is captured from a microphone, processed through a chain of effects, and dispatched to a playback device with low latency (approx. < 25ms). Five rotary encoders substitute as control dials, and an SSD1306 OLED displays the current intensity and mute state of each effect.

## Effects

Five effects are applied in series, each mapped to its own rotary encoder:

| Effect | Description |
| --- | --- |
| Volume | Simple quadratic scaling of output amplitude from 0× to 2×. |
| Reverb | Schroeder reverberator (parallel comb filters into series all-pass filters). |
| Distortion | Non-linear waveshaping to add noise and disturbance. |
| Tremolo | Amplitude modulation via a sine Low Frequency Oscillator. |
| Chorus | Modulated and layered short delays for a harmonic sound. |

Each effect's intensity is set with its encoder (0–100%, in steps of 2.5%). Pushing an encoder's button toggles mute for that effect.

## Architecture

- **Audio pipeline** (`audio.c`, `alsa_setup.c`): two ALSA PCM streams (capture + playback) are opened at a 44100 Hz sample rate with a period of 512 frames. The playback buffer is primed with two periods of silence (~20 ms), absorbing input/output jitter. A third period of headroom avoids triggering recovery unnecessarily.
- **Concurrency**: the audio thread (spun off from `main`) continuously reads, processes, and writes audio. The main loop acts as the control thread, polling the encoders every 1 ms. The two threads communicate through a `shared_state` struct (`state.c`/`state.h`) that holds arrays of atomic variables for each effect's intensity and mute flag.
- **Controls** (`controls.c`): rotary encoders are read via `libgpiod` (`/dev/gpiochip0`) using a canonical quadrature decode table. 
- **Recovery** (`recover.c`): playback underruns and capture overruns are caught and the buffer is filled with two periods of silence.
- **Display** (`display.c`): Uses an SSD1306 OLED screen over I²C.

The capture device defaults to `plughw:CARD=SF558` and the playback device to `plughw:CARD=Headphones` (stereo); if you have different hardware, change these hardcoded settings in main.c.

## Setup

### Hardware

The modular nature of the code means you can disable certain features and effects if you wish. However,
the original implementation is as follows:

- Raspberry Pi (running Raspberry Pi OSLite)
- USB/I²S microphone (capture device)
- Audio playback device (e.g. headphones / speaker)
- 5 × rotary encoder (with integrated push-button)
- 1 × SSD1306 128×64 OLED (I²C)
- Breadboard and jumper wires

### Wiring

Each rotary encoder uses three lines (`CLK`, `DT`, and `SW`), plus power and ground. N.b. The `SW` lines use the Pi's internal pull-up resistors.

| Effect | CLK | DT | SW |
| --- | --- | --- | --- |
| Volume | GPIO17 | GPIO27 | GPIO22 |
| Reverb | GPIO8 | GPIO9 | GPIO11 |
| Distortion | GPIO23 | GPIO24 | GPIO25 |
| Tremolo | GPIO16 | GPIO20 | GPIO21 |
| Chorus | GPIO4 | GPIO18 | GPIO7 |

The SSD1306 OLED is connected over I²C bus 1 at address `0x3c`:

| OLED pin | Raspberry Pi |
| --- | --- |
| SDA | GPIO2 (SDA1, physical pin 3) |
| SCL | GPIO3 (SCL1, physical pin 5) |
| VCC | 3V3 |
| GND | GND |

You must ensure I²C is enabled (`raspi-config`) so that `/dev/i2c-1` is present.

### Dependencies

```sh
sudo apt install gcc make libasound2-dev libgpiod-dev
```

The build links against `libasound`, `libpthread`, `libm`, and `libgpiod`.

### Build & run

```sh
cd audiofx
make
./audiofx
```

### Testing & Use Case

Ensure that all changes in dials are reflected in logs/screen readings and that each effect, individually, renders the desired output. The most interesting part of the project, however, is the freedom and flexibility it gives you in combining and chaining the effects to suit a user's need. 

## Licensing & Acknowledgements

This project uses lib-ssd1306 by armlabs, licensed under the MIT License. See lib-ssd1306/LICENSE for the full license text.

