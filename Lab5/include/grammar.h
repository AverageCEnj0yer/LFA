#ifndef GRAMMAR_H
#define GRAMMAR_H


#include <string>
#include <vector>
#include <set>

using Symbol = std::string;
using RHS = std::vector<Symbol>;
using LHS = std::vector<Symbol>;


struct Production
{
    LHS lhs{};
    RHS rhs{};
};


class Grammar
{
public:
    Grammar(std::set<Symbol> terminals, std::set<Symbol> nonterminals,
            Symbol start, std::vector<Production> productions);
    void print() const;
    const std::set<Symbol>& terminals() const;
    const std::set<Symbol>& nonterminals() const;
    const Symbol& start() const;
    const std::vector<Production>& productions() const;

private:
    // member variables
    std::set<Symbol> m_terminals{};
    std::set<Symbol> m_nonterminals{};
    Symbol m_start{};
    std::vector<Production> m_productions{};
};


Grammar normalize(const Grammar& grammar);
bool isChomskyNormalForm(const Grammar& grammar);


#endif

