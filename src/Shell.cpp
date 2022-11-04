#include "Shell.h"

Shell::Shell() : cwdSIZE(256) {
    setpgid(0, 0);

    signal(SIGINT, SIG_IGN);
    signal(SIGCHLD, Wait);

    buffer.lexems = NULL;
    buffer.execTree = NULL;
 
    Exec::SetZmbHead(NULL);
    Exec::SetZmbCount(0);

    Builtins builtins();

    while(true) {
        setjmp(*Common::GetEnv());

        List::DeleteList(&buffer.lexems);
        ExecTree::FreeCmdInfo(&buffer.execTree);

        char cwd[cwdSIZE];
        printf("[%s %s] > ", getlogin(), getcwd(cwd, sizeof(cwd)));

        buffer.lexems = Lexemes::GetLexemesList();
        buffer.execTree = ExecTree::BuildExecTree(buffer.lexems);

        Execute(buffer.execTree);
        KillZombie();
    }
}

void Shell::Wait(int) {
    wait(NULL);
}