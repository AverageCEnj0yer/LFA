#pragma once
#include <string>
#include <fstream>
#include "token.h"
#include <string_view>
#include <iostream>

class Lexer
{
    private:
    char m_currentChar{};
    std::string m_currentWord{};
    std::string m_fileContent{};
    size_t m_position{0};

    void advance();
    void skipWhitespace();
    Token number();
    Token identifierOrKeyword();
    Token stringLiteral();
    Token unknownToken();


    public:
    Lexer(const std::string& fileName);
    Token getNextToken();
    bool isAtEnd() const;

    //test
    void fileContent() const {std::cout << m_fileContent;};
    
};
