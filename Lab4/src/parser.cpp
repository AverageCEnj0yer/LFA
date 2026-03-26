#include "parser.h"


void Parser::consume(TokenType expectedType)
{
    if (m_currentToken.type == expectedType)
        m_currentToken = m_lexer.getNextToken();
    else   
    {    
        std::cerr << "Invalid token. expected: " << tokenTypeToString(expectedType) << "\n";
        std::exit(1);
    }
}

//handles the OR logic ( | )
Node* Parser::parseExpression()
{
    Node* node = parseSequence();
    AlternationNode* alt = nullptr;

    while (m_currentToken.type == TokenType::PIPE) // consume all pipes and make add the subnodes to children
    {
        if (!alt)
        {
            alt = new AlternationNode();
            alt->addOption(node);
        }
        consume(TokenType::PIPE);
        alt->addOption(parseSequence());
    }

    return alt ? alt : node; // if we didnt have a alternation node, we return the node passed as is
}

Node* Parser::parseSequence()
{
    Node* node = parseFactor();
    SequenceNode* seq  = nullptr;

    while (m_currentToken.type == TokenType::LPAREN ||
            m_currentToken.type == TokenType::LITERAL )
    {
        if (!seq)
        {
            seq = new SequenceNode();
            seq->addChild(node);
        }
        seq->addChild(parseFactor());
    }
    
    return seq ? seq : node;

}

Node* Parser::parseFactor()
{
    Node* node = parseBase();

    //handle +
    if (m_currentToken.type == TokenType::PLUS)
    {
        consume(TokenType::PLUS);
        node = new RepeatNode(node, 1, -1);
    }
    //handle *
    if (m_currentToken.type == TokenType::STAR)
    {
        consume(TokenType::STAR);
        node = new RepeatNode(node, 0, -1);
    }
    //handle ?
    if (m_currentToken.type == TokenType::QUESTION)
    {
        consume(TokenType::QUESTION);
        node = new RepeatNode(node, 0, 1);
    }
    //handle ^
    if (m_currentToken.type == TokenType::POWER)
    {
        // Convert the string lexeme like "5" to an actual integer
        int count = std::stoi(m_currentToken.lexeme); 
        consume(TokenType::POWER);
        node = new RepeatNode(node, count, count);
    }
    return node;
}

Node* Parser::parseBase()
{
    if (m_currentToken.type == TokenType::LPAREN)
    {
        consume(TokenType::LPAREN);
        Node* node = parseExpression();
        consume(TokenType::RPAREN);
        return node;
    }

    if (m_currentToken.type == TokenType::LITERAL) {
        Node* lit = new LiteralNode(m_currentToken.lexeme[0]);
        consume(TokenType::LITERAL);
        return lit;
    }

    // If its not a ( and not a Literal then its a syntax error (very edge case for string end)
    std::cerr << "Unexpected token: " << tokenTypeToString(m_currentToken.type) << "\n";
    std::exit(1);
}
