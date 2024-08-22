#ifndef __PROC_DATA__
#define __PROC_DATA__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "random.h"

#define EPSILON 1e-9
#define DUDECT_NUMBER_PERCENTILES 100

typedef struct {
    double *execTime;
    double *weight;
    size_t size;
    size_t processed_size;
} result_t;

uint32_t get_random(int, uint32_t, uint32_t);
uint32_t *gen_data(size_t, uint32_t, uint32_t);
uint32_t *proc_sorted_data(uint32_t *, size_t, int, int);
uint32_t *proc_data_reverse(uint32_t *, size_t);
uint32_t *gen_test_case(size_t, int);
void printData(uint32_t *, size_t, int, int);
void outputData(char *, uint32_t *, size_t);
char *getPath(char *, size_t, int);
double proc_result(double *, const size_t);

void prepare_percentiles(double *, double *, size_t);
void update_data(result_t *, double *, size_t);
#endif
