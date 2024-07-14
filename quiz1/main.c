#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "linux_list.h"
#include "list.h"

#define UNORDERED 0
#define ORDERED 1
#define STABLE 2


Introduce checking if list is stable after sorting

Besides verifing ordering, "__list_is_ordered()" and "linx_list_is_ordered()"         
two functions also can verfy                           


/* Verify if list is order */
// 00 -> not ordered
// 01 -> ordered but not stable
// 11 -> ordered and stable
static int __list_is_ordered(node_t *list)
{
    int stable = 1;

    for (node_t *node = list, *safe = node->next; node != NULL;
         node = safe, safe = node->next) {
        if (!safe)
            break;
        int *first = node->value;
        int *second = safe->value;
        if (*first > *second)
            return 0;
        if (*first == *second && first > second)
            stable = 0;
    }
    return stable * 2 + 1;
}


// 00 -> not ordered
// 01 -> ordered but not stable
// 11 -> ordered and stable
static int linx_list_is_ordered(struct list_head *head)
{
    int stable = 1;
    struct list_head *iter = NULL, *safe = NULL;

    list_for_each_safe (iter, safe, head) {
        if (safe == head)
            break;
        int *first = list_entry(iter, element_t, list)->value;
        int *second = list_entry(safe, element_t, list)->value;
        if (*first > *second)
            return 0;

        if (*first == *second && first > second) {
            stable = 0;
        }
    }
    return stable * 2 + 1;
}

/* shuffle array, only work if n < RAND_MAX */
void shuffle(int *array, size_t n)
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

int main(int argc, char **argv)
{
    node_t *list = NULL;
    LIST_HEAD(linux_list);

    size_t count = 10;
    size_t size = count;
    int *test_arr = malloc(sizeof(int) * count);

    for (int i = 0; i < count; ++i)
        test_arr[i] = i;

    shuffle(test_arr, count);

    while (count--) {
        list = __list_construct(list, &test_arr[count]);
        if (!linux_list_construct(&linux_list, &test_arr[count]))
            printf("linux_list_construct error: %d\n", test_arr[count]);
    }

    __quick_sort(&list);
    quick_sort_linux_list(&linux_list, size);

    assert(__list_is_ordered(list) == (ORDERED | STABLE));
    assert(linx_list_is_ordered(&linux_list) == (ORDERED | STABLE));

    __list_free(&list);
    linux_list_free(&linux_list);
    free(test_arr);

    return;
}
