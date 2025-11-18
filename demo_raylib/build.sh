#!/usr/bin/bash 


clang main.c -o main -lraylib -lm -lpthread -ldl -lrt -lGL

clang snake.c -o snake -lraylib -lm -lpthread -ldl -lrt -lGL

clang system_monitor.c -o system_monitor -lraylib -lm -lpthread -ldl -lrt -lGL

clang demo_sphere.c -o demo_sphere -lraylib -lm -lpthread -ldl -lrt -lGL

clang notification_sample.c -o notification_sample -lraylib -lm -lpthread -ldl -lrt -lGL
