# Formal Languages & Finite Automata - Variant 1

## Overview

This project implements a solution for Variant 1 of the Formal Languages & Finite Automata course assignment. It demonstrates the concepts of regular grammars and finite automata by providing classes to represent and manipulate these structures.

## Algorithm and Implementation (for Professors)

### Structure
- Two main classes: **Grammar** and **FiniteAutomaton**.
- The main program (main.cpp) demonstrates their use.

### Grammar Class
- **Encapsulates** a right-linear grammar (Variant 1).
- **Initialization:** Sets of terminals, non-terminals, start symbol, and production rules.
- **Key Methods:**
  - `generateWord()`: Randomly generates a valid string by simulating derivations from the grammar.
  - `toFiniteAutomaton()`: Converts the grammar to an equivalent finite automaton (NFA), following standard algorithms.

### FiniteAutomaton Class
- **Represents** a finite automaton (NFA) derived from the grammar.
- **Initialization:** Sets of states, alphabet, transition map, initial state, and final states.
- **Key Methods:**
  - `stringBelongsToLanguage(input)`: Simulates the automaton on the input string, returning true if accepted, false otherwise.

### Main Program Logic
1. **Setup:** Defines the grammar (Variant 1) and its productions. Instantiates the Grammar class.
2. **Demonstration:** Prints the grammar and the automaton. Generates valid words. Converts the grammar to a finite automaton and prints its structure. Tests string acceptance.

### How the Code Works (Algorithmic Flow)
- **Grammar to Automaton Conversion:**
  - Each non-terminal becomes a state.
  - Productions of the form `A → aB` become transitions from state A to B on input 'a'.
  - Productions of the form `A → a` become transitions from state A to a special final state on input 'a'.
- **Word Generation:**
  - Starts from the initial symbol.
  - Randomly selects applicable productions, accumulating terminal symbols, until reaching a terminal-only production.
- **Automaton Simulation:**
  - Starts at the initial state.
  - For each input symbol, follows all possible transitions.
  - Accepts if any final state is reached after processing the input.

### C++ Implementation Notes
- Uses standard containers (set, vector, map) for clarity and efficiency.
- Random word generation uses C++'s random library.
- Automaton is implemented as an NFA, but test cases are deterministic for clarity.
- Main logic is encapsulated in classes, with clear separation between grammar and automaton.

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

## Input/Output Values

- **Input Grammar**: Hardcoded in `main.cpp` according to Variant 1 specifications.
- **Generated Strings**: Random valid strings from the language (e.g., "ae", "abcdea", "ba").
- **Test Strings**: Various strings tested for acceptance, including valid ones (accepted) and invalid ones (rejected).
- **Output**: Console prints of grammar, automaton, generated words, and test results.

## Dependencies

- C++ standard library (included via headers like `<iostream>`, `<set>`, `<vector>`, `<map>`, `<random>`).
- No external libraries required.

## Building and Running

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

---

For further details, see the comments in the code or ask for specific algorithmic explanations.
