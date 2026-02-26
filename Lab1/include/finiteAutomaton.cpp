#include "finiteAutomaton.h"
#include "grammar.h"
#include <algorithm>
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
bool FiniteAutomaton::isDeterministic() const
{
    for(const auto& t : m_transitions)
        if (t.second.size() > 1)
            return false;
    return true;
}
FiniteAutomaton FiniteAutomaton::toDFA() const
{
    // Subset construction algorithm (handles epsilon-NFA, NFA → DFA)
    using StateSet = std::set<Symbol>;
    std::map<StateSet, Symbol> stateSetToName;
    std::vector<StateSet> dfaStates;
    std::set<Symbol> dfaStateNames;
    std::set<Symbol> dfaFinalStates;
    std::map<std::pair<Symbol, Symbol>, std::set<Symbol>> dfaTransitions;

    // Helper: compute epsilon closure of a set of states
    auto epsilonClosure = [&](const StateSet& states) {
        StateSet closure = states;
        std::vector<Symbol> stack(closure.begin(), closure.end());
        while (!stack.empty()) {
            Symbol s = stack.back(); stack.pop_back();
            auto it = m_transitions.find({s, ""}); // "" is epsilon
            if (it != m_transitions.end()) {
                for (const auto& next : it->second) {
                    if (closure.insert(next).second)
                        stack.push_back(next);
                }
            }
        }
        return closure;
    };

    // Start with epsilon closure of initial state
    StateSet startClosure = epsilonClosure({m_initialState});
    dfaStates.push_back(startClosure);
    stateSetToName[startClosure] = "Q0";
    dfaStateNames.insert("Q0");
    if (std::any_of(startClosure.begin(), startClosure.end(), [&](const Symbol& s){ return m_finalStates.count(s); }))
        dfaFinalStates.insert("Q0");

    size_t stateCount = 1;
    for (size_t i = 0; i < dfaStates.size(); ++i) {
        Symbol dfaStateName = stateSetToName[dfaStates[i]];
        for (const auto& input : m_alphabet) {
            if (input.empty()) continue; // skip epsilon
            StateSet nextSet;
            for (const auto& s : dfaStates[i]) {
                auto it = m_transitions.find({s, input});
                if (it != m_transitions.end()) {
                    nextSet.insert(it->second.begin(), it->second.end());
                }
            }
            // Take epsilon closure of the result
            nextSet = epsilonClosure(nextSet);
            if (nextSet.empty()) continue;
            // Assign a name if not already present
            if (!stateSetToName.count(nextSet)) {
                Symbol name = "Q" + std::to_string(stateCount++);
                stateSetToName[nextSet] = name;
                dfaStates.push_back(nextSet);
                dfaStateNames.insert(name);
                if (std::any_of(nextSet.begin(), nextSet.end(), [&](const Symbol& s){ return m_finalStates.count(s); }))
                    dfaFinalStates.insert(name);
            }
            // Add transition
            dfaTransitions[{dfaStateName, input}].insert(stateSetToName[nextSet]);
        }
    }

    // DFA only: transitions are single-valued
    std::map<std::pair<Symbol, Symbol>, std::set<Symbol>> dfaTransitionsSingle;
    for (const auto& t : dfaTransitions) {
        dfaTransitionsSingle[{t.first.first, t.first.second}].insert(*t.second.begin());
    }

    return FiniteAutomaton(dfaStateNames, m_alphabet, dfaTransitionsSingle, "Q0", dfaFinalStates);
}
