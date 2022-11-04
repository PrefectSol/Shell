#include "ExecTree.h"

void ExecTree::FreeCmdInfo(cmdInfo **cm) {
    if (*cm == NULL) return;

    FreeCmdInfo(&((*cm)->next));
    FreeCmdInfo(&((*cm)->pipe));
    FreeCmdInfo(&((*cm)->psubcmd));

    free((*cm)->infile);
    free((*cm)->outfile);

    while ((*cm)->argc--) {
        free((*cm)->argv[(*cm)->argc]);
    }

    free((*cm)->argv);
    free(*cm);
    
    *cm = NULL;
}

void ExecTree::FreeCmdInfoList() {
    cmdInfoNodeList *it = cmdList->head;
    free(cmdList);

    while (it != NULL) {
        FreeCmdInfo(&(it->cmd));
        cmdInfoNodeList *temp = it;
        it = it->next;
        free(temp);
    }
}

void ExecTree::SyntaxError() {
    FreeCmdInfoList();

    Common::Error("Invalid syntax");
}

void ExecTree::EmptyCmdInfo(cmdInfo *cm) {
    cm->argc = 0;
    cm->argv = NULL;
    cm->infile = NULL;
    cm->outfile = NULL;
}

void ExecTree::PopCmdInfo() {
    if (cmdList->tail == NULL) {
        Common::Error("Command_info_list error: pop from empty stack");
        return;
    }
    
    if (cmdList->tail->prev != NULL) {
        cmdList->tail = cmdList->tail->prev;
        free(cmdList->tail->next);
        cmdList->tail->next = NULL;
    }
    else {
        free(cmdList->head);
        cmdList->head = cmdList->tail = NULL;
    }
}

void ExecTree::PushCmdInfo(cmdInfo *ptr) {
    if (cmdList->head == NULL) {
        cmdList->head = cmdList->tail = (ExecTree::cmdInfoNodeList*)malloc(sizeof(cmdInfoNodeList));
        cmdList->head->prev = NULL;
    }
    else {
        cmdList->tail->next = (ExecTree::cmdInfoNodeList*)malloc(sizeof(cmdInfoNodeList));
        cmdList->tail->next->prev = cmdList->tail;
        cmdList->tail = cmdList->tail->next;
    }

    cmdList->tail->cmd = ptr;
    cmdList->tail->next = NULL;
}

bool ExecTree::IsOrdinarLexeme(List::node *lexeme) {
    return (lexeme != NULL && strstr("| || & && ; ( ) < > >>", lexeme->element) == NULL);
}

ExecTree::cmdInfo *ExecTree::SimpleCommand(List::node **curLexeme) {
    List::node *lexeme = *curLexeme;
	cmdInfo *cm = (ExecTree::cmdInfo*)malloc(sizeof(cmdInfo));

	PushCmdInfo(cm);
	EmptyCmdInfo(cm);

	cm->argc = 1;
	cm->argv = (char**)malloc(sizeof(char*));
	cm->argv[0] = (char*)malloc((strlen(lexeme->element) + 1) * sizeof(char));
	cm->psubcmd = cm->next = cm->pipe = NULL;
    
	strcpy(cm->argv[0], lexeme->element);
	lexeme = lexeme->next;

	while (lexeme != NULL && strstr("| || ; & && ( )", lexeme->element) == NULL) {
	    if (!IsOrdinarLexeme(lexeme) && (!IsOrdinarLexeme(lexeme->next) || lexeme->next == NULL)) {
            SyntaxError();
        }

		if (!strcmp(lexeme->element, ">")) {              
			lexeme = lexeme->next;

            if (cm->outfile != NULL) {
                free(cm->outfile);
            }

            cm->outfile = (char*)malloc((strlen(lexeme->element) + 1) * sizeof(char));
            strcpy(cm->outfile, lexeme->element);
            cm->append = 0;
		}

		else if (!strcmp(lexeme->element, ">>")) {        
			lexeme = lexeme->next;

            if (cm->outfile != NULL) {
                free(cm->outfile);
            }
                
            cm->outfile = (char*)malloc((strlen(lexeme->element) + 1) * sizeof(char));
            strcpy(cm->outfile, lexeme->element);
            cm->append = 1;
		}

		else if (!strcmp(lexeme->element, "<")) {         
			lexeme = lexeme->next;

            if (cm->infile != NULL) {
                free(cm->infile); 
            }

            cm->infile = (char*)malloc((strlen(lexeme->element) + 1) * sizeof(char));
            strcpy(cm->infile, lexeme->element);
		}
		else {                                         
			cm->argc++;
			cm->argv = (char**)realloc(cm->argv, cm->argc * (sizeof(char *)));
			cm->argv[cm->argc - 1] = (char*)malloc((strlen(lexeme->element) + 1) * sizeof(char));

			strcpy(cm->argv[cm->argc - 1], lexeme->element);
		}

		lexeme = lexeme->next;
	}

	cm->argv = (char**)realloc(cm->argv, (cm->argc + 1) * sizeof(char *));
	cm->argv[cm->argc] = NULL;

	*curLexeme = lexeme;
	PopCmdInfo();

	return cm;
}

