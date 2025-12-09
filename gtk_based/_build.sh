#!/usr/bin/bash


zig cc -o gui_shortcut_editor gui_shortcut_editor.c `pkg-config --cflags --libs gtk+-3.0`
zig cc -o gtkwrapper_demo gtkwrapper_demo.c `pkg-config --cflags --libs gtk+-3.0`
zig cc -o collapsible_demo collapsible_demo.c `pkg-config --cflags --libs gtk+-3.0`
zig cc -o modal_backdrop_example modal_backdrop_example.c `pkg-config --cflags --libs gtk+-3.0`
