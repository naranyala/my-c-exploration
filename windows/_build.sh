#!/bin/bash

zig cc sample.c -o sample.exe

# zig cc scan_user_permissions.c -o scan_user_permissions.exe -lnetapi32 -ladvapi32

zig cc windows_info.c -o windows_info.exe
zig cc counter_demo.c -o counter_demo.exe
zig cc dirscan.c -o dirscan.exe
