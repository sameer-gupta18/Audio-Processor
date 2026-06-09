CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

EMULATE_OBJS = \
	emulator/emulate.o \
	emulator/load.o \
	emulator/decode.o \
	emulator/output.o \
	emulator/processor.o \
	emulator/utils.o \
	emulator/execute/branch.o \
	emulator/execute/execute_utils.o \
	emulator/execute/immediate.o \
	emulator/execute/register.o \
	emulator/execute/single_data_transfer.o

ASSEMBLE_OBJS = \
	assembler/assemble.o \
	assembler/encode.o \
	assembler/output.o \
	assembler/parser.o \
	assembler/reader.o \
	assembler/symtable.o \
# 	asssembler/
# 	asssembler/util.o \
# Linking Rules:
assemble: $(ASSEMBLE_OBJS)
	$(CC) $(CFLAGS) -o $@ $(ASSEMBLE_OBJS) 
emulate: $(EMULATE_OBJS)
	$(CC) $(CFLAGS) -o $@ $(EMULATE_OBJS)

clean:
	$(RM) $(EMULATE_OBJS) $(ASSEMBLE_OBJS) assemble emulate
	
