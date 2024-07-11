#include "linux_list.h"

bool linux_list_construct(struct list_head *head, int value, int ordered)
{
    if (head == NULL)
        return false;

    element_t *new_element = malloc(sizeof(element_t));
    INIT_LIST_HEAD(&new_element->list);
    new_element->data.value = value;
    new_element->data.ordered = ordered;
    list_add(&new_element->list, head);

    return true;
}
void print_linux_list(struct list_head *head)
{
    struct list_head *node;
    list_for_each (node, head) {
        element_t *element = list_entry(node, element_t, list);
        printf("%d -> ", element->data.value);
    }
    printf("\n");
}

void print_linux_list_head2end(struct list_head *head, struct list_head *end)
{
    struct list_head *node;
    for (node = (head); node != (end); node = node->next) {
        element_t *element = list_entry(node, element_t, list);
        printf("%d -> ", element->data.value);
    }
    printf("\n");
}

static inline void linux_list_release_element(element_t *e)
{
    free(e);
}
void linux_list_free(struct list_head *l)
{
    if (!l)
        return;
    struct list_head *iter, *safe;
    list_for_each_safe (iter, safe, l) {
        list_del(iter);
        linux_list_release_element(list_entry(iter, element_t, list));
    }
}

struct list_head *linux_list_new()
{
    struct list_head *linux_list = malloc(sizeof(struct list_head));
    if (!linux_list)
        return NULL;
    INIT_LIST_HEAD(linux_list);
    return linux_list;
}

void quick_sort_linux_list(struct list_head **head, size_t size)
{
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return;

    int value;
    int i = 0;
    const int max_level = 2 * size;
    struct list_head heads[max_level];
    LIST_HEAD(result);
    for (int i = 0; i < max_level; i++)
        INIT_LIST_HEAD(&heads[i]);
    list_splice_init(head, &heads[0]);

    int k = 0;
    while (i >= 0) {
        struct list_head *L = &heads[i], *R = &heads[i + 2];
        if (!list_empty(L) && !list_is_singular(L)) {
            struct list_head *pivot = L->next;
            value = list_entry(pivot, element_t, list)->data.value;
            list_move_tail(pivot, &heads[i + 1]);

            struct list_head *iter, *safe;

            list_for_each_safe (iter, safe, L) {
                if (list_entry(iter, element_t, list)->data.value <= value) {
                    list_move_tail(iter, R);
                }
            }

            i += 2;
        } else {
            if (!list_empty(L))
                list_splice_tail_init(L, &result);
            i--;
        }
    }
    list_splice_init(&result, head);
}