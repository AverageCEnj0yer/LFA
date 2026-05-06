#pragma once
#include <string>
#include <iostream>

enum class TokenType
{
    UNKNOWN = 0,

    //keywords
    TRUE = -1,
    FALSE = -2,
    AND = -3,
    OR = -4,
    NOT = -5,
    RETURN = -6,
    
    //types
    INT = -7,
    FLOAT = -8,
    STRING = -9,
    BOOLEAN = -10, 

    IDENTIFIER = -11,

    //operators
    PLUS = -12,
    MINUS = -13,
    MULTIPLY = -14,
    DIVIDE = -15,
    EXPONENT = -16,
    ASSIGN = -17,
    LESS = -18,
    GREATER = -19,
    LESSEQ = -20,
    GREATEREQ = -21,
    EQ = -22,
    NOTEQ = -23,

    //delimiters
    LPAREN = -24,
    RPAREN = -25,
    COMMA = -26,
    SEMICOLON = -27,
    RCBRACKET = -28,
    LCBRACKET = -29,

    EOFILE = -30,

    //Variable Types
    INT_TYPE = -31,
    FLOAT_TYPE = -32,
    STRING_TYPE = -33,
    BOOL_TYPE = -34,
    VOID_TYPE = -35,
};

struct Token
{
    TokenType type{};
    std::string lexeme{};
};


inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::UNKNOWN: return "UNKNOWN";
        case TokenType::TRUE: return "TRUE";
        case TokenType::FALSE: return "FALSE";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";
        case TokenType::RETURN: return "RETURN";
        case TokenType::INT: return "INT";
        case TokenType::FLOAT: return "FLOAT";
        case TokenType::STRING: return "STRING";
        case TokenType::BOOLEAN: return "BOOLEAN";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::EXPONENT: return "EXPONENT";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::LESS: return "LESS";
        case TokenType::GREATER: return "GREATER";
        case TokenType::LESSEQ: return "LESSEQ";
        case TokenType::GREATEREQ: return "GREATEREQ";
        case TokenType::EQ: return "EQ";
        case TokenType::NOTEQ: return "NOTEQ";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::COMMA: return "COMMA";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::RCBRACKET: return "RIGHT_CURLY_BRACKET";
        case TokenType::LCBRACKET: return "LEFT_CURLY_BRACKET";
        case TokenType::EOFILE: return "EOFILE";
        case TokenType::INT_TYPE: return "INT_TYPE";
        case TokenType::FLOAT_TYPE: return "FLOAT_TYPE";
        case TokenType::STRING_TYPE: return "STRING_TYPE";
        case TokenType::BOOL_TYPE: return "BOOL_TYPE";
        case TokenType::VOID_TYPE: return "VOID_TYPE";
        default: return "UNKNOWN";
    }
}

inline void printToken(const Token& token) {
    std::string typeStr = tokenTypeToString(token.type);
    if (token.type == TokenType::UNKNOWN ||
        token.type == TokenType::INT ||
        token.type == TokenType::FLOAT ||
        token.type == TokenType::STRING ||
        token.type == TokenType::BOOLEAN ||
        token.type == TokenType::IDENTIFIER) {
        std::cout << typeStr << "(" << token.lexeme << ")" << std::endl;
    } else {
        std::cout << typeStr << std::endl;
    }
}

