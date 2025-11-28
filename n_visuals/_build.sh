#!/usr/bin/bash

zig cc -o presentation_01 presentation_01.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o presentation_02 presentation_02.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
