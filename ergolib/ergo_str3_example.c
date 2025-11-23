#define ERGO_STR_IMPLEMENTATION
#include "ergo_str3.h"
#include <stdio.h>

int main(void){
    char s[] = "   hello world  \n";
    printf("before:'%s'\n", s);
    ergo_str_trim(s);
    printf("after: '%s'\n", s);

    ergo_dstring d;
    ergo_dstr_init(&d);
    ergo_dstr_appendf(&d, "number=%d name=%s", 42, "alice");
    printf("dstring: %s\n", d.buf);
    ergo_dstr_free(&d);
    return 0;
}

