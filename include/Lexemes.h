#pragma once

#include "Headings.h"
#include "List.h"
#include "Common.h"

class Lexemes : protected List {
public:
    static nodeList *GetLexemesList();

private:    
    static inline char *strBegin, *lexeme;
    static inline nodeList *lexemesList;

    static char* NextLexeme(char *str);
    
    static bool isCorrect(char symb);

    static void LexemError();
};