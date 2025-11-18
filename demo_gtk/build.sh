#!/usr/bin/bash

gcc counter.c -o counter \
    -I/usr/include/gtk-3.0 \
    -I/usr/include/glib-2.0 \
    -I/usr/lib/x86_64-linux-gnu/glib-2.0/include \
    -I/usr/include/pango-1.0 \
    -I/usr/include/atk-1.0 \
    -I/usr/include/cairo \
    -I/usr/include/gdk-pixbuf-2.0 \
    -lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -latk-1.0 \
    -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 \
    -Wall -O2