ExecTree::cmdInfo *ExecTree::Command(List::node **curLexeme) {
    List::node *lexeme = *curLexeme;
	cmdInfo *temp = NULL;

	if (!strcmp(lexeme->element, "(")) {
		lexeme = lexeme->next;
		temp = (ExecTree::cmdInfo*)malloc(sizeof(cmdInfo));

		PushCmdInfo(temp);
		EmptyCmdInfo(temp);

		*curLexeme = lexeme;
		temp->psubcmd = SetCommands(curLexeme);     
		lexeme = *curLexeme;

		if (lexeme == NULL || strcmp(lexeme->element, ")")) {     
            SyntaxError();
        }

		lexeme = lexeme->next;
		while (lexeme != NULL && strstr("| || ; & && ( )", lexeme->element) == NULL) {
			if (!strcmp(lexeme->element, ">")) {              
                lexeme = lexeme->next;

                if (temp->outfile != NULL) {
                    free(temp->outfile);
                }

                temp->outfile = (char*)malloc(sizeof(*(lexeme->element)));
                strcpy(temp->outfile, lexeme->element);
                temp->append = 0;
            }

            else if (!strcmp(lexeme->element, ">>")) {        
                lexeme = lexeme->next;

                if (temp->outfile != NULL) {
                    free(temp->outfile);
                }
                    
                temp->outfile = (char*)malloc(sizeof(*(lexeme->element)));
                strcpy(temp->outfile, lexeme->element);
                temp->append = 1;
            }

            else if (!strcmp(lexeme->element, "<")) {        
                lexeme = lexeme->next;

                if (temp->infile != NULL) {
                    free(temp->infile);
                }
                    
                temp->infile = (char*)malloc(sizeof(*(lexeme->element)));
                strcpy(temp->infile, lexeme->element);
            }

			else {
                SyntaxError();
            }
		}

		*curLexeme = lexeme;

        PopCmdInfo();
	}

	else if (IsOrdinarLexeme(*curLexeme)) {
        temp = SimpleCommand(curLexeme);
    }

	else {
        SyntaxError();
    }
        
	return temp;
}

ExecTree::cmdInfo *ExecTree::_Pipe(List::node **curLexeme) {
    cmdInfo *first;
	cmdInfo *last;

	first = last = Command(curLexeme);

	PushCmdInfo(first);

	while(*curLexeme != NULL && !strcmp((*curLexeme)->element, "|")) {
		*curLexeme = (*curLexeme)->next;

		if (*curLexeme != NULL) {
			last->pipe = Command(curLexeme);
			last = last->pipe;
		}

		else {
            SyntaxError();
        }
	}

	last->pipe = NULL;

    first->background = *curLexeme != NULL && !(strcmp((*curLexeme)->element, "&")) ? 1 : 0;
        
    PopCmdInfo();

	return first;
}

