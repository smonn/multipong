# # # # #
# Makefile for Pong Client
#
# Work in progress...
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
OBJECTS := main.o lobby.o menu.o gameplay.o gfxinit.o communication.o
# GCC specific flags.
ifeq ($(BUILD_TYPE), debug)
  CFLAGS := `sdl-config --cflags` -pedantic -Wall -g3
else
  CFLAGS := `sdl-config --cflags` -pedantic -Wall -O2
endif
# Libraries that's needed for this to work.
LIBS := `sdl-config --libs` -lSDL_ttf -lSDLmain -lSDL_net -lSDL_image\
	-lGL -lGLU

.PHONY: all clean client msg

# Build executable
multipong: msg all
	@echo "Building target $@..."
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LIBS)
ifeq ($(BUILD_TYPE), production)
	strip $@
endif
	@echo " "

# Build all objects.
all: $(OBJECTS)

# Just a silly message.
msg:
	@echo "Building everything..."
	@echo " "

# Default for building objects.
%.o: %.c %.h
	@echo "Building target $@..."
	$(CC) $(CFLAGS) -c $< -o $@
	@echo " "

# Build main.o
main.o: main.c main.h lobby.o communication.o menu.o gfxinit.o
	@echo "Building target $@..."
	$(CC) $(CFLAGS) -c $< -o $@
	@echo " "

# Build lobby.o
lobby.o: lobby.c lobby.h gfxinit.o menu.o communication.o gameplay.o
	@echo "Building target $@..."
	$(CC) $(CFLAGS) -c $< -o $@
	@echo " "

# Build menu.o
menu.o: menu.c menu.h gfxinit.o communication.o
	@echo "Building target $@..."
	$(CC) $(CFLAGS) -c $< -o $@
	@echo " "

# Build gfxinit.o
gfxinit.o: gfxinit.c gfxinit.h
	@echo "Building target $@..."
	$(CC) $(CFLAGS) -c $< -o $@
	@echo " "

# Build communication.o
communication.o: communication.c communication.h gfxinit.o
	@echo "Building target $@..."
	$(CC) $(CFLAGS) -c $< -o $@
	@echo " "

# Build gameplay.o
gameplay.o: gameplay.c gameplay.h gfxinit.o communication.o
	@echo "Building target $@..."
	$(CC) $(CFLAGS) -c $< -o $@
	@echo " "

# Clean up.
clean:
	@echo "Cleaning..."
	$(RM) *.o multipong client 
	@echo " "

