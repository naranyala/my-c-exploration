/* better_chan.h */
#ifndef BETTER_CHAN_H
#define BETTER_CHAN_H

typedef struct b_chan_t b_chan;

// Create a channel with infinite capacity (linked list)
b_chan* bc_create(void);
// Push a pointer into the channel (Thread safe)
void bc_send(b_chan* c, void* data);
// Pop a pointer (Blocks if empty until data arrives)
void* bc_recv(b_chan* c);
// Clean up
void bc_destroy(b_chan* c);

#endif

/* IMPLEMENTATION */
#ifdef BETTER_CHAN_IMPLEMENTATION
#include <pthread.h>
#include <stdlib.h>

struct b_node {
    void* data;
    struct b_node* next;
};

struct b_chan_t {
    struct b_node *head, *tail;
    pthread_mutex_t m;
    pthread_cond_t  cv;
};

b_chan* bc_create(void) {
    b_chan* c = malloc(sizeof(b_chan));
    c->head = c->tail = NULL;
    pthread_mutex_init(&c->m, NULL);
    pthread_cond_init(&c->cv, NULL);
    return c;
}

void bc_send(b_chan* c, void* data) {
    struct b_node* n = malloc(sizeof(struct b_node));
    n->data = data;
    n->next = NULL;

    pthread_mutex_lock(&c->m);
    if (c->tail) c->tail->next = n;
    else c->head = n;
    c->tail = n;
    pthread_cond_signal(&c->cv);
    pthread_mutex_unlock(&c->m);
}

void* bc_recv(b_chan* c) {
    pthread_mutex_lock(&c->m);
    while (c->head == NULL) {
        pthread_cond_wait(&c->cv, &c->m);
    }
    struct b_node* n = c->head;
    void* data = n->data;
    c->head = n->next;
    if (c->head == NULL) c->tail = NULL;
    pthread_mutex_unlock(&c->m);
    
    free(n);
    return data;
}

void bc_destroy(b_chan* c) {
    // Note: In real usage, drain queue first
    pthread_mutex_destroy(&c->m);
    pthread_cond_destroy(&c->cv);
    free(c);
}
#endif
