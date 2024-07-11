#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "linux_list.h"
#include "list.h"

/* Verify if list is order */
static bool __list_is_ordered(node_t *list)
{
    bool first = true;
    int value;
    while (list) {
        if (first) {
            value = list->value;
            first = false;
        } else {
            if (list->value < value)
                return false;
            value = list->value;
        }
        list = list->next;
    }
    return true;
}

static bool linx_list_is_ordered(struct list_head *head)
{
    bool first = true;
    int value;
    struct list_head *node = NULL;
    list_for_each (node, head) {
        if (first) {
            value = list_entry(node, element_t, list)->data.value;
            first = false;
        } else {
            if (list_entry(node, element_t, list)->data.value < value)
                return false;
            value = list_entry(node, element_t, list)->data.value;
        }
    }
    return true;
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

    size_t count = 1000;
    size_t size = count;
    int *test_arr = malloc(sizeof(int) * count);

    for (int i = 0; i < count; ++i)
        test_arr[i] = i;
    shuffle(test_arr, count);

    while (count--) {
        list = __list_construct(list, test_arr[count]);
        if (!linux_list_construct(&linux_list, test_arr[count],
                                  test_arr[count]))
            printf("linux_list_construct error: %d\n", test_arr[count]);
    }

    __quick_sort(&list);
    quick_sort_linux_list(&linux_list, size);

    assert(__list_is_ordered(list));
    assert(linx_list_is_ordered(&linux_list));

    __list_free(&list);
    linux_list_free(&linux_list);

    free(test_arr);
    return;

}
