#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
        uint32_t t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

void init_linux_list_config(expConfig *config)
{
    config->algoName = &"linux_list_sort_";
    config->algorithm = &quick_sort_linux_list;
    config->validation = &linx_list_is_ordered;
    config->printList = &print_linux_list;
    config->list_construct = &linux_list_construct;
    config->list_free = &linux_list_free;
}

void init_list_config(expConfig *config)
{
    config->algoName = &"original_sort_";
    config->algorithm = &__quick_sort;
    config->validation = &__list_is_ordered;
    config->printList = &__print_list;
    config->list_construct = &__list_construct;
    config->list_free = &__list_free;
}

int main(int argc, char **argv)
{
    size_t maxBits = 18;
    const size_t algo_num = 2;
    Path_t *path = INIT_PATH_T("./test_cases/", "", "", "");
    expConfig config[2] = {0};
    init_list_config(&config[0]);
    init_linux_list_config(&config[1]);

    // fixed_amount_test(config, path, maxBits, algo_num);
    random_amount_test(config, path, 2500, algo_num, 2);



    return 0;

}
