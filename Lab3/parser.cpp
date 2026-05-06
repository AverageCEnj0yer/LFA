#include "parser.h"
#include <stdexcept>

// Construction
Parser::Parser(Lexer& lexer) : m_lexer(lexer)
{
    // Pre-fill the two-token window
    m_current  = m_lexer.getNextToken();
    m_next     = m_lexer.getNextToken();
    m_nextNext = m_lexer.getNextToken();
}

//Token helpers
Token Parser::advance()
{
    Token prev  = m_current;
    m_current   = m_next;
    m_next      = m_nextNext;
    m_nextNext  = m_lexer.getNextToken();
    return prev;
}

Token Parser::expect(TokenType type, const std::string& errMsg)
{
    if (!check(type))
        throw ParseError(errMsg + " — got '" + m_current.lexeme + "'");
    return advance();
}

bool Parser::match(TokenType type)
{
    if (check(type)) { advance(); return true; }
    return false;
}

bool Parser::isTypeKeyword() const
{
    return check(TokenType::INT_TYPE)    ||
           check(TokenType::FLOAT_TYPE)  ||
           check(TokenType::STRING_TYPE) ||
           check(TokenType::BOOL_TYPE)   ||
           check(TokenType::VOID_TYPE);
}

//Top level
std::unique_ptr<ProgramNode> Parser::parse()
{
    std::vector<ASTNodePtr> stmts;
    while (!check(TokenType::EOFILE))
    {
        // TYPE IDENTIFIER "(" → function declaration
        if (isTypeKeyword()
            && checkNext(TokenType::IDENTIFIER)
            && checkNextNext(TokenType::LPAREN))
        {
            stmts.push_back(parseFunctionDecl());
        }
        else
        {
            stmts.push_back(parseStatement());
        }
    }
    return std::make_unique<ProgramNode>(std::move(stmts));
}

//Statements
ASTNodePtr Parser::parseStatement()
{
    if (isTypeKeyword())          return parseVarDecl();
    if (check(TokenType::RETURN)) return parseReturnStmt();
    if (check(TokenType::LCBRACKET)) return parseBlock();
    return parseExprStmt();
}

