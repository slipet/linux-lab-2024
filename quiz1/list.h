#ifndef __LIST__
#define __LIST__
#include <stddef.h>
#include <stdint.h>

#define TEST_STABLE 0

typedef struct __node {
    struct __node *left, *right;
    struct __node *next;
    uint32_t *value;
} node_t;

void __list_add(node_t **, node_t *);
node_t *__list_tail(node_t **);
size_t __list_length(node_t **);
node_t *__list_construct(node_t *, uint32_t *);
void __list_free(node_t **);
void __print_list(node_t *);
void __quick_sort(node_t **);
int __list_is_ordered(node_t *);
#endif