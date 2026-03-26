#ifndef NODES_H
#define NODES_H

#include <string>
#include <iostream>
#include <vector>
#include <random>


// ========================================
// 1. Base Node Class
// ========================================
class Node {
public:
    virtual ~Node() = default; // Virtual destructor is MANDATORY for polymorphism
    virtual std::string generate() = 0;
    virtual void printStructure(int depth = 0) = 0;

protected:
    std::string indent(int d) { return std::string(d * 2, ' '); }
};

// ========================================
// 2. Specialized Node Types
// ========================================
class LiteralNode : public Node {
private:
    char m_value{};
public:
    LiteralNode(char v) : m_value(v) {}
    std::string generate() override;
    void printStructure(int depth) override;
};

class SequenceNode : public Node {
private:
    std::vector<Node*> m_children{};
public:
    ~SequenceNode() override; // Must delete children
    void addChild(Node* node) { m_children.push_back(node); }
    std::string generate() override;
    void printStructure(int depth) override;
};

class AlternationNode : public Node {
private:
    std::vector<Node*> m_options{};
public:
    ~AlternationNode() override; // Must delete options
    void addOption(Node* node) { m_options.push_back(node); }
    std::string generate() override;
    void printStructure(int depth) override;
};

class RepeatNode : public Node {
private:
    int m_min{};
    int m_max{};
    Node* m_child{}; // Single child pointer
public:
    RepeatNode(Node* child, int min, int max) 
        : m_child(child), m_min(min), m_max(max) {}
    ~RepeatNode() override; // Must delete the child
    std::string generate() override;
    void printStructure(int depth) override;
};

#endif
