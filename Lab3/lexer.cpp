#include "lexer.h"

Lexer::Lexer(const std::string& fileName)
{
    std::ifstream file{fileName};
    if (!file)
    {
        std::cerr << "File could not be opened\n";
        std::exit(1);
    }

    m_fileContent = std::string((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());

    if (!m_fileContent.empty())
        m_currentChar = m_fileContent[0];
    else
        m_currentChar = '\0';
}

void Lexer::advance()
{
    m_position++;

    if (m_position < m_fileContent.size())
        m_currentChar = m_fileContent[m_position];
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
    while (!isAtEnd())
    {
        if (std::isspace(m_currentChar))
        {
            skipWhitespace();
            continue;
        }

        if (std::isdigit(m_currentChar))
            return number();

        if (std::isalpha(m_currentChar))
            return identifierOrKeyword();

        switch (m_currentChar)
        {
        case '+': advance(); return Token{TokenType::PLUS, "+"};
        case '-': advance(); return Token{TokenType::MINUS, "-"};
        case '*': advance(); return Token{TokenType::MULTIPLY, "*"};
        case '/': advance(); return Token{TokenType::DIVIDE, "/"};
        case '^': advance(); return Token{TokenType::EXPONENT, "^"};

        case '=':
        {
            advance();
            if (m_currentChar == '=')
            {
                advance();
                return Token{TokenType::EQ, "=="};
            }
            return Token{TokenType::ASSIGN, "="};
        }

        case '<':
        {
            advance();
            if (m_currentChar == '=')
            {
                advance();
                return Token{TokenType::LESSEQ, "<="};
            }
            return Token{TokenType::LESS, "<"};
        }

        case '>':
        {
            advance();
            if (m_currentChar == '=')
            {
                advance();
                return Token{TokenType::GREATEREQ, ">="};
            }
            return Token{TokenType::GREATER, ">"};
        }

        case '!':
        {
            if (m_position + 1 < m_fileContent.size() &&
                m_fileContent[m_position + 1] == '=')
            {
                advance();
                advance();
                return Token{TokenType::NOTEQ, "!="};
            }
            break;
        }

        case '(':
            advance();
            return Token{TokenType::LPAREN, "("};

        case ')':
            advance();
            return Token{TokenType::RPAREN, ")"};

        case ',':
            advance();
            return Token{TokenType::COMMA, ","};

        case ';':
            advance();
            return Token{TokenType::SEMICOLON, ";"};

        case '"':
            return stringLiteral();

        case '{':
            advance();
            return Token{TokenType::LCBRACKET, "{"};

        case '}':
            advance();
            return Token{TokenType::RCBRACKET, "}"};
        }

        return unknownToken();
    }

    return Token{TokenType::EOFILE, ""};
}

Token Lexer::number()
{
    size_t startIDX = m_position;
    bool isFloat = false;

    while (std::isdigit(m_currentChar) || m_currentChar == '.')
    {
        if (m_currentChar == '.')
        {
            if (isFloat)  // second dot detected
            {
                while (!isAtEnd() && !std::isspace(m_currentChar))
                    advance();

                std::string value =
                    m_fileContent.substr(startIDX, m_position - startIDX);

                return Token{TokenType::UNKNOWN, value};
            }

            isFloat = true;
        }

        advance();
    }

    // detect cases like 123abc
    if (std::isalpha(m_currentChar))
    {
        while (!isAtEnd() && !std::isspace(m_currentChar))
            advance();

        std::string value =
            m_fileContent.substr(startIDX, m_position - startIDX);

        return Token{TokenType::UNKNOWN, value};
    }

    std::string value =
        m_fileContent.substr(startIDX, m_position - startIDX);

    if (isFloat)
        return Token{TokenType::FLOAT, value};

    return Token{TokenType::INT, value};
}

Token Lexer::identifierOrKeyword()
{
    size_t startIDX = m_position;

    while (std::isalnum(m_currentChar) || m_currentChar == '_')
        advance();

    std::string value =
        m_fileContent.substr(startIDX, m_position - startIDX);

    // type keywords
    if (value == "int")    return Token{TokenType::INT_TYPE, value};
    if (value == "float")  return Token{TokenType::FLOAT_TYPE, value};
    if (value == "string") return Token{TokenType::STRING_TYPE, value};
    if (value == "bool")   return Token{TokenType::BOOL_TYPE, value};
    if (value == "void")   return Token{TokenType::VOID_TYPE, value};

    // boolean literals
    if (value == "true")  return Token{TokenType::TRUE, value};
    if (value == "false") return Token{TokenType::FALSE, value};

    // other keywords
    if (value == "return") return Token{TokenType::RETURN, value};

    if (value == "AND") return Token{TokenType::AND, value};
    if (value == "OR")  return Token{TokenType::OR, value};
    if (value == "NOT") return Token{TokenType::NOT, value};

    return Token{TokenType::IDENTIFIER, value};
}

Token Lexer::stringLiteral()
{
    advance(); // skip opening quote

    size_t startIDX = m_position;

    while (m_currentChar != '"' && !isAtEnd())
        advance();

    std::string value =
        m_fileContent.substr(startIDX, m_position - startIDX);

    advance(); // skip closing quote

    return Token{TokenType::STRING, value};
}

Token Lexer::unknownToken()
{
    char c = m_currentChar;
    advance();

    return Token{TokenType::UNKNOWN, std::string(1, c)};
}

bool Lexer::isAtEnd() const
{
    return m_position >= m_fileContent.size();
}
