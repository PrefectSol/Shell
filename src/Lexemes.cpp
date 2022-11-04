#include "Lexemes.h"

List::nodeList *Lexemes::GetLexemesList() {
    static const size_t base_len = 256;
    size_t mult = 1;
    char buf[base_len + 1];

	strBegin = (char*)malloc(sizeof(char));
	strBegin[0] = 0;

	lexemesList = new nodeList;
	BuildEmptyList(lexemesList);

	while(fgets(buf, sizeof(buf) / sizeof(buf[0]), stdin) != NULL) {
        strBegin = (char*)realloc(strBegin, (mult * base_len + 1) * sizeof(char));
        strcat(strBegin, buf);
        mult++;

		if (strpbrk(buf, "\n")) {
		    char *str = strBegin;
		    str = NextLexeme(str);

			while (lexeme != NULL) {
                AddWord(lexemesList, lexeme);
                str = NextLexeme(str);
            }

			free(strBegin);
			strBegin = str = NULL;

			return lexemesList;
		}
	}
    
	return lexemesList;
}

char* Lexemes::NextLexeme(char *str) {
    char *lexEnd, *ptr;
	while (isspace(str[0])) {
        str++;
    }

	if (str[0] == 0) {    
        lexeme = NULL;

        return NULL;
    }

    switch (str[0]) {
        case ';':            
        case '(':
        case ')':
        case '<':
            lexEnd = str + 1;
            break;

        case '|':            
        case '&':
        case '>':
            if (str[1] == str[0]) {
                lexEnd = str + 2;
                if (str[2] == str[0]) {
                    LexemError();
                }
            }
            else {
                lexEnd = str + 1;
            }

            break;

        default:
            lexEnd = strpbrk(str, " \t\n;()<>|&");
            break;
    }

    lexeme = (char*)malloc((lexEnd - str + 1) * sizeof(char));
    for (ptr = lexeme; str < lexEnd; ++str, ++ptr) {
        if (isCorrect(str[0])) {
            ptr[0] = str[0];
        }
        else {
            LexemError();
        }
    }

    ptr[0] = 0;      

    return str;
}

bool Lexemes::isCorrect(char symb) {
    return ((tolower(symb) >= 'a' && tolower(symb) <= 'z') ||
            (tolower(symb) >= 'а' && tolower(symb) <= 'я') ||
            (symb >= '0' && symb <= '9') ||
            (symb == '_') ||
            (symb == '/') ||
            (symb == '.') ||
            (symb == ')') ||
            (symb == '(') ||
            (symb == '<') ||
            (symb == '>') ||
            (symb == '&') ||
            (symb == ';') ||
            (symb == '|') ||
            (symb == ' ') ||
            (symb == '-') || 
            (symb == '+') || 
            (symb == '#') || 
            (symb == '~'));
}

void Lexemes::LexemError() {
    if (strBegin != NULL) {
        free(strBegin);
    }

    strBegin = NULL;
    if (lexeme != NULL) {
        free(lexeme);
    }

    lexeme = NULL;
    if (lexemesList != NULL) {
        DeleteList(&lexemesList);
    }

    Common::Error("Bad character");
}