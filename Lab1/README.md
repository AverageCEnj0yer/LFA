# Formal Languages & Finite Automata - Variant 1

## Overview

This project implements a solution for Variant 1 of the Formal Languages & Finite Automata course assignment. It demonstrates the concepts of regular grammars and finite automata by providing classes to represent and manipulate these structures.

## Problem Solved

The code addresses the following requirements for Variant 1:

- **Grammar Definition**: Implements a class for a regular grammar with the given productions.
- **String Generation**: Generates valid strings from the language defined by the grammar.
- **Grammar to Finite Automaton Conversion**: Converts the regular grammar to an equivalent finite automaton.
- **String Acceptance Check**: Determines if a given string belongs to the language recognized by the finite automaton.

## Grammar Specification (Variant 1)

- **Non-terminals (VN)**: {S, P, Q}
- **Terminals (VT)**: {a, b, c, d, e, f}
- **Start Symbol**: S
- **Productions (P)**:
  - S → aP
  - S → bQ
  - P → bP
  - P → cP
  - P → dQ
  - P → e
  - Q → eQ
  - Q → fQ
  - Q → a

## Implementation Details

### Grammar Class

- **Constructor**: Initializes the grammar with terminals, non-terminals, start symbol, and production rules.
- **print()**: Displays the grammar components (terminals, non-terminals, start symbol, productions).
- **toFiniteAutomaton()**: Converts the regular grammar to a finite automaton using the standard algorithm for right-linear grammars. The conversion is cached for efficiency.
- **generateWord()**: Generates a random valid string by simulating a walk through the finite automaton until reaching an accepting state.

### FiniteAutomaton Class

- **Constructor**: Initializes the automaton with states, alphabet, transition function, initial state, and final states.
- **print()**: Displays the automaton components (states, alphabet, initial state, final states, transitions).
- **stringBelongsToLanguage()**: Simulates the automaton on the input string to determine acceptance.

### Main Program

The `main.cpp` file demonstrates the functionality by:
1. Creating the grammar instance with the specified components.
2. Printing the grammar.
3. Converting to and printing the finite automaton.
4. Generating 5 random valid strings.
5. Testing string acceptance with assertions for both accepted and rejected strings.

## How It Works

1. **Grammar Representation**: The grammar is stored as sets of terminals and non-terminals, a start symbol, and a list of production rules.

2. **Conversion to FA**: The `toFiniteAutomaton()` method treats the grammar as right-linear and creates an NFA where:
   - States correspond to non-terminals plus an extra accepting state "X".
   - Transitions are built based on production rules.
   - Terminal-only productions lead to the accepting state.
   - Right-linear productions (terminal + non-terminal) create transitions to the next non-terminal.

3. **String Generation**: `generateWord()` starts from the initial state and randomly follows transitions, accumulating terminals until reaching a final state.

4. **Acceptance Check**: `stringBelongsToLanguage()` processes the input character by character, following transitions and checking if any current state is accepting at the end.

## Input/Output Values

- **Input Grammar**: Hardcoded in `main.cpp` according to Variant 1 specifications.
- **Generated Strings**: Random valid strings from the language (e.g., "ae", "abcdea", "ba").
- **Test Strings**: Various strings tested for acceptance, including valid ones (accepted) and invalid ones (rejected).
- **Output**: Console prints of grammar, automaton, generated words, and test results.

## Dependencies

- C++ standard library (included via headers like `<iostream>`, `<set>`, `<vector>`, `<map>`, `<random>`).
- No external libraries required.

## Building and Running

g++ main.cpp include/grammar.cpp include/finiteAutomaton.cpp -o lfa_project

### Using CMake (Recommended, Cross-Platform)

1. Create a build directory and navigate into it:
  ```
  mkdir build
  cd build
  ```
2. Generate build files with CMake:
  ```
  cmake ..
  ```
3. Build the project:
  ```
  cmake --build .
  ```
  Or, if you are on Linux/macOS and Makefiles are generated, you can use:
  ```
  make
  ```
4. Run the program:
  ```
  ./test
  ```
  Or, if you want to build and run in one step (with the custom target):
  ```
  cmake --build . --target run
  # or
  make run
  ```

#### On Windows
- If using Visual Studio, open the generated `.sln` file and build/run from the IDE, or use:
  ```
  cmake --build . --config Release
  cmake --build . --target run --config Release
  ```
- If using MinGW, use `mingw32-make` instead of `make`.

## Notes

- The implementation assumes a right-linear regular grammar for the conversion algorithm.
- String generation uses random selection for non-deterministic choices.
- The automaton is represented as an NFA, with transitions mapping (state, input) to a set of next states.
- Assertions in `main.cpp` verify correctness with known valid and invalid strings.
