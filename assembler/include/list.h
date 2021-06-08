#ifndef _LIST_
#define _LIST_

/* A single node in the list */
typedef struct node
{
	void *data;
	struct node *next;
}node;

void *get_data(node * head, int index);
void add_first(node **head, void *data, size_t data_size);
void free_list(node *head);
void reset_values(node *head);
/* matching functions */
node * pointer_of(node * head,void *target,bool (*match)(void * target,void * data));
int index_of(node * head,void * target,bool (*match)(void * target,void * data));
bool match_string(void *target,void *data);
bool match_int(void *target,void *data);

#endif /* _LIST_ */