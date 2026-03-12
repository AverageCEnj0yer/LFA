#pragma once
#include <string>

enum TokenType
{
    UNKNOWN = 0,

    //keywords
    SIN = -1,
    COS = -2,
    LEN = -3,
    PRINT = -4,
    TRUE = -5,
    FALSE = -6,
    AND = -7,
    OR = -8,
    NOT = -9,
    
    //types
    INT = -10,
    FLOAT = -11,
    STRING = -12,
    BOOLEAN = -13, 

    IDENTIFIER = -14,

    //operators
    PLUS = -15,
    MINUS = -16,
    MULTIPLY = -17,
    DIVIDE = -18,
    EXPONENT = -19,
    ASSIGN = -20,
    LESS = -21,
    GREATER = -22,
    LESSEQ = -23,
    GREATEREQ = -24,
    EQ = -25,
    NOTEQ = -26,

    //delimiters
    LPAREN = -27,
    RPAREN = -28,
    COMMA = -29,
    SEMICOLON = -30,

    EOFILE = -31,

};

struct Token
{
    TokenType type{};
    std::string lexeme{};
};

