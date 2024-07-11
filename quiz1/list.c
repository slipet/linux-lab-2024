#include "list.h"

void __list_add(node_t **list, node_t *node_t)
{
    node_t->next = *list;
    *list = node_t;
}

node_t *__list_tail(node_t **left)
{
    while ((*left) && (*left)->next)
        left = &((*left)->next);
    return *left;
}
void __printList(node_t *left)
{
    while (left) {
        printf("%ld -> ", left->value);
        left = left->next;
    }
    printf("\n");
    return;
}
int __list_length(node_t **left)
{
    int n = 0;
    while (*left) {
        ++n;
        left = &((*left)->next);
    }
    return n;
}

node_t *__list_construct(node_t *list, int n)
{
    node_t *node = malloc(sizeof(node_t));
    node->next = list;
    node->value = n;
    return node;
}

void __list_free(node_t **list)
{
    node_t *node = (*list)->next;
    while (*list) {
        free(*list);
        *list = node;
        if (node)
            node = node->next;
    }
}


void __quick_sort(node_t **list)
{
    int n = __list_length(list);
    int value;
    int i = 0;
    int max_level = 2 * n;
    node_t *begin[max_level], *end[max_level];
    node_t *result = NULL, *left = NULL, *right = NULL;

    begin[0] = *list;
    end[0] = __list_tail(list);

    while (i >= 0) {
        node_t *L = begin[i], *R = end[i];

        if (L != R) {
            node_t *pivot = L;
            value = pivot->value;
            node_t *p = pivot->next;
            pivot->next = NULL;

            while (p) {
                node_t *n = p;
                p = p->next;
                __list_add(n->value > value ? &right : &left, n);
            }

            begin[i] = left;
            end[i] = __list_tail(&left);

            begin[i + 1] = pivot;
            end[i + 1] = pivot;

            begin[i + 2] = right;
            end[i + 2] = __list_tail(&right);

            left = right = NULL;
            i += 2;
        } else {
            if (L)
                __list_add(&result, L);

            i--;
        }
    }
    *list = result;
}