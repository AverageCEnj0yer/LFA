nanananabatman


# Custom Regular Expression Engine for String Generation

## Project Overview

This project implements a custom regular expression engine that, instead of matching or validating strings, generates random strings that satisfy a user-provided regex-like pattern. The system is particularly useful for generating test data, fuzzing, or educational demonstrations where producing valid examples is more important than checking them.

## Architecture

The engine is structured in three main stages, inspired by the classic compiler front-end model:

1. **Lexical Analysis (Lexer):** The lexer scans the input pattern and breaks it into a stream of tokens, each with a type and a string value.
2. **Parsing (Recursive Descent Parser):** The parser processes the token stream and builds an abstract syntax tree (AST) that represents the hierarchical structure of the pattern.
3. **String Generation (Polymorphic Node System):** The AST is traversed by a system of polymorphic nodes, each responsible for generating its part of the output string, resulting in a random string that matches the pattern.

## Lexical Analysis and Token Stream

The lexer converts the raw regex string into structured tokens. Each token contains a type and a lexeme. A key feature is the handling of escape sequences: whenever a `/` character is encountered, the next character is always treated as a literal, even if it would normally be interpreted as an operator. For example, `/+` in the pattern will match a literal plus sign, not the repetition operator. This allows users to include reserved symbols such as `*`, `+`, `|`, and others in their patterns without ambiguity.

**Example of escape handling in the lexer:**

```cpp
if (m_currentChar == '/') {
	if (m_position + 1 < m_regex.length()) {
		advance(); // Skip the '/'
		return literal(); // Returns TokenType::LITERAL even if char is '+'
	}
}
```

## Parsing Logic

The parser uses a recursive descent strategy with clear operator precedence. There are three main parsing functions:

1. `parseExpression` handles alternation (the `|` operator) and has the lowest precedence.
2. `parseSequence` handles concatenation and has medium precedence.
3. `parseFactor` handles quantifiers like `*`, `+`, `?`, and `^`, and has the highest precedence.

This ensures that patterns are interpreted according to standard regex rules. For example, the pattern `a|bc*` is parsed as either `a` or `b` followed by zero or more `c` characters.

**Example implementation of sequence parsing:**

```cpp
Node* Parser::parseSequence() {
	Node* node = parseFactor();
	SequenceNode* seq = nullptr;

	while (m_currentToken.type == TokenType::LPAREN || 
		   m_currentToken.type == TokenType::LITERAL) {
		if (!seq) {
			seq = new SequenceNode();
			seq->addChild(node);
		}
		seq->addChild(parseFactor());
	}

	return seq ? seq : node;
}
```

## Polymorphic AST and Memory Management

The AST is built from a hierarchy of node classes. At the base is the `Node` class, with derived classes such as `LiteralNode`, `SequenceNode`, `RepeatNode`, and others. Each node implements a `generate` method that produces a string according to its semantics. Memory management is handled recursively: deleting the root node of the AST automatically cleans up the entire tree, as destructors are implemented to delete child nodes.

## Random Generation and Quantifiers

Randomness in string generation is provided by the standard `mt19937` random number generator. Quantifiers such as `*` (zero or more) and `+` (one or more) are capped to prevent excessive output. If a quantifier is unbounded (max = -1), the system limits the number of repetitions to a range between zero and five. This ensures that generated strings are of reasonable length and that the program remains performant and stable.

## What the Program Recognizes

The engine supports a wide range of regex constructs:

1. Any character as a literal.
2. Parentheses to override normal precedence rules.
3. The `+` operator for one or more repetitions.
4. The `*` operator for zero or more repetitions.
5. The `?` operator for zero or one occurrence.
6. The `^` operator to specify an exact number of repetitions for a sub-pattern (e.g., `(ab)^3` for exactly three repetitions of `ab`).
7. Escape sequences using `/` allow the user to include operator characters as literals in the pattern (e.g., `/+` matches a literal plus sign).

This comprehensive feature set makes the engine flexible and expressive for a variety of string generation tasks.

**Example:**

Input pattern:

```
(na)^4batman
```

AST structure:

```
Sequence:
  Repeat (4 to 4):
	Sequence:
	  Literal: 'n'
	  Literal: 'a'
  Literal: 'b'
  Literal: 'a'
  Literal: 't'
  Literal: 'm'
  Literal: 'a'
  Literal: 'n'
```

Generated output:

```
nanananabatman
```

## Extensibility

The design of the system emphasizes extensibility and separation of concerns. For example, to add support for character ranges like `[a-z]`, one would only need to implement a new `RangeNode` class and add handling for it in the parser's base parsing function. The lexer, sequence handling, and quantifier logic would not require modification, demonstrating the modularity of the architecture.

## Difficulties Encountered

Several significant difficulties were encountered during development:

1. Understanding and implementing the recursive descent logic of the parser was extremely challenging, especially in ensuring correct handling of operator precedence and associativity.
2. Designing a robust abstraction for the various node types and using polymorphism effectively was another major hurdle, as it was essential to keep the codebase maintainable and extensible.
3. The sheer number of edge cases arising from the interplay between the lexer and parser was daunting, and handling all of them correctly demanded meticulous attention to detail.
4. Finally, a subtle bug in the `printTokenType` function led to incorrect visual output, even though the actual generated strings were correct. This discrepancy resulted in many hours of frustrating debugging before the true cause was discovered and resolved.


## Future Work

While the current implementation stops at the abstract syntax tree (AST) and focuses on string generation, there is a clear path forward for extending the engine into a full-featured regular expression matcher. By introducing a logic layer that transforms the AST into a nondeterministic finite automaton (NFA), the system could be adapted to recognize and validate input strings against the specified patterns, not just generate them. This would involve traversing the AST and constructing the corresponding NFA states and transitions for each node type. Once the NFA is built, standard algorithms could be used to check whether a given string matches the pattern. With this extension, the project would become a complete regex engine, capable of both generating and recognizing strings according to user-defined patterns.

## Summary

This project demonstrates a clean, compiler-inspired architecture for regex-based string generation. It features strong separation of concerns, a polymorphic and extensible design, safe recursive memory management, and controlled randomness for output generation. The engine is capable of recognizing and generating strings for a wide variety of regex-like patterns, including those with complex quantifiers and escape sequences, making it a powerful tool for anyone needing to generate structured random strings.
