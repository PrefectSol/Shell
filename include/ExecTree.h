#pragma once

#include "Headings.h"
#include "List.h"
#include "Common.h"

class ExecTree {
public:
    typedef struct cmdInfo {
        int argc;
        char **argv;
        char *infile, *outfile;
        int append, background, status;
        
        cmdInfo *psubcmd, *pipe, *next;
    };

    static void FreeCmdInfo(cmdInfo **cm);

    static cmdInfo *BuildExecTree(List::nodeList* lexemes);

protected:
    static cmdInfo *BuildSubExecTree(cmdInfo **execTree);

private:
    typedef struct cmdInfoNodeList {
        cmdInfoNodeList *next, *prev;
        cmdInfo *cmd;
    };

    typedef struct CmdInfoList {
        cmdInfoNodeList *head;
        cmdInfoNodeList *tail;
    };

    static inline CmdInfoList *cmdList;

    static bool IsOrdinarLexeme(List::node *lexeme);

    static void SyntaxError();

    static void FreeCmdInfoList();

    static void PopCmdInfo();

    static void PushCmdInfo(cmdInfo *ptr);

    static void EmptyCmdInfo(cmdInfo *cm);

    static void AddCmdInfo(cmdInfo*& execTree, cmdInfo cmdInfo);

    static void AddSubCmdInfo(cmdInfo*& execTree, cmdInfo* cmdInfo);

    static cmdInfo *SetCommands(List::node** curLexeme);

    static cmdInfo *_Pipe(List::node **curLexeme);

    static cmdInfo *Command(List::node **curLexeme);

    static cmdInfo *SimpleCommand(List::node **curLexeme);
};