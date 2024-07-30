#include "proc_data.h"

/* A better seed can be obtained by combining getpid() and its parent ID
 * with the Unix time.
 */
uint32_t get_random(int rand_flag, uint32_t lo, uint32_t hi)
{
    uint32_t tmp, range;
    range = (hi - lo) + 1;
    switch (rand_flag) {
    case 0:
        randombytes((uint8_t *) &tmp, sizeof(tmp));
        tmp = tmp % range + lo;
        break;

    default:
        break;
    }
    return tmp;
}

uint32_t *gen_data(size_t size, uint32_t lo, uint32_t hi)
{
    srand(time(NULL));
    uint32_t *data = (uint32_t *) malloc(sizeof(uint32_t) * ((uint32_t) size));
    for (int i = 0; i < size; i++) {
        data[i] = get_random(0, lo, hi);
    }
    return data;
}
// For qsort
static inline int compare(const void *arg1, const void *arg2)
{
    return (*(uint32_t *) arg2 - *(uint32_t *) arg1);
}

uint32_t *proc_sorted_data(uint32_t *data, size_t size, int groups, int sorted)
{
    if (!sorted)
        return data;
    int remainder = size % groups;
    int idx = 0;
    for (int i = 0; i < groups; i++) {
        int group_size = size / groups + (i < remainder);
        int *group_data = &data[idx];
        qsort((void *) data, group_size, sizeof(uint32_t), compare);
        idx += group_size;
    }
    return data;
}

uint32_t *proc_data_reverse(uint32_t *data, size_t size)
{
    uint32_t *ret = (uint32_t *) malloc(sizeof(uint32_t) * size);
    qsort((void *) data, size, sizeof(uint32_t), compare);
    int flag = 0;
    int l = 0, r = size - 1;
    for (int i = size - 1; i >= 0; i--) {
        if (!flag)
            ret[r--] = data[i];
        else
            ret[l++] = data[i];
        flag = !flag;
    }
    for (int i = 0; i < size; i++) {
        data[i] = ret[i];
    }
    free(ret);
    return data;
}

void printData(uint32_t *data, size_t size, int groups, int sorted)
{
    int remainder = size % groups;
    printf("Groups: %d, Sorted: %s\n", groups, sorted ? "Yes" : "No");
    printf("Size: %u\n", size);
    printf("Data: \n");
    int idx = 0;
    for (int i = 0; i < groups; i++) {
        int group_size = size / groups + (i < remainder);
        for (int j = 0; j < group_size; j++) {
            printf("%u ", data[idx++]);
        }
        printf("\n");
    }
}

void outputData(char *path, uint32_t *data, size_t size)
{
    FILE *fp = fopen(path, "w");
    if (!fp) {
        fprintf(stderr, "Error opening file: %s\n", path);
        return;
    }
    fprintf(fp, "%u ", size);
    for (int i = 0; i < size; i++) {
        fprintf(fp, "%u ", data[i]);
    }
    fprintf(fp, "\n");
    fclose(fp);
}

char *getPath(char *path, size_t size, int ordered)
{
    char *tag, *outputpath;
    char file[] = ".data";
    char sorted[] = "sorted_";
    char reverse[] = "reverse_";
    char random[] = "random_";
    char data_size[20];
    sprintf(data_size, "%d\0", size);


    switch (ordered) {
    case 0:  // reverse
        tag = reverse;
        break;
    case 1:  // sorted
        tag = sorted;
        break;
    case 2:  // random
        tag = random;
        break;
    default:
        break;
    }

    size_t path_len = strlen(path);
    size_t tag_len = strlen(tag);
    size_t data_size_len = strlen(data_size);
    size_t file_len = strlen(file);

    outputpath = malloc(path_len + tag_len + data_size_len + file_len + 1);
    if (outputpath == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    strcpy(outputpath, path);
    strcat(outputpath, tag);
    strcat(outputpath, data_size);
    strcat(outputpath, file);
    return outputpath;
}
uint32_t *gen_test_case(size_t size, int ordered)
{
    uint32_t n = 100000;
    uint32_t *data = gen_data(size, 1, n);
    switch (ordered) {
    case 0:
        data = proc_data_reverse(data, size);
        break;
    case 1:
        data = proc_sorted_data(data, size, 1, 1);
        break;
    case 2:
        data = proc_sorted_data(data, size, 1, 0);  // random
    default:
        break;
    }

    return data;
}
