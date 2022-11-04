#pragma once

#include "Headings.h"
#include "Commands.h"

class Builtins {
public:
    typedef struct builtinNode {
        const char* name;
        void (*func)(int, char**);

        builtinNode *next;

        builtinNode(void (*func)(int, char**), const char* name,  builtinNode *next);
    };

private:
    typedef struct builtinList {
        builtinNode *head;
        builtinNode *tail;

        size_t size;
    };

    static inline builtinList builtins;

    static void AddBuiltin(void (*func)(int, char**), const char* name);

    Builtins();

    ~Builtins();

public:
    static builtinList GetBuiltins();

    static int SameFile(int fd1, int fd2);
};