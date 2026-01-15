#ifndef THREAD_H
#define THREAD_H

typedef struct {
    int id;
    int burst;
    int priority;
    int group;   // <-- EKLE (process/thread grubu)
} Thread;


#endif