ExecTree::cmdInfo *ExecTree::SetCommands(List::node** curLexeme) {
    cmdInfo *first;
	cmdInfo *last;

	first = last = _Pipe(curLexeme);

	PushCmdInfo(first);

	first->status = 0;
	while (*curLexeme != NULL && strstr("; & || &&", (*curLexeme)->element) != NULL) {
		int status;
		if (!strcmp((*curLexeme)->element, "||")) {
			if ((*curLexeme)->next == NULL) {
                SyntaxError();
            }
                
			else {
                status = 1;
            }
		}

		else if (!strcmp((*curLexeme)->element, "&&")) {
			if ((*curLexeme)->next == NULL) {
                SyntaxError();
            }
                
			else {
                status = 2;
            }
		}

		else {
            status = 0;
        }
        
		*curLexeme = (*curLexeme)->next;
		if (*curLexeme != NULL && strcmp((*curLexeme)->element, ")")) {
			last->next = _Pipe(curLexeme);
			last = last->next;
			last->status = status;
		}
	}

	last->next = NULL;

	PopCmdInfo();

	return first;
}

ExecTree::cmdInfo *ExecTree::BuildExecTree(List::nodeList* lexemes) {
    if (lexemes->head == NULL) {
        return NULL;
    }

    cmdList = (CmdInfoList*)malloc(sizeof(CmdInfoList));
    cmdList->head = cmdList->tail = NULL;

    List::node *cur_lexeme = lexemes->head;
	if (lexemes->size == 0) {
        longjmp(*Common::GetEnv(), 1);
    }
        
	cmdInfo *cmds = SetCommands(&cur_lexeme);

	PushCmdInfo(cmds);

	if (cur_lexeme != NULL) {
        SyntaxError();
    }

    PopCmdInfo();
    FreeCmdInfoList();
    
    return cmds;
}

void ExecTree::AddSubCmdInfo(cmdInfo*& execTree, cmdInfo* cmdInfo) {
    ExecTree::cmdInfo* root = execTree;

    if (execTree == NULL) {
        execTree = cmdInfo;

        return;
    }

    while(execTree->next != NULL) {
        execTree = execTree->next;
    }

    execTree->next = cmdInfo;

    execTree = root;
}

void ExecTree::AddCmdInfo(cmdInfo*& execTree, cmdInfo cmdInfo) {
    ExecTree::cmdInfo* root = execTree;

    if (execTree == NULL) {
        execTree = (ExecTree::cmdInfo*)malloc(sizeof(cmdInfo));

        execTree->argc = cmdInfo.argc;
        execTree->append = cmdInfo.append;
        execTree->argv = cmdInfo.argv;
        execTree->background = cmdInfo.background;
        execTree->infile = cmdInfo.infile;
        execTree->outfile = cmdInfo.outfile;
        execTree->pipe = cmdInfo.pipe;
        execTree->psubcmd = cmdInfo.psubcmd;
        execTree->status = cmdInfo.status;
        execTree->next = NULL;
        
        return;
    }

    while(execTree->next != NULL) {
        execTree = execTree->next;
    }
    
    execTree->next = (ExecTree::cmdInfo*)malloc(sizeof(cmdInfo));

    execTree->next->argc = cmdInfo.argc;
    execTree->next->append = cmdInfo.append;
    execTree->next->argv = cmdInfo.argv;
    execTree->next->background = cmdInfo.background;
    execTree->next->infile = cmdInfo.infile;
    execTree->next->outfile = cmdInfo.outfile;
    execTree->next->pipe = cmdInfo.pipe;
    execTree->next->psubcmd = cmdInfo.psubcmd;
    execTree->next->status = cmdInfo.status;
    execTree->next->next = NULL;

    execTree = root;
}

ExecTree::cmdInfo *ExecTree::BuildSubExecTree(ExecTree::cmdInfo **execTree) {
    ExecTree::cmdInfo* subExecTree = NULL;
    ExecTree::cmdInfo* newExecTree = NULL;

    ExecTree::cmdInfo* ptr = *execTree;

    while(ptr != NULL) {
        if (ptr->psubcmd != NULL) {
            AddSubCmdInfo(subExecTree, ptr->psubcmd);
        }
        else {
            AddCmdInfo(newExecTree, *ptr);
        }

        ptr = ptr->next;
    }

    *execTree = newExecTree;

    return subExecTree;
}