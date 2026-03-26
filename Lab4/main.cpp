#include <iostream>

#include "lexer.h"
#include "token.h"
#include <vector>
int main()
{
    //You got this soldier
    Lexer l{"(a|/+)(c|d)E+G?"};
    std::vector<Token> tokens{};

    while (!l.isAtEnd())
    {
        tokens.push_back(l.getNextToken());
    }

    for(Token token : tokens)
    {
        printToken(token);
    }
    
    return 0;
}
