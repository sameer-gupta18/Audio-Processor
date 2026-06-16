CC      = gcc
CFLAGS  = -O2 -Wall -Wextra -std=gnu11
LDFLAGS = -lasound -lpthread -lm -lgpiod
SRC     = main.c audio.c alsa_setup.c recover.c controls.c effects/volume.c state.c
BIN     = audiofx

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC) $(LDFLAGS)

clean:
	rm -f $(BIN)

.PHONY: all clean