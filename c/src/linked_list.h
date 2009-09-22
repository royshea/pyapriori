#ifndef LINKED_LIST_
#define LINKED_LIST_

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


struct node
{
    void* data;
    struct node* next;
};


int add(struct node **n, void* d);

void free_list(struct node **n,void (*free_funct)(void *));

int get_len_list(struct node *n);

struct node *get_node(struct node *n, int index);

struct node *copy_list(struct node *n, int data_size);

struct node *mergesort(struct node *head,int (*cmp)(void *,void *));

struct node *merge(struct node *head_one, struct node *head_two,int (*cmp)(void *,void *));

int is_subset(struct node *l_1, struct node *l_2, int (*cmp)(void *,void *));


#endif /*LINKED_LIST_*/
