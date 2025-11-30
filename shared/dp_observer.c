
#include <stdio.h>
#include <stdlib.h>

typedef struct Observer Observer;
typedef struct Subject Subject;

typedef void (*NotifyCallback)(Observer *observer, const char *message);

typedef struct Observer {
    NotifyCallback callback;
} Observer;

typedef struct Subject {
    Observer **observers;
    int numObservers;
} Subject;

// Initialize a subject
void initializeSubject(Subject *subject, int maxObservers) {
    subject->observers = (Observer **)malloc(maxObservers * sizeof(Observer *));
    subject->numObservers = 0;
}

// Add an observer to the subject
void addObserver(Subject *subject, Observer *observer) {
    subject->observers[subject->numObservers++] = observer;
}

// Notify all observers
void notifyObservers(Subject *subject, const char *message) {
    for (int i = 0; i < subject->numObservers; i++) {
        subject->observers[i]->callback(subject->observers[i], message);
    }
}

// Example observer callback
void exampleObserverCallback(Observer *observer, const char *message) {
    printf("Observer received message: %s\n", message);
}
