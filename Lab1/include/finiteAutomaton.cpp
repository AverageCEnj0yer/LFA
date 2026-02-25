#include "finiteAutomaton.h"

FiniteAutomaton::FiniteAutomaton(std::set<Symbol> states, std::set<Symbol> alphabet,
                    std::map<std::pair<Symbol, Symbol>, std::set<Symbol>> transitions,
                    Symbol initialState, std::set<Symbol> finalStates)
                    : m_states {states}
                    , m_alphabet {alphabet}
                    , m_transitions {transitions}
                    , m_initialState {initialState}
                    , m_finalStates {finalStates}
    {

    }
void FiniteAutomaton::print() const
    {
        std::cout << "States: { ";
        for (const auto& s : m_states)
            std::cout << s << " ";
        std::cout << "}\n";

        std::cout << "Alphabet: { ";
        for (const auto& a : m_alphabet)
            std::cout << a << " ";
        std::cout << "}\n";

        std::cout << "Initial state: " << m_initialState << "\n";

        std::cout << "Final states: { ";
        for (const auto& f : m_finalStates)
            std::cout << f << " ";
        std::cout << "}\n";

        std::cout << "Transitions:\n";
        for (const auto& [key, destinations] : m_transitions)
        {
            const auto& [from, input] = key;
            for (const auto& to : destinations)
            {
                std::cout << "  " << from << " --" 
                          << (input.empty() ? "ε" : input) 
                          << "--> " << to << "\n";
            }
        }
    }
    
//this assumes that all symbols are a single character
bool FiniteAutomaton::stringBelongsToLanguage(std::string_view input) const
{
    std::set<Symbol> currentStates { m_initialState };

    for (char c : input)
    {
        Symbol symbol(1, c); // convert char → string
        std::set<Symbol> nextStates;

        for (const auto& state : currentStates)
        {
            auto it = m_transitions.find({state, symbol});
            if (it != m_transitions.end())
            {
                nextStates.insert(it->second.begin(), it->second.end());
            }
        }

        if (nextStates.empty())
            return false; // no transitions → dead end

        currentStates = std::move(nextStates);
    }

    // Accept if any current state is final
    for (const auto& state : currentStates)
    {
        if (m_finalStates.find(state) != m_finalStates.end())
            return true;
    }

    return false;
}
