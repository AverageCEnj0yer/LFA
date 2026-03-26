#ifndef TOKEN_H
#define TOKEN_H

// ========================================
// =========THE TOKENS OF REGEX============
// ========================================

#include <string>
#include <iostream>

enum class TokenType
{
    UNKNOWN = 0,

    //keywords
    LPAREN = -1,    // (
    RPAREN = -2,    // )
    PIPE = -3,      // |
    STAR = -4,      // *
    PLUS = -5,      // +
    QUESTION = -6,  // ?
    POWER = -7,     // ^nr
    LITERAL = -8,   // anything
    END = -9,

};

struct Token
{
    TokenType type{};
    std::string lexeme{};
};


inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::UNKNOWN: return "UNKNOWN";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::PIPE: return "PIPE";
        case TokenType::STAR: return "STAR";
        case TokenType::PLUS: return "PLUS";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::POWER: return "POWER";
        case TokenType::LITERAL: return "LITERAL";
        default: return "UNKNOWN";
    }
}

inline void printToken(const Token& token) {
    std::string typeStr = tokenTypeToString(token.type);
    if (token.type == TokenType::UNKNOWN ||
        token.type == TokenType::LITERAL) {
        std::cout << typeStr << "(" << token.lexeme << ")" << std::endl;
    } else {
        std::cout << typeStr << std::endl;
    }
}



#endif
