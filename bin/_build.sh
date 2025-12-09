#!/usr/bin/bash 

zig cc -o status_clock status_clock.c
zig cc -o file_wordscount file_wordscount.c
zig cc -o extract_git_info extract_git_info.c
zig cc -o cpu_ram_usage cpu_ram_usage.c
zig cc -o sum_of_bandwidth sum_of_bandwidth.c
zig cc -o list_appdata list_appdata.c
zig cc -o xuptime xuptime.c
zig cc -o hrsize hrsize.c
zig cc -o headonly_md_parser headonly_md_parser.c
zig cc -o scan_desktop_shortcut scan_desktop_shortcut.c

