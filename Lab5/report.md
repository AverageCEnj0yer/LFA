# Chomsky Normal Form Converter

## Project Overview

This project transforms any context-free grammar into Chomsky Normal Form (CNF). CNF is a simplified format where every production rule must be one of two things: a rule that makes exactly two nonterminals, or a rule that makes exactly one terminal. The start symbol can also make an empty string (epsilon) if the language includes one. Having our grammar in CNF is really useful for parsing algorithms, because the right-hand sides of the rules are very predictable.

## Architecture

The normalization function follows a pipeline architecture, executing a sequence of well-defined transformation steps. This rigid ordering is mathematically required to ensure that introducing new rules in one step does not violate the constraints resolved in a previous step. 

1. **Initial Setup (New Start Symbol):** A new start symbol is introduced to guarantee that the start symbol never appears on the right-hand side of any production.
2. **Epsilon Elimination:** Nullable nonterminals are identified, and all epsilon productions (rules deriving the empty string) are removed by compensating with new variant rules.
3. **Unit Production Elimination:** Rules of the form where a nonterminal derives exactly one nonterminal are collapsed using a graph-closure algorithm.
4. **Terminal Isolation:** Mixed right-hand sides containing both terminals and nonterminals are separated by introducing proxy nonterminals that strictly derive single terminals.
5. **Binarization:** Long production rules containing more than two nonterminals are broken down into a cascading sequence of binary rules.
6. **Cleanup (Optimization):** Non-productive symbols (which can never derive a string of terminals) and inaccessible symbols (which can never be reached from the start symbol) are purged from the grammar to keep it minimal.


## How the code works

The code goes through the grammar and makes changes step-by-step. The order is very important mathematically because changing things in the wrong order could mess up the work we did in a previous step. This is what the main function looks like:

```cpp
Grammar normalize(const Grammar& grammar)
{
    // ... copy the current grammar data ...
    
    // Step 1: create a new start symbol
    const Symbol newStart = makeFreshSymbol("S0", terminals, nonterminals);
    
    // Step 2: remove epsilon productions
    removeEpsilonProductions(productions, nonterminals, startSymbol);
    
    // Step 3: remove unit productions
    removeUnitProductions(productions, nonterminals, startSymbol);
    
    // Step 4: fix mixed right-hand sides
    isolateTerminalsInMixedRhs(productions, nonterminals, terminals);
    
    // Step 5: split up long right-hand sides
    binarizeLongProductions(productions, nonterminals, terminals);
    
    // Cleanup step
    removeNonProductiveSymbols(productions, nonterminals, terminals, startSymbol);
    removeInaccessibleSymbols(productions, nonterminals, terminals, startSymbol);
    
    return Grammar(terminals, nonterminals, startSymbol, productions);
}
```

Here's how each step works in more detail:

### Step 1: Adding a new start symbol

We add a new start symbol (like `S0`) that simply points to the original start symbol. We do this to make sure that the start symbol never shows up on the right-hand side of any rule.

```cpp
const Symbol newStart = makeFreshSymbol("S0", terminals, nonterminals);
nonterminals.insert(newStart);
productions.push_back(makeProduction(newStart, RHS{startSymbol}));
startSymbol = newStart;
```

### Step 2: Removing epsilon productions

First, we find "nullable" nonterminals (symbols that can turn into an empty string). Then we look at all the rules and see what happens if we remove the nullable symbols, creating new combinations.

```cpp
const auto variants = generateNullableVariants(production.rhs, nullable);
for (const auto& rhsVariant : variants)
{
    if (rhsVariant.empty() && production.lhs.front() != startSymbol)
        continue;
    transformed.insert(makeProduction(production.lhs.front(), rhsVariant));
}
```

### Step 3: Removing unit productions

A unit production is when a rule just has one nonterminal pointing to another (`A -> B`). We map out what symbols can reach other symbols. If `A` can reach `C`, and `C` has a rule like `C -> a`, we just give `A` that rule directly (`A -> a`) and throw away the `A -> B` rule.

```cpp
for (const auto& target : closure[nt])
{
    for (const auto& production : byLhs[target])
    {
        if (isUnitProduction(production, nonterminals))
            continue; // Skip the 'A -> B' rules
        transformed.insert(makeProduction(nt, production.rhs));
    }
}
```

