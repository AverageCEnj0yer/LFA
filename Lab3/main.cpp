#include <iostream>
#include "lexer.h"
int main()
{
    Lexer lex{"test.lex"};
    lex.fileContent();
    Token t1{};
    while (!lex.isAtEnd())
    {
        t1 = lex.getNextToken();
        printToken(t1);
        
    }
    return 0;
}
