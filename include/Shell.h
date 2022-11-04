#pragma once

#include "Builtins.h"
#include "Common.h"
#include "Exec.h"
#include "ExecTree.h"
#include "List.h"
#include "Lexemes.h"

using namespace std;

class Shell : protected Exec {
private:
    typedef struct interResults {
        List::nodeList *lexems;
        ExecTree::cmdInfo *execTree;
    };

    interResults buffer;

    static void Wait(int);

    const int cwdSIZE;

public:
    Shell();
};