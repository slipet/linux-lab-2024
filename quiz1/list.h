#ifndef __LIST__
#define __LIST__

typedef struct __node {
    struct __node *left, *right;
    struct __node *next;
    long value;
} node_t;

void list_add(node_t **, node_t *);
node_t *list_tail(node_t **);
int list_length(node_t **);
node_t *list_construct(node_t *, int);
void list_free(node_t **);
void printList(node_t *);

#endif