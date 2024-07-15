#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "linux_list.h"
#include "list.h"
#include "test.h"

#define UNORDERED 0
#define ORDERED 1
#define STABLE 2


/* shuffle array, only work if n < RAND_MAX */
void shuffle(long *array, size_t n)
{
    if (n <= 0)
        return;

    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        int t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

void init_linux_list_config(expConfig *config)
{
    config->algorithm = &quick_sort_linux_list;
    config->validation = &linx_list_is_ordered;
    config->printList = &print_linux_list;
    config->list_construct = &linux_list_construct;
    config->list_free = &linux_list_free;
}

void init_list_config(expConfig *config)
{
    config->algorithm = &__quick_sort;
    config->validation = &__list_is_ordered;
    config->printList = &__print_list;
    config->list_construct = &__list_construct;
    config->list_free = &__list_free;
}

int main(int argc, char **argv)
{
    int algo_num = 2;
    expConfig config[2] = {0};
    init_list_config(&config[0]);
    init_linux_list_config(&config[1]);

    node_t *list = NULL;
    LIST_HEAD(linux_list);

    size_t count = 10;
    size_t size = count;
    int *test_arr = malloc(sizeof(int) * count);

    for (int i = 0; i < count; ++i)
        test_arr[i] = i;

    shuffle(test_arr, count);

    while (count--) {
        list = config[0].list_construct(list, &test_arr[count]);
        if (!config[1].list_construct(&linux_list, &test_arr[count]))
            printf("linux_list_construct error: %d\n", test_arr[count]);
    }

    config[0].algorithm(&list);
    config[1].algorithm(&linux_list);

    printf("list_is_ordered: %d\n", config[0].validation(list));
    printf("list_is_ordered: %d\n", config[1].validation(&linux_list));

    config[0].list_free(&list);
    config[1].list_free(&linux_list);

    free(test_arr);

    return;
}
