#pragma once
#include "lexer.h"
#include "ast.h"
#include <stdexcept>
#include <string>

//Parse error
class ParseError : public std::runtime_error
{
public:
    explicit ParseError(const std::string& msg)
        : std::runtime_error("ParseError: " + msg) {}
};

// Parser
/*
 * Grammar (highest precedence last):
 *
 *  program     → statement* EOF
 *  statement   → varDecl | returnStmt | block | exprStmt
 *  varDecl     → TYPE IDENTIFIER ("=" expression)? ";"
 *  returnStmt  → "return" expression? ";"
 *  block       → "{" statement* "}"
 *  exprStmt    → expression ";"
 *
 *  expression  → assignment
 *  assignment  → IDENTIFIER "=" expression          (right-assoc)
 *              | logicalOr
 *  logicalOr   → logicalAnd  ("OR"  logicalAnd)*
 *  logicalAnd  → equality    ("AND" equality)*
 *  equality    → comparison  ("==" | "!=" comparison)*
 *  comparison  → term        ("<" | ">" | "<=" | ">=" term)*
 *  term        → factor      ("+" | "-" factor)*
 *  factor      → power       ("*" | "/" power)*
 *  power       → unary       ("^" power)?            (right-assoc)
 *  unary       → ("-" | "NOT") unary | primary
 *  primary     → INT | FLOAT | STRING | "true" | "false"
 *              | IDENTIFIER "(" argList ")"
 *              | IDENTIFIER
 *              | "(" expression ")"
 *  argList     → (expression ("," expression)*)?
 */
class Parser
{
public:
    explicit Parser(Lexer& lexer);

    std::unique_ptr<ProgramNode> parse();

private:
    Lexer& m_lexer;
    Token  m_current;   // token being examined
    Token  m_next;      // one token of lookahead

    //Token helpers
    Token advance();
    Token expect(TokenType type, const std::string& errMsg);
    bool  check    (TokenType type) const { return m_current.type == type; }
    bool  checkNext(TokenType type) const { return m_next.type    == type; }
    bool  match    (TokenType type);
    bool  isTypeKeyword() const;

    //Grammar rules
    ASTNodePtr parseStatement();
    ASTNodePtr parseVarDecl();
    ASTNodePtr parseReturnStmt();
    ASTNodePtr parseBlock();
    ASTNodePtr parseExprStmt();

    ASTNodePtr parseExpression();
    ASTNodePtr parseAssignment();
    ASTNodePtr parseLogicalOr();
    ASTNodePtr parseLogicalAnd();
    ASTNodePtr parseEquality();
    ASTNodePtr parseComparison();
    ASTNodePtr parseTerm();
    ASTNodePtr parseFactor();
    ASTNodePtr parsePower();
    ASTNodePtr parseUnary();
    ASTNodePtr parsePrimary();

    std::vector<ASTNodePtr> parseArgList();
};
