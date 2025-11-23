
#define ERGO_SYNC_IMPLEMENTATION
#include "ergo_sync.h"
#include <pthread.h>
#include <stdio.h>

int main(){
    ergo_mutex m;
    ergo_cond c;
    ergo_mutex_init(&m);
    ergo_cond_init(&c);

    ergo_mutex_lock(&m);
    /* ... */
    ergo_cond_signal(&c);
    ergo_mutex_unlock(&m);
}
