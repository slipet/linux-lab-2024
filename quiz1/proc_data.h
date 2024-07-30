#ifndef __PROC_DATA__
#define __PROC_DATA__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "random.h"


uint32_t get_random(int, uint32_t, uint32_t);
uint32_t *gen_data(size_t, uint32_t, uint32_t);
uint32_t *proc_sorted_data(uint32_t *, size_t, int, int);
uint32_t *proc_data_reverse(uint32_t *, size_t);
uint32_t *gen_test_case(size_t, int);
void printData(uint32_t *, size_t, int, int);
void outputData(char *, uint32_t *, size_t);
char *getPath(char *, size_t, int);
#endif
