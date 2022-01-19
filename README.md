# hw_chess_norbey
***A basic chess application written in C as a programming homework.***
Contains the CodeBlocks project and a pre-compiled version for windows (among releases).
To compile and run you need to have the SDL2 library at hand. They could be found on the official SDL website, or you can use https://infoc.eet.bme.hu/sdl_telepito/.
Debugmalloc has been excluded from the git release (a mandatory library for the homework to debug memory leaks).
Usage is pretty obvious, documentations will be uploaded as soon as I'm in the mood to translate them.
**Have fun!**
*gcc command: gcc -o <name> main.c display.c datasruct.c 'sdl2-config --cflags --libs' -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_gfx -lm*
