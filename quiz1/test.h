#ifndef __TEST__
#define __TEST__
#include <bits/types.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

static double first_time, last_time;
struct timeval tv;

typedef struct {
    int *data;
    size_t size;
} testCases;

typedef struct {
    void (*algorithm)(void **);
    int (*validation)(void *);
    void (*printList)(void *);
    void *(*list_construct)(void *, void *);
    void (*list_free)(void *);

} expConfig;

void init_time(double *);
double delta_time(double *);

#endif