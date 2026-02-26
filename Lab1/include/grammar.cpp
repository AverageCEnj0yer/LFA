#include "grammar.h"
#include <random>
#include <set>
#include <algorithm>



Grammar::Grammar(std::set<Symbol> terminals, std::set<Symbol> nonterminals,
                 Symbol start, std::vector<Production> productions)
    : m_terminals{terminals}
    , m_nonterminals{nonterminals}
    , m_start{start}
    , m_productions{productions}
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
            // For now, only handle single-symbol LHS for FA conversion
            if (production.lhs.size() != 1) continue;
            Symbol lhs = production.lhs[0];

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
            std::cout << "  ";
            for (const auto& sym : prod.lhs)
                std::cout << sym;
            std::cout << " → ";
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
//AKHFASBKJGNASSKMVOIAIWHBFAKSMDLASJFIAB
void Grammar::classifyGrammar() const
{
    // Flags for Chomsky classification
    bool isRegular = true;
    bool isContextFree = true;
    bool isContextSensitive = true;

    // Track if grammar is left-linear or right-linear (for strict regular grammar)
    bool foundLeftLinear = false;
    bool foundRightLinear = false;

    // Edge case: needed for context-sensitive grammar (S -> ε allowed only if S not on RHS)
    bool startAppearsOnRHS = false;
    for (const auto& p : m_productions) {
        if (std::find(p.rhs.begin(), p.rhs.end(), m_start) != p.rhs.end()) {
            startAppearsOnRHS = true;
            break; 
        }
    }

    // === Regular grammar check ===
    // All productions must be either right-linear or left-linear (not both)
    for (const auto& p : m_productions) 
    {
        // LHS must be a single non-terminal
        if (p.lhs.size() != 1 || m_nonterminals.find(p.lhs[0]) == m_nonterminals.end()) {
            isRegular = false;
            break;
        }
        // RHS: [terminal, non-terminal] (right-linear), [non-terminal, terminal] (left-linear), or [terminal] (terminal-only)
        if (p.rhs.size() == 2) {
            // Right-linear: A -> aB
            if (m_terminals.find(p.rhs[0]) != m_terminals.end() && m_nonterminals.find(p.rhs[1]) != m_nonterminals.end()) {
                foundRightLinear = true;
            // Left-linear: A -> Ba
            } else if (m_nonterminals.find(p.rhs[0]) != m_nonterminals.end() && m_terminals.find(p.rhs[1]) != m_terminals.end()) {
                foundLeftLinear = true;
            } else {
                // Not a valid regular production
                isRegular = false;
                break;
            }
        } else if (p.rhs.size() == 1) {
            // Terminal-only: A -> a
            if (!(m_terminals.find(p.rhs[0]) != m_terminals.end())) {
                isRegular = false;
                break;
            }
        } else if (p.rhs.empty()) {
            // Epsilon productions not allowed in regular grammars
            isRegular = false;
            break;
        }
        // If both left-linear and right-linear forms are found, not regular
        if(foundLeftLinear && foundRightLinear) {
            isRegular = false;
            break;
        }
    }

    // === Context-free grammar check ===
    // LHS must be a single non-terminal, RHS can be any string
    for (const auto& p : m_productions) 
    {
        if (p.lhs.size() != 1 || m_nonterminals.find(p.lhs[0]) == m_nonterminals.end()) {
            isContextFree = false;
            break;
        }
    }

    // === Context-sensitive grammar check ===
    // LHS and RHS are strings, LHS must have at least one non-terminal, |RHS| >= |LHS| (except S -> ε)
    for (const auto& p : m_productions) 
    {
        bool lhsHasNonTerminal = false;
        for (const auto& symbol : p.lhs) {
            if (m_nonterminals.find(symbol) != m_nonterminals.end()) {
                lhsHasNonTerminal = true;
                break;
            }
        }
        // Allow S -> ε only if S does not appear on RHS
        if (p.rhs.empty()) {
            if (!(p.lhs.size() == 1 && p.lhs[0] == m_start)) {
                isContextSensitive = false;
                break;
            }
            if (startAppearsOnRHS)
            {
                isContextSensitive = false;
                break;
            }
        } else {
            if (!lhsHasNonTerminal || p.rhs.size() < p.lhs.size()) {
                isContextSensitive = false;
                break;
            }
        }
    }

    // === Output the result ===
    if (isRegular)
        std::cout << "Grammar is Regular\n";
    else if (isContextFree)
        std::cout << "Grammar is Context-Free\n";
    else if (isContextSensitive)
        std::cout << "Grammar is Context-Sensitive\n";
    else
        std::cout << "Grammar is Recursively Enumerable\n";
}
