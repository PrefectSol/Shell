#include "List.h"

void List::DeleteList(nodeList **list) {
    if (*list == NULL) {
		return;
	}

	node* cur = (*list)->head, *next;
	while (cur != NULL) {
		next = cur->next;

		free(cur->element);
		free(cur);
        
		cur = next;
	}
    
	free(*list);
	*list = NULL;
}

void List::AddWord(nodeList* list, char *word) {
	if (list->size == 0) {
		list->head = list->tail = (node*)malloc(sizeof(nodeList));
	}
	else {
		list->tail->next = (node*)malloc(sizeof(nodeList));
		list->tail = list->tail->next;
	}

	list->tail->element = word;
	list->tail->next = NULL;
	list->size++;
}

void List::BuildEmptyList(nodeList* list) {
	list->head = list->tail = NULL;
    list->size = 0;
}