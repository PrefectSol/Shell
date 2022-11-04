#include "Exec.h"

void Exec::SetZmbHead(zombie* head) {
    zHead = head;
}

void Exec::SetZmbCount(size_t count) {
    zCount = count;
}

Exec::zombie* Exec::GetZmbHead() {
    return zHead;
}

size_t Exec::GetZmbCount() {
    return zCount;
}

int Exec::ExecutePipe(ExecTree::cmdInfo *execInfo) {
	ExecTree::cmdInfo *now = execInfo;
	pid_t pid, pgid;

    int fd_temp = -1;
    int fd[2], laststat = 0;
    bool is_fork = false;
    
    if (execInfo->background && getpgrp() != getpid()) {
        setpgid(0, 0);
        is_fork = true;
    }

    pgid = getpgrp();
	while (now != NULL) {
        if (now->argv && !strcmp(now->argv[0], "exit")) {
            exit(0);
        }

        if (now->argv && !strcmp(now->argv[0], "cd")) {
            if (fd_temp != -1) {
                close(fd_temp);
            }

            if (now->pipe != NULL) {
                if (pipe(fd) < 0) {
                    exit(1);
                }

                fd_temp = fd[0];
                close(fd[1]);
            }

            if (execInfo->background) {
                AddZombie(pid);
            }

            return cd(now->argc, now->argv);
        }

		if (now == execInfo) {
            if (now->pipe != NULL) {
                if (pipe(fd) < 0) {
                    exit(1);
                }

                if ((pid = fork()) == 0) {
                    dup2(fd[1], 1);

                    close(fd[0]);
                    close(fd[1]);

                    MakeProcess(now, execInfo->background, pgid);
                }

                else if (execInfo->background) {
                    AddZombie(pid);
                }

                fd_temp = fd[0];

                close(fd[1]);
            }

            else if ((pid = fork()) == 0) {
                MakeProcess(now, execInfo->background, pgid);
            }

            else if (execInfo->background) {
                AddZombie(pid);
            }
        }

        else if (now->pipe == NULL) {
            if ((pid = fork()) == 0) {
                dup2(fd_temp, 0);
                close(fd_temp);

                MakeProcess(now, execInfo->background, pgid);
            }

            else if (execInfo->background) {
                AddZombie(pid);
            }

            close(fd_temp);
        }

        else {
            if (pipe(fd) < 0) {
                exit(1);
            }

            if ((pid = fork()) == 0) {
                dup2(fd_temp, 0);
                dup2(fd[1], 1);

                close(fd[0]);
                close(fd[1]);
                close(fd_temp);

               	MakeProcess(now, execInfo->background, pgid);
            }

            else if (execInfo->background) {
                AddZombie(pid);
            }

            close(fd[1]);
            close(fd_temp);

            fd_temp = fd[0];
        }

		now = now->pipe;
	}

    if (!execInfo->background || is_fork) {
    	waitpid(pid, &laststat, 0);
    	while (wait(NULL) != -1);
    }

    if (is_fork) {
        exit(laststat);
    }

    return laststat;
}

void Exec::AddZombie(pid_t pid) {
    zombie *znode = (zombie*)malloc(sizeof(zombie));

	znode->pid = pid;
	znode->next = zHead;
	zHead = znode;
}

void Exec::KillZombie() {
    int status;
	while (zHead != NULL) {
		waitpid(zHead->pid, &status, 0);

		printf("[%ld] Done\n", (long)zHead->pid);

		zombie *temp = zHead;
		zHead = zHead->next;

		free(temp);
	}

	return;
}

void Exec::MakeProcess(ExecTree::cmdInfo *execTree, bool background, pid_t pgid) {
    background ? signal(SIGINT, SIG_IGN) : signal(SIGINT, SIG_DFL);
    
    if (execTree->infile != NULL) {
        int f = open(execTree->infile, O_RDONLY);
        if (f == -1) {
            Common::Error(strcat(execTree->infile, ": no such file"));
            longjmp(*Common::GetEnv(), 1);
        }
        else {
            dup2(f, 0);
        }

        close(f);
    }

    if (execTree->outfile != NULL) {
        int f;

        if (execTree->append) {
            f = open(execTree->outfile, O_WRONLY|O_APPEND|O_CREAT, 0664);
        }
        else {
            f = open(execTree->outfile, O_WRONLY|O_APPEND|O_CREAT, 0664);
        }

        if (f == -1) {
            Common::Error(strcat(execTree->outfile, ": can't redirect output to file"));
            longjmp(*Common::GetEnv(), 1);
        }
        else {
            dup2(f, 1);
        }
            
        close(f);
    }

    if (background) {
        setpgid(0, pgid);      

        int f = open("/dev/null", O_RDONLY);    
        dup2(f, 0);
    }

    if (execTree->argv != NULL) {
        Builtins::builtinNode *it;
        for (it = Builtins::GetBuiltins().head; it != NULL; it = it->next) {
            if (!strcmp(execTree->argv[0], it->name)) {
                it->func(execTree->argc, execTree->argv);
            }
        }
    }

    if (execTree->argv == NULL) {
        exit(0);
    }
    else {
        execvp(execTree->argv[0], execTree->argv);
    }

    Common::Error(strcat(execTree->argv[0], ": command not found"));

    exit(1);
}

void Exec::ExecuteTree(ExecTree::cmdInfo *execTree) {
    int laststat = 0;
	while (execTree != NULL) {
		if (execTree->status == 1) {
            if (laststat) {
                laststat = ExecutePipe(execTree);
            }
        }
		else if (execTree->status == 2) {
            if (!laststat) {
                laststat = ExecutePipe(execTree);
            }
        }
		else {
            laststat = ExecutePipe(execTree);
        }

		execTree = execTree->next;
	}
}

bool Exec::IsOnlySubExecTree(ExecTree::cmdInfo *execTree) {
    cmdInfo* ptr = execTree;
    while(ptr != NULL) {
        if (ptr->psubcmd != NULL) {
            return false;
        }

        ptr = ptr->next;
    }

    return true;
}

void Exec::Execute(ExecTree::cmdInfo *execTree) {
    ExecTree::cmdInfo* subExecTree = BuildSubExecTree(&execTree);
    
    if (!IsOnlySubExecTree(subExecTree)) {
        Execute(subExecTree);
        ExecuteTree(execTree);
    }
    else {
        ExecuteTree(subExecTree);
        ExecuteTree(execTree);
    }
}