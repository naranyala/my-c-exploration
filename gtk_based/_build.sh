#!/usr/bin/bash


# zig cc -o gtk_rotating_cube gtk_rotating_cube.c \
# gcc -o gtk_rotating_cube gtk_rotating_cube.c ../vendor/glad/src/glad.c \
#     `pkg-config --cflags gtk+-3.0` \
#     `pkg-config --libs   gtk+-3.0` \
#     -I../vendor/glad/include -I../vendor/cglm \
#     -ldl -lm -Wall -Wextra -O2


zig cc -o gui_shortcut_editor gui_shortcut_editor.c `pkg-config --cflags --libs gtk+-3.0`
zig cc -o gtkwrapper_demo gtkwrapper_demo.c `pkg-config --cflags --libs gtk+-3.0`
zig cc -o collapsible_demo collapsible_demo.c `pkg-config --cflags --libs gtk+-3.0`
zig cc -o modal_backdrop_example modal_backdrop_example.c `pkg-config --cflags --libs gtk+-3.0`


# gcc -o markdown_reader markdown_reader.c \
#     `pkg-config --cflags --libs gtk+-3.0` \
#     `pkg-config --cflags --libs webkit2gtk-4.0` \
#     # `pkg-config --cflags --libs cmark` \
#     # -rdynamic