ASTNodePtr Parser::parseVarDecl()
{
    // TYPE IDENTIFIER ("=" expression)? ";"
    std::string typeName = m_current.lexeme;
    advance(); // consume type keyword

    Token nameToken = expect(TokenType::IDENTIFIER,
                             "Expected variable name after type '" + typeName + "'");

    ASTNodePtr initializer = nullptr;
    if (match(TokenType::ASSIGN))
        initializer = parseExpression();

    expect(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    return std::make_unique<VarDeclNode>(typeName, nameToken.lexeme,
                                        std::move(initializer));
}

ASTNodePtr Parser::parseReturnStmt()
{
    advance(); // consume 'return'
    ASTNodePtr value = nullptr;
    if (!check(TokenType::SEMICOLON))
        value = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' after return");
    return std::make_unique<ReturnNode>(std::move(value));
}

ASTNodePtr Parser::parseBlock()
{
    advance(); // consume '{'
    std::vector<ASTNodePtr> stmts;
    while (!check(TokenType::RCBRACKET) && !check(TokenType::EOFILE))
        stmts.push_back(parseStatement());
    expect(TokenType::RCBRACKET, "Expected '}' to close block");
    return std::make_unique<BlockNode>(std::move(stmts));
}

ASTNodePtr Parser::parseExprStmt()
{
    ASTNodePtr expr = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' after expression");
    return std::make_unique<ExprStmtNode>(std::move(expr));
}

//Expressions
ASTNodePtr Parser::parseExpression()
{
    return parseAssignment();
}

ASTNodePtr Parser::parseAssignment()
{
    // Use one-token lookahead: IDENTIFIER followed by "=" (not "==")
    if (check(TokenType::IDENTIFIER) && checkNext(TokenType::ASSIGN))
    {
        std::string name = m_current.lexeme;
        advance(); // consume IDENTIFIER
        advance(); // consume '='
        ASTNodePtr value = parseExpression(); // right-associative
        return std::make_unique<AssignNode>(name, std::move(value));
    }
    return parseLogicalOr();
}

ASTNodePtr Parser::parseLogicalOr()
{
    ASTNodePtr left = parseLogicalAnd();
    while (check(TokenType::OR))
    {
        std::string op = m_current.lexeme;
        advance();
        ASTNodePtr right = parseLogicalAnd();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

ASTNodePtr Parser::parseLogicalAnd()
{
    ASTNodePtr left = parseEquality();
    while (check(TokenType::AND))
    {
        std::string op = m_current.lexeme;
        advance();
        ASTNodePtr right = parseEquality();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

ASTNodePtr Parser::parseEquality()
{
    ASTNodePtr left = parseComparison();
    while (check(TokenType::EQ) || check(TokenType::NOTEQ))
    {
        std::string op = m_current.lexeme;
        advance();
        ASTNodePtr right = parseComparison();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

ASTNodePtr Parser::parseComparison()
{
    ASTNodePtr left = parseTerm();
    while (check(TokenType::LESS)    || check(TokenType::GREATER) ||
           check(TokenType::LESSEQ)  || check(TokenType::GREATEREQ))
    {
        std::string op = m_current.lexeme;
        advance();
        ASTNodePtr right = parseTerm();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

ASTNodePtr Parser::parseTerm()
{
    ASTNodePtr left = parseFactor();
    while (check(TokenType::PLUS) || check(TokenType::MINUS))
    {
        std::string op = m_current.lexeme;
        advance();
        ASTNodePtr right = parseFactor();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

ASTNodePtr Parser::parseFactor()
{
    ASTNodePtr left = parsePower();
    while (check(TokenType::MULTIPLY) || check(TokenType::DIVIDE))
    {
        std::string op = m_current.lexeme;
        advance();
        ASTNodePtr right = parsePower();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }
    return left;
}

ASTNodePtr Parser::parsePower()
{
    // Right-associative: a ^ b ^ c  →  a ^ (b ^ c)
    ASTNodePtr base = parseUnary();
    if (check(TokenType::EXPONENT))
    {
        std::string op = m_current.lexeme;
        advance();
        ASTNodePtr exponent = parsePower(); // recursive for right-assoc
        return std::make_unique<BinaryOpNode>(op, std::move(base), std::move(exponent));
    }
    return base;
}

ASTNodePtr Parser::parseUnary()
{
    if (check(TokenType::MINUS))
    {
        advance();
        return std::make_unique<UnaryOpNode>("-", parseUnary());
    }
    if (check(TokenType::NOT))
    {
        advance();
        return std::make_unique<UnaryOpNode>("NOT", parseUnary());
    }
    return parsePrimary();
}

ASTNodePtr Parser::parsePrimary()
{
    // Integer literal
    if (check(TokenType::INT))
    {
        int val = std::stoi(m_current.lexeme);
        advance();
        return std::make_unique<IntLiteralNode>(val);
    }

    // Float literal
    if (check(TokenType::FLOAT))
    {
        float val = std::stof(m_current.lexeme);
        advance();
        return std::make_unique<FloatLiteralNode>(val);
    }

    // String literal
    if (check(TokenType::STRING))
    {
        std::string val = m_current.lexeme;
        advance();
        return std::make_unique<StringLiteralNode>(std::move(val));
    }

    // Boolean literals
    if (check(TokenType::TRUE))  { advance(); return std::make_unique<BoolLiteralNode>(true);  }
    if (check(TokenType::FALSE)) { advance(); return std::make_unique<BoolLiteralNode>(false); }

    // Identifier or function call
    if (check(TokenType::IDENTIFIER))
    {
        std::string name = m_current.lexeme;
        advance();

        // Function call: name "(" argList ")"
        if (check(TokenType::LPAREN))
        {
            advance(); // consume '('
            std::vector<ASTNodePtr> args = parseArgList();
            expect(TokenType::RPAREN, "Expected ')' after argument list");
            return std::make_unique<FunctionCallNode>(name, std::move(args));
        }

        return std::make_unique<IdentifierNode>(name);
    }

    // Grouped expression: "(" expression ")"
    if (check(TokenType::LPAREN))
    {
        advance(); // consume '('
        ASTNodePtr expr = parseExpression();
        expect(TokenType::RPAREN, "Expected ')' after grouped expression");
        return expr;
    }

    throw ParseError("Unexpected token '" + m_current.lexeme + "' in expression");
}

std::vector<ASTNodePtr> Parser::parseArgList()
{
    std::vector<ASTNodePtr> args;
    if (check(TokenType::RPAREN))
        return args; // empty argument list

    args.push_back(parseExpression());
    while (match(TokenType::COMMA))
        args.push_back(parseExpression());

    return args;
}



ASTNodePtr Parser::parseFunctionDecl()
{
    // returnType
    std::string returnType = m_current.lexeme;
    advance();

    // name
    std::string name = m_current.lexeme;
    advance();

    // "(" paramList ")"
    expect(TokenType::LPAREN, "Expected '(' after function name");
    auto params = parseParamList();
    expect(TokenType::RPAREN, "Expected ')' after parameter list");

    // body must be a block
    if (!check(TokenType::LCBRACKET))
        throw ParseError("Expected '{' to begin function body for '" + name + "'");

    ASTNodePtr body = parseBlock();

    return std::make_unique<FunctionDeclNode>(returnType, name,
                                             std::move(params), std::move(body));
}


std::vector<std::unique_ptr<ParameterNode>> Parser::parseParamList()
{
    std::vector<std::unique_ptr<ParameterNode>> params;

    if (check(TokenType::RPAREN))
        return params; // void parameter list

    // first parameter
    if (!isTypeKeyword())
        throw ParseError("Expected type in parameter list, got '" + m_current.lexeme + "'");

    std::string typeName = m_current.lexeme;
    advance();
    Token nameToken = expect(TokenType::IDENTIFIER, "Expected parameter name after type");
    params.push_back(std::make_unique<ParameterNode>(typeName, nameToken.lexeme));

    // additional parameters
    while (match(TokenType::COMMA))
    {
        if (!isTypeKeyword())
            throw ParseError("Expected type after ',' in parameter list");

        typeName = m_current.lexeme;
        advance();
        nameToken = expect(TokenType::IDENTIFIER, "Expected parameter name after type");
        params.push_back(std::make_unique<ParameterNode>(typeName, nameToken.lexeme));
    }

    return params;
}
