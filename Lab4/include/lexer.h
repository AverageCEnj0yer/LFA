#pragma once
#include <string>
#include "token.h"
#include <string_view>
#include <iostream>

class Lexer
{
    private:
    char m_currentChar{};
    std::string m_currentWord{};
    std::string m_regex{};
    size_t m_position{0};

    void advance();
    void skipWhitespace();
    Token literal();   // handles literals
    Token special();   // handles a special character

    Token unknownToken();

    public:
    Lexer(const std::string& regex);
    Token getNextToken();
    bool isAtEnd() const;


    //test
    void fileContent() const {std::cout << m_regex;};
    
};
