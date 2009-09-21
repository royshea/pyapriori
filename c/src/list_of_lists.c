unsigned char remove_list(struct node **list_of_lists, struct node *list);
unsigned char same_list(struct node *l_1, struct node *l_2);


void free_list_of_lists(struct node **n);

struct node *copy_list_of_lists(struct node *n);

int compare_lists(void* first, void* second);

int compare_lists(void* first, void* second)
{

    struct node* list_1 = (struct node*)(first);
    struct node* list_2 = (struct node*)(second);

    while(list_1 != NULL && list_2 != NULL)
    {
        if(*((int *)(list_1->data)) < *((int *)(list_2->data)))
            return -1;
        else if(*((int *)(list_1->data)) > *((int *)(list_2->data)))
            return 1;
        else
        {
            list_1 = list_1->next;
            list_2 = list_2->next;
        }


    }

    if(list_1 == NULL && list_2 == NULL)
        return 0;
    else if(list_1 == NULL && list_2 != NULL)
        return -1;
    else
        return 1;

}

struct node *copy_list_of_lists(struct node *n)
{
    struct node *iter = n;
    struct node *copy = NULL;

    while(iter != NULL)
    {
        add(&copy,copy_list((struct node *)iter->data, sizeof(uint32_t)),iter->count);
        iter = iter->next;
    }
    return copy;
}

void free_list_of_lists(struct node **n)
{
    struct node *current;
    struct node *next;
    for(current = *n; current != NULL; current = next)
    {
        next = current->next;
        free_list(((struct node**)(&current->data)),&free);
        free(current);
    }
    *n = NULL;
}


unsigned char remove_list(struct node **list_of_lists, struct node *list)
    /* Removes list from list_of_lists */
{
    if(list_of_lists == NULL)
        return FALSE;
    struct node *iter = *list_of_lists;
    struct node *temp;
    unsigned char inside = FALSE;

    if(iter == NULL || list == NULL)
        return FALSE;

    if(same_list((struct node *)iter->data,list))
    {
        *list_of_lists = iter->next;
        free_list((struct node **)&iter->data,&free);
        free(iter);
        return TRUE;
    }

    while(iter->next != NULL)
    {
        if(same_list((struct node *)iter->next->data,list))
        {
            inside = TRUE;
            break;
        }
        iter = iter->next;
    }

    if(inside == TRUE)
    {
        temp = iter->next->next;
        free_list((struct node **)&iter->next->data,&free);
        free(iter->next);

        iter->next = temp;
        return TRUE;
    }
    else
        return FALSE;


}


/* TODO: This should use a generic equality function rather than
 * assuming the structure of data. */
unsigned char same_list(struct node *l_1, struct node *l_2)
{
    struct node *iter_1 = l_1;
    struct node *iter_2 = l_2;
    while(iter_1 != NULL && iter_2 != NULL)
    {
        if(*((uint32_t *)iter_1->data) != *((uint32_t *)iter_2->data))
            return FALSE;
        iter_1 = iter_1->next;
        iter_2 = iter_2->next;
    }
    return TRUE;
}



