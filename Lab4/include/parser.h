#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "token.h"
#include "nodes.h"
#include "token.h"

class Parser
{
    private:
    Lexer& m_lexer;
    Token m_currentToken{};
    void consume(TokenType expectedType);

    public:
    Parser(Lexer& lexer) : m_lexer{lexer}
    {
        m_currentToken = m_lexer.getNextToken();
    }
    Node* parseExpression(); // OR
    Node* parseSequence();   // CONCAT
    Node* parseFactor();     // ?, *, +, ^nr
    Node* parseBase();       // literal

};

#endif
