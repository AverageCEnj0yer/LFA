# Laboratory 2: Finite Automata and Grammar Conversion

**Course:** Formal Languages & Finite Automata  
**Author:** Gabriel Balan

---

## Theory

This laboratory explores the relationship between formal grammars and finite automata, focusing on the conversion between different representations and the classification of grammars. The implementation required adapting the grammar structure to support a vector of symbols on the left-hand side (LHS) of productions, enabling the handling of any type of grammar, not just regular ones.

---

## Objectives

- Implement grammar classification according to Chomsky hierarchy.
- Convert grammars to finite automata and vice versa.
- Check if a finite automaton is deterministic.
- Convert any NFA (including epsilon-NFA) to a DFA using subset construction.

---

## Implementation Description

### 1. `void classifyGrammar() const;`

This function analyzes the productions of the grammar and determines its type according to the Chomsky hierarchy. It iterates through all productions, checking the structure of the left-hand side (LHS) and right-hand side (RHS) to identify regular, context-free, or context-sensitive rules. The most difficult part was handling edge cases:
- Productions with multiple nonterminals on the LHS (e.g., S → aA or S → AB).
- Epsilon productions (empty RHS).
- Mixed left/right linear rules (e.g., S → aA and S → Ab).
- Context-sensitive rules (e.g., AB → aB).
- Ambiguities between context-free and context-sensitive.

**Relevant snippets:**

_Checking if start symbol appears on RHS (for context-sensitive epsilon):_
```cpp
for (const auto& p : m_productions) {
    if (std::find(p.rhs.begin(), p.rhs.end(), m_start) != p.rhs.end()) {
        startAppearsOnRHS = true;
        break;
    }
}
```

_Regular grammar check: strict left/right linearity and terminal-only productions:_
```cpp
for (const auto& p : m_productions) {
    if (p.lhs.size() != 1 || m_nonterminals.find(p.lhs[0]) == m_nonterminals.end()) {
        isRegular = false;
        break;
    }
    if (p.rhs.size() == 2) {
        // Right-linear: A -> aB
        if (m_terminals.find(p.rhs[0]) != m_terminals.end() && m_nonterminals.find(p.rhs[1]) != m_nonterminals.end()) {
            foundRightLinear = true;
        // Left-linear: A -> Ba
        } else if (m_nonterminals.find(p.rhs[0]) != m_nonterminals.end() && m_terminals.find(p.rhs[1]) != m_terminals.end()) {
            foundLeftLinear = true;
        } else {
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
    if(foundLeftLinear && foundRightLinear) {
        isRegular = false;
        break;
    }
}
```

_Context-free grammar check: LHS must be a single nonterminal:_
```cpp
for (const auto& p : m_productions) {
    if (p.lhs.size() != 1 || m_nonterminals.find(p.lhs[0]) == m_nonterminals.end()) {
        isContextFree = false;
        break;
    }
}
```

_Context-sensitive grammar check: LHS must have at least one nonterminal, |RHS| >= |LHS| (except S → ε):_
```cpp
for (const auto& p : m_productions) {
    bool lhsHasNonTerminal = false;
    for (const auto& symbol : p.lhs) {
        if (m_nonterminals.find(symbol) != m_nonterminals.end()) {
            lhsHasNonTerminal = true;
            break;
        }
    }
    if (p.rhs.empty()) {
        if (!(p.lhs.size() == 1 && p.lhs[0] == m_start)) {
            isContextSensitive = false;
            break;
        }
        if (startAppearsOnRHS) {
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
```

### 2. `Grammar toGrammar() const;`

This function converts a finite automaton to an equivalent grammar. It loops through all states and transitions, creating productions for each transition. The main challenge was ensuring that the artificial state 'X' (added for accepting states) was properly removed, and handling epsilon transitions correctly. For each transition, if the destination is 'X', only the terminal is added to the RHS; otherwise, both terminal and next state are included.

**Relevant snippet:**
```cpp
for (const auto& p : m_transitions) {
    if (p.first.first == "X") continue; // skip transitions from X
    for(const auto& symbol : p.second) {
        if (symbol == "X") {
            Production temp{};
            temp.lhs.push_back(p.first.first);
            temp.rhs.push_back(p.first.second);
            productions.push_back(temp);
        } else {
            Production temp{};
            temp.lhs.push_back(p.first.first);
            temp.rhs.push_back(p.first.second);
            if (m_finalStates.find(symbol) == m_finalStates.end()) {
                temp.rhs.push_back(symbol);
            }
            productions.push_back(temp);
        }
    }
}
```

### 3. `bool isDeterministic() const;`

This function checks if the automaton is deterministic by iterating through the transition table and verifying that each input symbol leads to at most one destination state. Thanks to the clear structure of the transition table, this was straightforward.

**Relevant snippet:**
```cpp
for(const auto& t : m_transitions) {
    if (t.second.size() > 1)
        return false;
}
return true;
```

### 4. `FiniteAutomaton toDFA() const;`

This function implements the subset construction algorithm to convert any NFA (including epsilon-NFA) to a DFA. It starts by computing the epsilon closure of the initial state, then iteratively explores all possible transitions for each set of states, assigning unique names to new DFA states. The process continues until all reachable state sets are processed. Difficulties included:
- Managing sets of states as new DFA states.
- Computing epsilon closures efficiently.
- Ensuring all possible transitions are explored.
- Assigning unique names to DFA states.
- Handling edge cases with unreachable or dead states.

**Relevant snippet:**
```cpp
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

// Subset construction loop
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
        nextSet = epsilonClosure(nextSet);
        if (nextSet.empty()) continue;
        if (!stateSetToName.count(nextSet)) {
            Symbol name = "Q" + std::to_string(stateCount++);
            stateSetToName[nextSet] = name;
            dfaStates.push_back(nextSet);
            dfaStateNames.insert(name);
            if (std::any_of(nextSet.begin(), nextSet.end(), [&](const Symbol& s){ return m_finalStates.count(s); }))
                dfaFinalStates.insert(name);
        }
        dfaTransitions[{dfaStateName, input}].insert(stateSetToName[nextSet]);
    }
}
```

---

## Results / Screenshots / Conclusions

- The grammar classification function correctly identifies grammar types, even in edge cases.
- Conversion between grammar and automaton works for both regular and more complex grammars.
- Determinism check is reliable and efficient.
- NFA to DFA conversion is robust, handling epsilon transitions and complex state sets.

**Sample Output:**
```
DFA states and transitions (converted):
States: { Q0 Q1 Q2 Q3 }
Alphabet: { a b }
Initial state: Q0
Final states: { Q2 Q3 }
Transitions:
  Q0 --a--> Q1
  Q0 --b--> Q0
  Q1 --a--> Q1
  Q1 --b--> Q2
  Q2 --a--> Q3
  Q2 --b--> Q2
  Q3 --a--> Q3
  Q3 --b--> Q2
```

---

## Difficulties Encountered

- Adapting the grammar structure for generality.
- Handling edge cases in classification.
- Managing artificial states and epsilon transitions in conversion.
- Implementing subset construction for DFA conversion.

---

## Conclusion

The implementation successfully bridges the gap between grammars and automata, providing robust tools for classification, conversion, and analysis. The most challenging aspect was the DFA conversion, which required careful management of state sets and transitions.
