#!/usr/bin/bash 


cc main.c -o main -lraylib -lm -lpthread -ldl -lrt -lGL

cc snake.c -o snake -lraylib -lm -lpthread -ldl -lrt -lGL

cc system_monitor.c -o system_monitor -lraylib -lm -lpthread -ldl -lrt -lGL
