#include "grammar.h"
#include <random>
#include <set>


Grammar::Grammar(std::set<Symbol> terminals, std::set<Symbol> nonterminals,  
                 Symbol start, std::vector<Production> productions)
            : m_terminals {terminals}
            , m_nonterminals {nonterminals}
            , m_start {start}
            , m_productions {productions} 
    {

    }
FiniteAutomaton Grammar::getFiniteAutomaton() const
    {
        //======IMPORTANT - THIS ASSUMES THE GRAMMAR IS A RIGHT LINEAR REGULAR GRAMMAR======
        // IF IT IS NOT, THIS FUNCTION WILL NOT WORK CORRECTLY.

        std::set<Symbol> states {m_nonterminals};
        states.insert("X"); // extra accepting state

        std::set<Symbol> alphabet {m_terminals};
        Symbol initialState {m_start}; 
        std::set<Symbol> finalStates {"X"};

        // map (from_state, terminal) -> set of next states (NFA-ready)
        std::map<std::pair<Symbol, Symbol>, std::set<Symbol>> transitions {};

        for (const auto& production : m_productions)
        {
            Symbol lhs = production.lhs;

            if (production.rhs.empty())
            {
                // Empty production: treat as terminal-only to accepting state
                transitions[{lhs, ""}].insert("X"); // "" can represent epsilon if needed
            }
            else if (production.rhs.size() == 1)
            {
                // Terminal-only production: lhs -> terminal -> X
                Symbol terminal = production.rhs[0];
                transitions[{lhs, terminal}].insert("X");
            }
            else
            {
                // Standard right-linear: lhs -> terminal -> next nonterminal
                Symbol terminal = production.rhs[0];
                Symbol nextNonterminal = production.rhs[1];
                transitions[{lhs, terminal}].insert(nextNonterminal);
            }
        }
        
        return FiniteAutomaton {states, alphabet, transitions, initialState, finalStates};
    }
void Grammar::print() const
    {
        std::cout << "Terminals: { ";
        for (const auto& t : m_terminals)
            std::cout << t << " ";
        std::cout << "}\n";

        std::cout << "Nonterminals: { ";
        for (const auto& nt : m_nonterminals)
            std::cout << nt << " ";
        std::cout << "}\n";

        std::cout << "Start symbol: " << m_start << "\n";

        std::cout << "Productions:\n";
        for (const auto& prod : m_productions)
        {
            std::cout << "  " << prod.lhs << " → ";
            if (prod.rhs.empty())
            {
                std::cout << "ε"; // epsilon for empty RHS
            }
            else
            {
                for (const auto& sym : prod.rhs)
                    std::cout << sym;
            }
            std::cout << "\n";
        }
    }
const FiniteAutomaton& Grammar::toFiniteAutomaton() const
{
    if (!m_faGenerated)
    {
        m_cachedFA = getFiniteAutomaton(); // generate only once
        m_faGenerated = true;
    }
    return m_cachedFA;
}
void Grammar::generateWord() const
{
    std::random_device rd{};
    std::seed_seq seed{rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
    std::mt19937 mt {seed};

    const FiniteAutomaton& fa = m_cachedFA;

    if (!m_faGenerated)
        toFiniteAutomaton();

    Symbol current_state = fa.initialState();
    std::string word;
    const int maxSteps {20};

    for (int steps = 0;
         steps < maxSteps && fa.finalStates().find(current_state) == fa.finalStates().end();
         ++steps)
    {
        std::vector<std::pair<Symbol, Symbol>> choices {}; // represents {input, next state}

        for(const auto& [key, destination] : fa.transitions())
        {
            const auto& [from, input] = key;
            if (from == current_state)
            {
                for(const auto& next_state : destination)
                    choices.push_back({input, next_state});
            }
        }

        if (choices.empty())
            break;

        std::uniform_int_distribution<size_t> dist(0, choices.size() - 1);
        auto [input, next] = choices[dist(mt)];

        if (!input.empty())
            word += input;

        current_state = next;
    }
    std::cout << word;
}
