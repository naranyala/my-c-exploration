#!/usr/bin/bash 

gcc -o tui_demo tui_demo.c $(pkg-config --cflags --libs ncurses)
gcc -o tui_snake tui_snake.c $(pkg-config --cflags --libs ncurses)
gcc -o tui_sysmonitor tui_sysmonitor.c $(pkg-config --cflags --libs ncurses)

# gcc -o state_machine_demo state_machine_demo.c $(pkg-config --cflags --libs raylib)
gcc -o state_machine_demo state_machine_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
gcc -o color_utils_demo color_utils_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL

