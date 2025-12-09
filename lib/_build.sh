#!/usr/bin/bash

# zig cc -o tui_demo tui_demo.c $(pkg-config --cflags --libs ncurses)
# zig cc -o tui_snake tui_snake.c $(pkg-config --cflags --libs ncurses)
# zig cc -o tui_sysmonitor tui_sysmonitor.c $(pkg-config --cflags --libs ncurses)

# zig cc -o state_machine_demo state_machine_demo.c $(pkg-config --cflags --libs raylib)
zig cc -o state_machine_demo state_machine_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
zig cc -o color_utils_demo color_utils_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
zig cc -o orbit_math_demo orbit_math_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL

# zig cc -o collision_2d_demo collision_2d_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
zig cc -o shapes2d_demo shapes2d_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
zig cc -o shapes3d_demo shapes3d_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL

# zig cc -o tinyphys_2d_example tinyphys_2d_example.c -lraylib -lm -lpthread -ldl -lrt -lGL

zig cc -o mini_opt_example mini_opt_example.c
zig cc -o simple_fs_example simple_fs_example.c
zig cc -o event_bus_demo event_bus_demo.c

zig cc -o smart_rand_demo smart_rand_demo.c

zig cc -o gp2d_math_demo gp2d_math_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
zig cc -o gp2d_collision_demo gp2d_collision_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
zig cc -o gp3d_math_demo gp3d_math_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
zig cc -o gp3d_collision_demo gp3d_collision_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL

zig cc -o rxjs_inspired_demo rxjs_inspired_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
zig cc -o gp2d_particles_demo gp2d_particles_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
zig cc -o particle_made_sphere particle_made_sphere.c -lraylib -lm -lpthread -ldl -lrt -lGL

zig cc -o ano_reactivity_demo ano_reactivity_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
zig cc -o ano_calculator ano_calculator.c -lraylib -lm -lpthread -ldl -lrt -lGL
zig cc -o ano_snake_demo ano_snake_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL

zig cc -o deventbbus deventbud.c
zig cc -o dhash dhash.c
