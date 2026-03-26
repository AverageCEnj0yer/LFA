#include "lexer.h"

Lexer::Lexer(const std::string& regex) : m_regex(regex), m_position(0)
{
    if (!m_regex.empty())
        m_currentChar = m_regex[0];
    else
        m_currentChar = '\0';
}

bool Lexer::isAtEnd() const 
{
    return m_position >= m_regex.size() || m_currentChar == '\0';
}

void Lexer::advance() 
{
    m_position++;
    if (m_position < m_regex.size())
        m_currentChar = m_regex[m_position];
    else
        m_currentChar = '\0';
}

void Lexer::skipWhitespace() 
{
    while (!isAtEnd() && std::isspace(m_currentChar))
        advance();
}

Token Lexer::getNextToken() 
{
    skipWhitespace();

    if (isAtEnd()) return Token{TokenType::END, ""};

    // 1. Handle Escaping (for example /+)
    if (m_currentChar == '/') 
    {
        if (m_position + 1 < m_regex.length()) // Check bounds FIRST
        {
            char nextChar = m_regex[m_position + 1];
            if (nextChar == '/' || nextChar == '+' || nextChar == '*' ||
                nextChar == '|' || nextChar == '(' || nextChar == ')' ||
                nextChar == '^') 
            {
                advance(); // Skip the '/'
                return literal(); 
            }
        }
return literal(); // If bounds check fails or it's not a special char, just return the '/' as a literal
    }

    // 2. Handle Special Operators
    switch (m_currentChar) 
    {
        case '(':
        case ')': 
        case '+': 
        case '*': 
        case '|': 
        case '?': 
        case '^':
            return special();
    }


    // Default case accepts any type of characters as literals
    Token literal = Token{TokenType::LITERAL, std::string{m_currentChar}};
    advance();
    return literal;

}

Token Lexer::literal() 
{
    Token t{TokenType::LITERAL, std::string{1, m_currentChar}};
    advance();
    return t;
}

Token Lexer::special() 
{
    TokenType type;
    std::string val{m_currentChar};

    switch (m_currentChar) 
    {
        case '(': type = TokenType::LPAREN; break;
        case ')': type = TokenType::RPAREN; break;
        case '+': type = TokenType::PLUS; break;
        case '*': type = TokenType::STAR; break;
        case '|': type = TokenType::PIPE; break;
        case '?': type = TokenType::QUESTION; break;
        case '^': 
        {
            advance(); // move past '^'
            // Capture the number following '^'
            std::string num = "";
            while (!isAtEnd() && std::isdigit(m_currentChar)) 
            {
                num += m_currentChar;
                advance();
            }
            return Token{TokenType::POWER, num};
        }
        default: type = TokenType::UNKNOWN; break;
    }
    
    if (type != TokenType::POWER) advance(); 
    return Token{type, val};
}
