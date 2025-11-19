#!/usr/bin/bash 

gcc -o tui_demo tui_demo.c $(pkg-config --cflags --libs ncurses)

gcc -o tui_snake tui_snake.c $(pkg-config --cflags --libs ncurses)

gcc -o tui_sysmonitor tui_sysmonitor.c $(pkg-config --cflags --libs ncurses)

