# # # # #
# Makefile for Pong Server
#
# Add your name to this list if you edit this file.
# Edited by: simon
#
# How to use:
#
# > make [module].o		# this will compile [module].c
#
# > make all			# this will compile all modules
#
# > make				# this will create the executable
# 

# Build type, "debug" or "production".
#BUILD_TYPE := debug
BUILD_TYPE := production

# Cleaning command.
RM := rm -rf
# Compiler.
CC := gcc
# Where to find our code.
VPATH := ./source:./header
# A list of every module.
OBJECTS := jobs.o lobby.o vector.o collision.o communication.o main.o \
	gameplay.o
# GCC specific flags.
ifeq ($(BUILD_TYPE), debug)
  CFLAGS := -pedantic -Wall -g3
else
  CFLAGS := -pedantic -Wall -O2
endif
# Libraries that's needed for this to work.
LIBS := -lpthread -lm -lrt

.PHONY: clean all server

# Build executable.
mpongserver: all
	@echo "Building target $@..."
	$(CC) $(CFLAGS) $(OBJECTS) -o "mpongserver" $(LIBS)
ifeq ($(BUILD_TYPE), production)
	strip $@
endif
	@echo " "

# Build all objects.
all: $(OBJECTS)

# Build main.o.
main.o: main.c lobby.o
	@echo "Building target $@..."
	$(CC) $(CFLAGS) -c $< -o $@
	@echo " "

# Build lobby.o
lobby.o: lobby.c lobby.h gameplay.o
	@echo "Building target $@..."
	$(CC) $(CFLAGS) -c $< -o $@
	@echo " "

# Build gameplay.o.
gameplay.o: gameplay.c gameplay.h communication.o jobs.o
	@echo "Building target $@..."
	$(CC) $(CFLAGS) -c $< -o $@
	@echo " "

# Build communication.o.
communication.o: communication.c communication.h collision.o
	@echo "Building target $@..."
	$(CC) $(CFLAGS) -c $< -o $@
	@echo " "

# Build collision.o.
collision.o: collision.c collision.h vector.o
	@echo "Building target $@..."
	$(CC) $(CFLAGS) -c $< -o $@
	@echo " "

# Build for default objects.
%.o: %.c %.h
	@echo "Building target $@..."
	$(CC) $(CFLAGS) -c $< -o $@
	@echo " "

# Clean up.
clean:
	@echo "Cleaning..."
	$(RM) *.o mpongserver server
	@echo " "