### Step 4: Isolating terminals

CNF doesn't allow rules that mix terminals and nonterminals (like `A -> aB`) or have a single terminal string next to another terminal string (`a b`). So we go through and whenever we see a terminal in a long rule, we replace it with a new nonterminal (like `T0`) and then add a simple rule saying `T0 -> a`.

```cpp
const Symbol fresh = makeFreshSymbol("T", terminals, nonterminals);
nonterminals.insert(fresh);
terminalProxy[symbol] = fresh; // remember it for later
transformed.insert(makeProduction(fresh, RHS{symbol}));
symbol = fresh; // replace the terminal with T0 in the current rule
```

### Step 5: Binarizing long rules

Rules in CNF can only have flat pairs of nonterminals (like `A -> BC`). If we have a rule with three or more symbols (like `A -> BCD`), we break it down by creating new symbols (like `X0`) so it becomes `A -> BX0` and `X0 -> CD`.

```cpp
Symbol currentLhs = production.lhs.front();
for (std::size_t i = 0; i + 2 < production.rhs.size(); ++i)
{
    const Symbol fresh = makeFreshSymbol("X", terminals, nonterminals);
    transformed.insert(makeProduction(currentLhs, RHS{production.rhs[i], fresh}));
    currentLhs = fresh;
}
```

### Step 6: Cleanup

Finally, we remove any extra symbols that are useless. These could be non-productive symbols (they can never possibly become just terminals) or inaccessible symbols (they can never mathematically be reached starting from the start symbol). This clears up the clutter created from the earlier steps.

```cpp
for (const auto& production : productions)
{
    if (rhsValid) // only keep the rule if it produces something real
        filteredProductions.insert(production);
}
```

## Testing and Validation

To make sure the normalization logic works perfectly, we tested it against four different, complex grammar variants. We wrote a strict helper function called `isChomskyNormalForm` that goes through all the rules of a grammar and checks if they strictly follow CNF (only pairs of nonterminals, single terminals, or an empty string for the start symbol). 

For each of the four test cases, we load the grammar, prove that it starts out failing the CNF test, run our `normalize` function, and then assert that the resulting grammar fully passes the CNF test. 

```cpp
Grammar g(terminals, nonterminals, start, productions);
assert(!isChomskyNormalForm(g)); // Fails before

Grammar gn = normalize(g);
assert(isChomskyNormalForm(gn)); // Passes after
std::cout << "Variant CNF normalization: OK\n";
```

All four test variants passed successfully, proving that our code properly handles tricky edge cases like removing empty strings, cleaning up long rules, and breaking endless loops of unit rules.

## Difficulties Encountered

There were a few challenges when writing this logic:

1. Getting the order of the steps right was hard. Doing epsilon removal before unit elimination might create new unit rules that get left behind, so the sequence of operations is very important.
2. Generating completely fresh symbols that don't conflict. I had to write a helper function to keep checking if a suggested symbol (like `S0`) was already being used anywhere in the grammar, which can easily cause subtle bugs.
3. Epsilon removal is tricky because if `A -> BCD` has multiple nullable symbols, you end up with many different versions to insert back into the grammar. Using recursion made it work, but making sure doing this doesn't recreate bad empty strings (except for the start symbol) required attention to detail.
4. Figuring out loop cycles for unit productions. Sometimes `A -> B` and `B -> A`. Doing a simple search would loop forever, so I had to keep track of a "closure" set of things we've already seen to know when to stop.

## Future Work

Right now, the code only converts the grammar rules into Chomsky Normal Form. A great next step is to write a Cocke-Younger-Kasami (CYK) parser. 

Since the grammar is completely in CNF, checking if a word belongs to the grammar's language using a CYK algorithm is very easy. The CYK algorithm takes a CNF grammar and looks at a word, using dynamic programming to scan it out. By adding this, the program can go from just editing the rules to actually checking syntax!

## Conclusion

This project successfully converts any context-free grammar into a clean Chomsky Normal Form. By carefully dividing the math into simple step-by-step logic, we handle complicated things like empty strings, endless loops of loops, and overly long strings. Keeping everything organized in a clear list of steps makes the code easier to understand and guarantees it is working correctly.
