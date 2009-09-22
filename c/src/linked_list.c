#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linked_list.h"


int add(struct node **head, void* d)
{
    if (*head == NULL)
    {
        if ((*head = malloc(sizeof(struct node))) == NULL)
        {
            printf("Memory allocation error");
            return 0;
        }
        (*head)->next = NULL;
        (*head)->data = d;
    }
    else
    {
        struct node *tmp = *head;
        while (tmp->next != NULL)
        {
            tmp = tmp->next;
        }

        if ((tmp->next = malloc(sizeof(struct node))) == NULL)
        {
            printf("Memory allocation error");
            return 0;
        }
        tmp->next->next = NULL;
        tmp->next->data = d;
    }
    return 1;

}


void free_list(struct node **n, void (*free_funct)(void *))
{
    struct node *current;
    struct node *next;
    for (current = *n; current != NULL; current = next)
    {
        next = current->next;
        (*free_funct)(current->data);
        free(current);
    }
    *n = NULL;
}


int get_len_list(struct node *n)
{
    int len = 0;
    while (n != NULL)
    {
        len += 1;
        n = n->next;
    }
    return len;
}


struct node *get_node(struct node *n, int index)
{
    int count = 0;
    while (n != NULL)
    {
        if (count == index)
        {
            return n;
        }
        count++;
        n = n->next;
    }

    return NULL;
}

/* TODO: This assumes that data is an integer */
struct node *copy_list(struct node *n, int data_size)
{
    void *tmp = NULL;
    struct node *iter = n;
    struct node *copy = NULL;
    while (iter != NULL)
    {
        if ((tmp = malloc(data_size)) == NULL)
        {
            printf("Memory allocation error");
            return 0;
        }
        memcpy(tmp, iter->data, data_size);
        add(&copy,(void *)tmp);
        iter = iter->next;
    }
    return copy;
}

/* merge the lists.. */
struct node *merge(struct node *head_one, struct node *head_two,
        int (*cmp)(void *, void *))
{
    struct node *head_three;


    /* TODO: Convert recursion into a loop. */
    if (head_one == NULL)
        return head_two;

    if (head_two == NULL)
        return head_one;

    if ((*cmp)(head_one->data, head_two->data) < 0)
    {
        head_three = head_one;
        head_three->next = merge(head_one->next, head_two, cmp);
    }
    else
    {
        head_three = head_two;
        head_three->next = merge(head_one, head_two->next, cmp);
    }

    return head_three;
}


struct node *mergesort(struct node *head, int (*cmp)(void *, void *))
{
    struct node *head_one;
    struct node *head_two;

    if ((head == NULL) || (head->next == NULL))
        return head;

    head_one = head;
    head_two = head->next;
    while ((head_two != NULL) && (head_two->next != NULL))
    {
        head = head->next;
        head_two = head->next->next;
    }
    head_two = head->next;
    head->next = NULL;

    return merge(mergesort(head_one, cmp), mergesort(head_two, cmp), cmp);
}


/* TODO / BUG: This assumes that data is sorted. */
int is_subset(struct node *l_1, struct node *l_2, int (*cmp)(void *, void *))
    /* Returns if l_2 is a subset of l_1 */
{
    unsigned char inside;
    struct node *iter_1;
    struct node *iter_2 = l_2;

    if (l_1 == NULL || l_2 == NULL)
        return FALSE;

    if (get_len_list(l_1) < get_len_list(l_2))
        return FALSE;

    while (iter_2 != NULL)
    {
        iter_1 = l_1;
        inside = FALSE;
        while (iter_1 != NULL)
        {
            if ((*cmp)(iter_1->data, iter_2->data) == 0)
                inside = TRUE;
            iter_1 = iter_1->next;
        }
        if (inside == FALSE)
            return FALSE;
        iter_2 = iter_2->next;
    }

    return TRUE;
}
