#ifndef FINITE_AUTOMATON
#define FINITE_AUTOMATON

#include <string>
#include <utility>
#include <set>
#include <map>
#include <iostream>


using Symbol = std::string;

class FiniteAutomaton
{
    public:
    FiniteAutomaton(std::set<Symbol> states, std::set<Symbol> alphabet,
                    std::map<std::pair<Symbol, Symbol>, std::set<Symbol>> transitions,
                    Symbol initialState, std::set<Symbol> finalStates);
    FiniteAutomaton() = default;
    void print() const;
    bool stringBelongsToLanguage(std::string_view) const;

    //getters
    const std::set<Symbol>& states() const { return m_states; }
    const std::set<Symbol>& alphabet() const { return m_alphabet; }
    const std::set<Symbol>& finalStates() const { return m_finalStates; }
    const Symbol& initialState() const { return m_initialState; }
    const std::map<std::pair<Symbol, Symbol>, std::set<Symbol>>& transitions() const { return m_transitions; }


    private:
    std::set<Symbol> m_states {};
    std::set<Symbol> m_alphabet {};
    std::map<std::pair<Symbol, Symbol>, std::set<Symbol>> m_transitions {};
    Symbol m_initialState {};
    std::set<Symbol> m_finalStates {};


};

#endif
