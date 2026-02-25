#include "finiteAutomaton.h"
#include "grammar.h"
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

Grammar FiniteAutomaton::toGrammar() const
{
    std::set<Symbol> terminals{};
    std::set<Symbol> nonterminals{};
    Symbol start{m_initialState};
    std::vector<Production> productions{};

    for (const auto& state : m_states)
        if (state != "X")
            nonterminals.insert(state);
    
    for (const auto& letter : m_alphabet)
        terminals.insert(letter);
    
    for (const auto& p : m_transitions)
    {
        if (p.first.first == "X") continue; // skip transitions from X
        for(const auto& symbol : p.second)
        {
            if (symbol == "X") 
            {
                // If destination is X (accepting), only add terminal production
                Production temp{};
                temp.lhs.push_back(p.first.first);
                temp.rhs.push_back(p.first.second);
                productions.push_back(temp);
            }
            else 
            {
                Production temp{};
                temp.lhs.push_back(p.first.first);
                temp.rhs.push_back(p.first.second);
                // If destination is a final state, do not add the next state
                if (m_finalStates.find(symbol) == m_finalStates.end()) 
                {
                    temp.rhs.push_back(symbol);
                }
                productions.push_back(temp);
            }
        }
    }


    return Grammar (terminals, nonterminals, start, productions);
    
}
