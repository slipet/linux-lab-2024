#ifndef __LIST__
#define __LIST__
#include <stddef.h>
#include <stdint.h>
typedef struct __node {
    struct __node *left, *right;
    struct __node *next;
    long value;
} node_t;

void __list_add(node_t **, node_t *);
node_t *__list_tail(node_t **);
int __list_length(node_t **);
node_t *__list_construct(node_t *, int);
void __list_free(node_t **);
void __printList(node_t *);

#endif