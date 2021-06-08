/* This file contains the code for a generic linked list*/

#include "include/stdheaders.h"
#include "include/list.h"
#include "include/data_manager.h"
#include "include/error_handler.h"

/* This function add a node to the begining of a list 
@head - the head node of the list
@data - the data of the new node
@data_size - the size of the new data assigned*/
void add_first(node **head, void *new_data, size_t data_size)
{
    int i;
    /* Allocate memory for node */
    node *new_node;
    if((new_node = (node *)malloc(sizeof(node))) == NULL)
    {
        call_error(UNABLE_TO_ALLOCATE_MEMORY,NO_LINE);
    }
    if((new_node->data = malloc(data_size)) == NULL)
    {
        call_error(UNABLE_TO_ALLOCATE_MEMORY,NO_LINE);
    }
    new_node->next = (*head);

    /* Copy contents of new_data to newly allocated memory. */
    /* Assumption: char takes 1 byte.*/
    for (i = 0; i < data_size; i++)
    {
        *((char *)(new_node->data) + i) = *((char *)(new_data) + i);
    }
    /* Change head pointer as new node is added at the beginning */
    (*head) = new_node;
}

/* This function frees all alocated memory for a given list */
void free_list(node *head)
{
    node *tmp;
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        free(tmp->data);
        free(tmp);
    }
    /* reset_values(head); */
}


/* this function get the data from a given index
@head - the head of linked list
@index - the index of the element starting from 0 */
void *get_data(node *head, int index)
{
    while (index--)
    {
        head = head->next;
    }
    return head->data;
}

void reset_values(node *head)
{
    node *tmp;
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        tmp->data = NULL;
    }
}


/* this function gets the index of some data in the list
@head - the head of the list you want to search in
@target - the target data you want to match with. 
@match - a bool function that matchs @target with the data of @head
@return - if @match finds a match the function returns the index of itme in the list
if not match was found -1 is returned.
*/
int index_of(node * head,void * target,bool (*match)(void * target,void * data))
{
    int i = 0;
    while(head != NULL)
    {
        if(match(target,head->data))
        {
            return i;
        }
        i++;
        head = head->next;
    }
    return -1;
}

/* works exactly like @index_of but returns the pointer in the list instead. */
node * pointer_of(node * head,void *target,bool (*match)(void * target,void * data))
{
    while(head != NULL)
    {
        if(match(target,head->data))
        {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

/* this is a helper function for @index_of function */
bool match_string(void *target,void *data)
{
	return strcmp((char *)target,(char *)data) == 0;
}

bool match_int(void *target,void *data)
{
	return *(int *)target == *(int *)data;
}