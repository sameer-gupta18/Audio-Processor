CC      = gcc
CFLAGS  = -O2 -Wall -Wextra -std=gnu11
LDFLAGS = -lasound -lpthread -lm -lgpiod
SRC     = main.c audio.c alsa_setup.c recover.c \
		  controls.c effects/volume.c effects/reverb.c \
		  effects/distortion.c effects/tremolo.c effects/chorus.c state.c \
		  display.c lib-ssd1306/linux_i2c.c lib-ssd1306/ssd1306.c 
		 
BIN     = audiofx

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC) $(LDFLAGS)

clean:
	rm -f $(BIN)

.PHONY: all clean