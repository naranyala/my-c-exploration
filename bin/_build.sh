#!/usr/bin/bash 

# Detect if targeting Windows
if [[ "$OSTYPE" == "msys"* || "$OSTYPE" == "win32"* || "$OSTYPE" == "cygwin"* ]]; then
    TARGET="-target x86_64-windows-gnu"
else
    TARGET=""
fi

zig cc -o status_clock status_clock.c
zig cc -o file_wordscount file_wordscount.c
zig cc -o extract_git_info extract_git_info.c
zig cc -o cpu_ram_usage cpu_ram_usage.c
zig cc -o sum_of_bandwidth sum_of_bandwidth.c
zig cc -o list_appdata list_appdata.c
zig cc -o xuptime xuptime.c
zig cc -o hrsize hrsize.c
# zig cc -o headonly_md_parser headonly_md_parser.c
zig cc -o scan_desktop_shortcut scan_desktop_shortcut.c
zig cc -o count_system_fonts count_system_fonts.c

zig cc -o targetdir_finder targetdir_finder.c $TARGET

