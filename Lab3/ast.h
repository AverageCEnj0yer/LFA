#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>

// Base
struct ASTNode
{
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;

protected:
    void printIndent(int indent) const
    {
        for (int i = 0; i < indent; ++i)
            std::cout << "  ";
    }
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

//Literals
struct IntLiteralNode : ASTNode
{
    int value;
    explicit IntLiteralNode(int v) : value(v) {}

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "IntLiteral(" << value << ")\n";
    }
};

struct FloatLiteralNode : ASTNode
{
    float value;
    explicit FloatLiteralNode(float v) : value(v) {}

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "FloatLiteral(" << value << ")\n";
    }
};

struct StringLiteralNode : ASTNode
{
    std::string value;
    explicit StringLiteralNode(std::string v) : value(std::move(v)) {}

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "StringLiteral(\"" << value << "\")\n";
    }
};

struct BoolLiteralNode : ASTNode
{
    bool value;
    explicit BoolLiteralNode(bool v) : value(v) {}

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "BoolLiteral(" << (value ? "true" : "false") << ")\n";
    }
};

//Identifier
struct IdentifierNode : ASTNode
{
    std::string name;
    explicit IdentifierNode(std::string n) : name(std::move(n)) {}

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Identifier(" << name << ")\n";
    }
};

//Expressions
struct BinaryOpNode : ASTNode
{
    std::string  op;
    ASTNodePtr   left;
    ASTNodePtr   right;

    BinaryOpNode(std::string op, ASTNodePtr l, ASTNodePtr r)
        : op(std::move(op)), left(std::move(l)), right(std::move(r)) {}

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "BinaryOp(" << op << ")\n";
        left ->print(indent + 1);
        right->print(indent + 1);
    }
};

struct UnaryOpNode : ASTNode
{
    std::string op;
    ASTNodePtr  operand;

    UnaryOpNode(std::string op, ASTNodePtr operand)
        : op(std::move(op)), operand(std::move(operand)) {}

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "UnaryOp(" << op << ")\n";
        operand->print(indent + 1);
    }
};

struct FunctionCallNode : ASTNode
{
    std::string              name;
    std::vector<ASTNodePtr>  args;

    FunctionCallNode(std::string name, std::vector<ASTNodePtr> args)
        : name(std::move(name)), args(std::move(args)) {}

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "FunctionCall(" << name << ")\n";
        for (const auto& arg : args)
            arg->print(indent + 1);
    }
};

//Statements
struct AssignNode : ASTNode
{
    std::string name;
    ASTNodePtr  value;

    AssignNode(std::string name, ASTNodePtr value)
        : name(std::move(name)), value(std::move(value)) {}

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Assign(" << name << ")\n";
        value->print(indent + 1);
    }
};

struct VarDeclNode : ASTNode
{
    std::string typeName;
    std::string varName;
    ASTNodePtr  initializer; // nullable

    VarDeclNode(std::string type, std::string name, ASTNodePtr init)
        : typeName(std::move(type)), varName(std::move(name)),
          initializer(std::move(init)) {}

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "VarDecl(" << typeName << " " << varName << ")\n";
        if (initializer)
            initializer->print(indent + 1);
    }
};

struct ReturnNode : ASTNode
{
    ASTNodePtr value; // nullable (bare return)

    explicit ReturnNode(ASTNodePtr v) : value(std::move(v)) {}

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Return\n";
        if (value)
            value->print(indent + 1);
    }
};

struct ExprStmtNode : ASTNode
{
    ASTNodePtr expr;

    explicit ExprStmtNode(ASTNodePtr e) : expr(std::move(e)) {}

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "ExprStmt\n";
        expr->print(indent + 1);
    }
};

struct BlockNode : ASTNode
{
    std::vector<ASTNodePtr> statements;

    explicit BlockNode(std::vector<ASTNodePtr> stmts)
        : statements(std::move(stmts)) {}

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Block\n";
        for (const auto& stmt : statements)
            stmt->print(indent + 1);
    }
};

//Root
struct ProgramNode : ASTNode
{
    std::vector<ASTNodePtr> statements;

    explicit ProgramNode(std::vector<ASTNodePtr> stmts)
        : statements(std::move(stmts)) {}

    void print(int indent = 0) const override
    {
        std::cout << "Program\n";
        for (const auto& stmt : statements)
            stmt->print(indent + 1);
    }
};
