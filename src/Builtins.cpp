#include "Builtins.h"

Builtins::Builtins() {
    builtins.head = NULL;
    builtins.size = 0; 

    AddBuiltin(cat, "cat");
    AddBuiltin(cmp, "cmp");
    AddBuiltin(mv, "mv");
}

Builtins::~Builtins() {
    while(builtins.head->next != NULL) {
        builtinNode *temp = builtins.head->next;

        delete builtins.head;
    }
}

Builtins::builtinNode::builtinNode(void (*func)(int, char**), const char* name, builtinNode *next) {
    builtinNode::name = name;
    builtinNode::func = func;
    builtinNode::next = next;
}

int Builtins::SameFile(int fd1, int fd2) {
    struct stat stat1, stat2;

    if ((fstat(fd1, &stat1) < 0) || (fstat(fd2, &stat2) < 0)) {
        return -1;
    }

    return (stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino);
}

void Builtins::AddBuiltin(void (*func)(int, char**), const char *name) {
    if (builtins.head == NULL) {
        builtins.head = builtins.tail = new builtinNode(func, name, NULL);
    }
    else {
        builtins.tail->next = new builtinNode(func, name, NULL);
        builtins.tail = builtins.tail->next;
    }

    builtins.size++;
}

Builtins::builtinList Builtins::GetBuiltins() {
    return builtins;
}