#!/usr/bin/bash 

gcc -o tui_demo tui_demo.c $(pkg-config --cflags --libs ncurses)
gcc -o tui_snake tui_snake.c $(pkg-config --cflags --libs ncurses)
gcc -o tui_sysmonitor tui_sysmonitor.c $(pkg-config --cflags --libs ncurses)

# gcc -o state_machine_demo state_machine_demo.c $(pkg-config --cflags --libs raylib)
gcc -o state_machine_demo state_machine_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
gcc -o color_utils_demo color_utils_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
gcc -o orbit_math_demo orbit_math_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL

# gcc -o collision_2d_demo collision_2d_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
gcc -o shapes2d_demo shapes2d_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
gcc -o shapes3d_demo shapes3d_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL

# gcc -o tinyphys_2d_example tinyphys_2d_example.c -lraylib -lm -lpthread -ldl -lrt -lGL

clang -o mini_opt_example mini_opt_example.c
clang -o simple_fs_example simple_fs_example.c
clang -o event_bus_demo event_bus_demo.c
clang -o smart_rand_demo smart_rand_demo.c

