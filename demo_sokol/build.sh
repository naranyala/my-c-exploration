#!/usr/bin/bash

gcc -o simple_calculator simple_calculator.c \
    -lGL -lX11 -lpthread -lm -ldl
