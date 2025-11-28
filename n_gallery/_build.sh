#!/usr/bin/bash 

zig cc -o sdl2_approach sdl2_approach.c tinyfiledialogs.c -lSDL2 -std=c11 -o gallery

zig cc -o raylib_approach raylib_approach.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11  # Add -std=c99 if strict

zig cc -o glfw_approach glfw_approach.c -lglfw -lGLEW -lGL -lm



