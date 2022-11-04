#pragma once

#include "Headings.h"

class List {
public:
    typedef struct node {
        char *element;

        node *next;
    };

    typedef struct nodeList {
        node *head;
        node *tail;

        size_t size;
    };

    static void DeleteList(nodeList** list);

protected:
    static void BuildEmptyList(nodeList* list);

    static void AddWord(nodeList* list, char *word);
};