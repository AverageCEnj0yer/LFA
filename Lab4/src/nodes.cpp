#include "nodes.h"
#include <algorithm>

static std::random_device rd;
static std::mt19937 gen(rd());

// --- LiteralNode ---
std::string LiteralNode::generate() {
    return std::string(1, m_value);
}
void LiteralNode::printStructure(int depth) {
    std::cout << indent(depth) << "Literal: '" << m_value << "'\n";
}

// --- SequenceNode ---
SequenceNode::~SequenceNode() {
    for (Node* child : m_children) {
        delete child; // Manual cleanup
    }
}
std::string SequenceNode::generate() {
    std::string result = "";
    for (Node* child : m_children) result += child->generate();
    return result;
}
void SequenceNode::printStructure(int depth) {
    std::cout << indent(depth) << "Sequence:\n";
    for (Node* child : m_children) child->printStructure(depth + 1);
}

// --- AlternationNode ---
AlternationNode::~AlternationNode() {
    for (Node* opt : m_options) {
        delete opt; // Manual cleanup
    }
}
std::string AlternationNode::generate() {
    if (m_options.empty()) return "";
    std::uniform_int_distribution<> dist(0, m_options.size() - 1);
    return m_options[dist(gen)]->generate();
}
void AlternationNode::printStructure(int depth) {
    std::cout << indent(depth) << "Alternation (Choice):\n";
    for (Node* opt : m_options) opt->printStructure(depth + 1);
}

// --- RepeatNode ---
RepeatNode::~RepeatNode() {
    delete m_child; // Clean up the wrapped node
}
std::string RepeatNode::generate() {
    int limit = (m_max == -1) ? 5 : m_max;
    std::uniform_int_distribution<> dist(m_min, limit);
    int count = dist(gen);
    
    std::string result = "";
    for (int i = 0; i < count; ++i) result += m_child->generate();
    return result;
}
void RepeatNode::printStructure(int depth) {
    std::cout << indent(depth) << "Repeat (" << m_min << " to " 
              << (m_max == -1 ? 5 : m_max) << "):\n";
    m_child->printStructure(depth + 1);
}
