header
------
Obviously, all header files (*.h) should live in this folder.

source
------
And in this folder you should put standard source files (*.c). Note that
to include the header files use

	#include "../header/file.h"

to include file.h.

include libraries (-l)
----------------------
Every extra library the compiler needs to link to with the flags -l
should be in this list.

	- pthread
	- SDL
	- SDLmain
	- SDL_ttf
	- SDL_net
	- SDL_image
	- GL
	- GLU

internal dependencies
---------------------
List every module that one module needs to work. That is, if file.h
includes someheader.h, it should say so in this list.

NOTE: AVOID CIRCULAR DEPENDENCIES!!!

	File			Depends on
	==================================================
	main			gfxinit communication lobby menu
	lobby			gfxinit communication gameplay
	menu			gfxinit communication
	gameplay		gfxinit communication
	communication	gfxinit
	gfxinit
	

make
----
To compile everything, simply run the command:

	> make

To compile a specific object (module), append it's name to the
command, e.g.:

	> make module.o

To remove every file the compiler creates run:

	> make clean

Make currently is set to compile with debugging options. This means 
the program "gdb" can testrun the application (replace "executable"
with the name of the executable):

	> gdb executable

Then in gdb (the prompt will be: (gdb) ) type "run":

	(gdb) run

And if there are any errors it will hopefully detect them. Note that
if gdb doesn't have everything it needs, it will probably alert you
with something containing "debuginfo-install". It's recommended run
that command (with the parameters listed).

Config file:
A config file exists and work like this, write on exactly 3 lines: 
name
ipaddress
port

Also, pressing enter in the menu saves that input to the file.
Max characters are for each line, 10 15 and 5. 


