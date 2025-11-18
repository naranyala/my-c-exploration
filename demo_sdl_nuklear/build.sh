#!/usr/bin/bash

gcc counter.c -o counter \
    -lSDL2 -lGL -lGLEW \
    -I../vendor/Nuklear/src/ \
    -I/path/to/nuklear/demo/sdl_opengl3


# gcc main.c -o counter_demo \
#     -framework OpenGL -lSDL2 -lGLEW \
#     -I/path/to/nuklear -I/path/to/nuklear/demo/sdl_opengl3

