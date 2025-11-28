#!/usr/bin/bash 


# the source need to be updated based on new reactivity
#
# zig cc calculator.c -o calculator -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
# zig cc system_monitor.c -o system_monitor -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
# zig cc demo_sphere.c -o demo_sphere -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
# zig cc event_driven_notif.c -o event_driven_notif -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
# zig cc two_circles.c -o two_circles -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
# zig cc analog_clock.c -o analog_clock -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
# zig cc -o task_manager_like task_manager_like.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
# zig cc -o counter_buttons counter_buttons.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
# zig cc -o simple_physics simple_physics.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL

# updated reactivity
zig cc -o snake snake.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o breakout breakout.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o random_math random_math.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o random_vec2 random_vec2.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o random_vec3 random_vec3.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o physics_3d physics_3d.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o animation_demo animation_demo.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o shooting_stars shooting_stars.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o cmatrix_remake cmatrix_remake.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o easing_demo easing_demo.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
# zig cc -o linear_algebra_demo linear_algebra_demo.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL

zig cc -o simple_2d_shooter simple_2d_shooter.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o event_driven_keyboard event_driven_keyboard.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o system_bus_demo system_bus_demo.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o mindmap_demo mindmap_demo.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o particle_demo particle_demo.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o tesseract_demo tesseract_demo.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL

# zig cc -o space_invader space_invader.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
# zig cc -o space_wormhole space_wormhole.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL
zig cc -o earth_wireframe earth_wireframe.c -lraylib -lGL -lm -lpthread -ldl -lrt -lGL



