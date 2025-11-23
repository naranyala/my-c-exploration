#!/usr/bin/bash 


# the source need to be updated based on new reactivity
#
# clang calculator.c -o calculator -lraylib -lm -lpthread -ldl -lrt -lGL
# clang system_monitor.c -o system_monitor -lraylib -lm -lpthread -ldl -lrt -lGL
# clang demo_sphere.c -o demo_sphere -lraylib -lm -lpthread -ldl -lrt -lGL
# clang event_driven_notif.c -o event_driven_notif -lraylib -lm -lpthread -ldl -lrt -lGL
# clang two_circles.c -o two_circles -lraylib -lm -lpthread -ldl -lrt -lGL
# clang analog_clock.c -o analog_clock -lraylib -lm -lpthread -ldl -lrt -lGL
# clang -o task_manager_like task_manager_like.c -lraylib -lm -lpthread -ldl -lrt -lGL
# clang -o counter_buttons counter_buttons.c -lraylib -lm -lpthread -ldl -lrt -lGL


# updated reactivity
clang -o snake snake.c -lraylib -lm -lpthread -ldl -lrt -lGL
clang -o breakout breakout.c -lraylib -lm -lpthread -ldl -lrt -lGL
clang -o random_math random_math.c -lraylib -lm -lpthread -ldl -lrt -lGL
clang -o random_vec2 random_vec2.c -lraylib -lm -lpthread -ldl -lrt -lGL
clang -o random_vec3 random_vec3.c -lraylib -lm -lpthread -ldl -lrt -lGL
clang -o simple_physics simple_physics.c -lraylib -lm -lpthread -ldl -lrt -lGL
clang -o physics_3d physics_3d.c -lraylib -lm -lpthread -ldl -lrt -lGL
clang -o animation_demo animation_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
clang -o shooting_stars shooting_stars.c -lraylib -lm -lpthread -ldl -lrt -lGL
clang -o cmatrix_remake cmatrix_remake.c -lraylib -lm -lpthread -ldl -lrt -lGL
clang -o easing_demo easing_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL
clang -o linear_algebra_demo linear_algebra_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL

clang -o simple_2d_shooter simple_2d_shooter.c -lraylib -lm -lpthread -ldl -lrt -lGL
clang -o event_driven_keyboard event_driven_keyboard.c -lraylib -lm -lpthread -ldl -lrt -lGL
clang -o system_bus_demo system_bus_demo.c -lraylib -lm -lpthread -ldl -lrt -lGL


