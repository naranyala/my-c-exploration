#!/usr/bin/bash 


clang calculator.c -o calculator -lraylib -lm -lpthread -ldl -lrt -lGL

clang snake.c -o snake -lraylib -lm -lpthread -ldl -lrt -lGL

clang system_monitor.c -o system_monitor -lraylib -lm -lpthread -ldl -lrt -lGL

clang demo_sphere.c -o demo_sphere -lraylib -lm -lpthread -ldl -lrt -lGL

clang event_driven_notif.c -o event_driven_notif -lraylib -lm -lpthread -ldl -lrt -lGL

clang two_circles.c -o two_circles -lraylib -lm -lpthread -ldl -lrt -lGL

clang analog_clock.c -o analog_clock -lraylib -lm -lpthread -ldl -lrt -lGL

clang -o task_manager_like task_manager_like.c -lraylib -lm -lpthread -ldl -lrt -lGL

clang -o counter_buttons counter_buttons.c -lraylib -lm -lpthread -ldl -lrt -lGL
