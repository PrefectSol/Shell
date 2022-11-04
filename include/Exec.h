#pragma once

#include "Headings.h"
#include "ExecTree.h"
#include "Builtins.h"

class Exec : protected ExecTree {
private:
    typedef struct zombie {
        pid_t pid;
        zombie *next;
    };

    static inline zombie *zHead;
    static inline size_t zCount;

    static int ExecutePipe(ExecTree::cmdInfo *execInfo);

    static void AddZombie(pid_t pid);

    static void MakeProcess(ExecTree::cmdInfo *execTree, bool background, pid_t pgid);

    static bool IsOnlySubExecTree(ExecTree::cmdInfo *execTree);

public:
    static zombie* GetZmbHead();

    static size_t GetZmbCount();

    static void SetZmbHead(zombie* head);

    static void SetZmbCount(size_t count);

    static void Execute(ExecTree::cmdInfo *execTree);

    static void ExecuteTree(ExecTree::cmdInfo *execTree);

    static void KillZombie();
};