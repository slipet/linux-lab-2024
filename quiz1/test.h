#ifndef __TEST__
#define __TEST__
#include <assert.h>
#include <bits/types.h>
#include <gsl/gsl_cdf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include "linux_list.h"
#include "list.h"
#include "proc_data.h"
// static double first_time, last_time;
// struct timeval tv;

#define INIT_PATH_T(ROOT, PREFIX, SUFFIX, TAG)                     \
    &(Path_t)                                                      \
    {                                                              \
        .root = &(ROOT), .prefix = &(PREFIX), .suffix = &(SUFFIX), \
        .tag = &(TAG)                                              \
    }

typedef struct {
    char *root;
    char *prefix;
    char *suffix;
    char *tag;
    char data_size[20];
} Path_t;

typedef struct {
    uint32_t *data;
    size_t size;
} testCases;

typedef struct {
    void (*algorithm)(void **);
    void *(*list_construct)(void *, void *);
    void (*printList)(void *);
    void (*list_free)(void *);
    int (*validation)(void *);
    char *algoName;
} expConfig;

void init_time(double *);
double delta_time(double *);
double test_list(expConfig *, testCases *);
double test_linux_list(expConfig *, testCases *);
testCases *init_testcases(uint32_t *, size_t);
void free_testcases(testCases *);
uint32_t *read_testcases(char *, uint32_t *);
char *getFileName(Path_t *, size_t);
void fixed_quantity_test(expConfig *, Path_t *, const size_t, const size_t);
void random_quantity_test(expConfig *,
                          Path_t *,
                          const size_t,
                          const size_t,
                          const size_t,
                          int);
void repeated_fixed_quantity_test(expConfig *,
                                  Path_t *,
                                  const size_t,
                                  const size_t,
                                  const size_t,
                                  int);

double calculate_mean(double *, double *, size_t);
double calculate_stddev(double *, double *, size_t, double);
double calculate_groups_stddev(TEST_METHOD_T *, double);
double calculate_Fvalue(TEST_RESULT_T *);

#endif