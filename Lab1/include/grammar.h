#ifndef GRAMMAR_H
#define GRAMMAR_H


#include <string>
#include <vector>
#include <set>
#include "finiteAutomaton.h"


using Symbol = std::string;
using RHS = std::vector<Symbol>;

struct Production
{
    Symbol lhs {};
    RHS rhs {};
};

class Grammar
{
    public:
    Grammar(std::set<Symbol> terminals, std::set<Symbol> nonterminals,  
            Symbol start, std::vector<Production> productions);
    void print() const;
    const FiniteAutomaton& toFiniteAutomaton() const;
    void generateWord() const;
    
    private:
    //member variables
    std::set<Symbol> m_terminals {};
    std::set<Symbol> m_nonterminals {};
    Symbol m_start {};
    std::vector<Production> m_productions {};

    mutable bool m_faGenerated {false};       // tracks if FA is already generated
    mutable FiniteAutomaton m_cachedFA {};       // stores the automaton
    
    //member functions
    FiniteAutomaton getFiniteAutomaton() const;


};


#endif

