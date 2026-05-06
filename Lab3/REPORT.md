# Lexer Laboratory Report (Key Points)

## Supported Words and Tokens

- Keywords: `int`, `float`, `string`, `bool`, `true`, `false`, `return`, `AND`, `OR`, `NOT`
- Types: `INT_TYPE`, `FLOAT_TYPE`, `STRING_TYPE`, `BOOL_TYPE`
- Operators: `+`, `-`, `*`, `/`, `^`, `=`, `==`, `<`, `<=`, `>`, `>=`, `!=`
- Delimiters: `(`, `)`, `,`, `;`, `{`, `}`
- Identifiers: variable/function names (letters, digits, underscores)
- Literals: integer, float, string
- Unknown: any unrecognized character or malformed token

## Example: TokenType Enum
```cpp
enum class TokenType {
    UNKNOWN, TRUE, FALSE, AND, OR, NOT, RETURN,
    INT, FLOAT, STRING, BOOLEAN, IDENTIFIER,
    PLUS, MINUS, MULTIPLY, DIVIDE, EXPONENT, ASSIGN,
    LESS, GREATER, LESSEQ, GREATEREQ, EQ, NOTEQ,
    LPAREN, RPAREN, COMMA, SEMICOLON, RCBRACKET, LCBRACKET,
    EOFILE, INT_TYPE, FLOAT_TYPE, STRING_TYPE, BOOL_TYPE
};
```
## How the Program Works

The core of the program is the `Lexer` class, which manages the process of tokenizing input. The class reads the entire file into a string for efficient access. Its main methods include:
- `advance()`: Moves to the next character, updating the current state.
- `skipWhitespace()`: Skips over any whitespace characters.
- `getNextToken()`: Extracts the next token from the input, deciding which parsing method to use based on the current character.
- `number()`, `identifierOrKeyword()`, `stringLiteral()`, `unknownToken()`: Specialized methods for parsing numbers, identifiers/keywords, string literals, and unknown tokens, respectively.

The flow starts by initializing the lexer with the file content. It repeatedly calls `getNextToken()` to extract tokens until the end of the file is reached. Each token is classified and returned, allowing the program to build a stream of tokens representing the input source.

## Initial Problems and Logical Errors

Early versions of the lexer suffered from several logical flaws:

- **Number/Identifier Confusion:** Using `isalnum` for number detection meant identifiers starting with letters were misclassified as numbers. The fix was to use `isdigit` for numbers and `isalpha` for identifiers, ensuring correct tokenization.

- **Position Reset in Identifier Parsing:** Resetting the position after parsing an identifier caused infinite loops and incorrect token extraction. Removing this reset allowed identifiers to be processed correctly.

- **Operator Handling:** The logic for distinguishing `=` and `==` (and similar operators) was reversed, leading to wrong token types. The improved version advances the character pointer before checking for double operators, ensuring proper token assignment.

- **Unknown Token Consumption:** The initial approach consumed multiple characters for unknown tokens, which could swallow valid tokens. The fix was to process unknown tokens one character at a time, improving error handling and clarity.

Each of these issues was addressed with targeted changes, resulting in a lexer that reliably distinguishes between numbers, identifiers, operators, and unknown tokens.

## Improved Version Highlights
- Reads the whole file into a string for speed.
- Handles edge cases: `193` is a number, `193DAWJF` is unknown.
- Supports flexible whitespace: `int x=4` and `int x = 4` both work.
- Robust operator and delimiter handling.
- Unknown tokens are processed one character at a time.

## Example: Robust Number Handling
```cpp
Token Lexer::number() {
    size_t startIDX = m_position;
    bool isFloat = false;
    while (std::isdigit(m_currentChar) || m_currentChar == '.') {
        if (m_currentChar == '.') {
            if (isFloat) {
                // Second dot: malformed float
                while (!isAtEnd() && !std::isspace(m_currentChar))
                    advance();
                std::string value = m_fileContent.substr(startIDX, m_position - startIDX);
                return Token{TokenType::UNKNOWN, value};
            }
            isFloat = true;
        }
        advance();
    }
    if (std::isalpha(m_currentChar)) {
        while (!isAtEnd() && !std::isspace(m_currentChar))
            advance();
        std::string value = m_fileContent.substr(startIDX, m_position - startIDX);
        return Token{TokenType::UNKNOWN, value};
    }
    std::string value = m_fileContent.substr(startIDX, m_position - startIDX);
    if (isFloat)
        return Token{TokenType::FLOAT, value};
    return Token{TokenType::INT, value};
}
```

## Difficulties Encountered

### End-of-File Handling

Early versions crashed when reaching the end of the file, leaving the lexer in a bad state:
```cpp
// Problem: No check for EOF, could access out of bounds
m_currentChar = m_fileContent[m_position]; // unsafe if file is empty or at end
```
**Fix:**
```cpp
if (!m_fileContent.empty())
    m_currentChar = m_fileContent[0];
else
    m_currentChar = '\0'; // safe EOF handling
```

### Empty File Handling

Trying to access characters in an empty file caused immediate crashes:
```cpp
// Problem: Accessing index 0 in empty file
m_currentChar = m_fileContent[0]; // crash if file is empty
```
**Fix:**
```cpp
if (!m_fileContent.empty())
    m_currentChar = m_fileContent[0];
else
    m_currentChar = '\0';
```

### Index Management

Keeping index access correct was tricky, especially when advancing or reading tokens:
```cpp
// Problem: Advance logic could leave index out of bounds
if (m_position < m_fileContent.length() - 1)
    m_currentChar = m_fileContent[++m_position];
```
**Fix:**
```cpp
m_position++;
if (m_position < m_fileContent.size())
    m_currentChar = m_fileContent[m_position];
else
    m_currentChar = '\0';
```

### Function Invariants and Integration

It was difficult to design functions so they always left the lexer in a valid state and worked together correctly. For example, skipping whitespace, advancing, and token extraction had to be carefully coordinated to avoid infinite loops or missed tokens. This required careful reasoning about invariants and state transitions throughout the lexer class.

## Difficulties
- Early version was slow and buggy due to character-by-character file reading.
- Many logical bugs: wrong token types, infinite loops, unsafe memory access.
- Improved by reading the file into RAM, fixing parsing logic, and adding missing tokens.

## Example Test and Output

**Test Input:**
```plaintext
int a = 5;
int b = 3;
bool c = a AND b;
```

**Lexer Output:**
```
INT_TYPE
IDENTIFIER(a)
ASSIGN
INT(5)
SEMICOLON
INT_TYPE
IDENTIFIER(b)
ASSIGN
INT(3)
SEMICOLON
BOOL_TYPE
IDENTIFIER(c)
ASSIGN
IDENTIFIER(a)
AND
IDENTIFIER(b)
SEMICOLON
EOFILE
```

## Conclusion

The lexer now reliably tokenizes variable declarations, assignments, logical expressions, and handles malformed input. Edge cases like `193` vs `193abc` are correctly distinguished. The code is much cleaner and safer after fixing logic bugs and improving file reading. Next steps could include adding support for more keywords, handling comments, and integrating the lexer with a parser for full language processing. Further improvements might focus on error reporting and extensibility for new token types.




---

## Chapter 2: Parser & Building an Abstract Syntax Tree

### Overview

With a working lexer in place, the natural next step in building a language processor is **parsing**, the process of taking the flat stream of tokens produced by the lexer and giving it hierarchical, grammatical meaning. Where the lexer answers *"what are these tokens?"*, the parser answers *"what do they mean together?"*.

The result of parsing is an **Abstract Syntax Tree (AST)**: a tree data structure where each node represents a construct in the source code. For example, the expression `x + 1` is not just three tokens, it is an addition operation with two operands, and the AST captures that relationship explicitly. The "abstract" part means irrelevant syntactic details (semicolons, parentheses used only for grouping) are discarded, leaving only the meaningful structure.

Two entirely new files were added: `ast.h` and `parser.h`/`parser.cpp` and the existing `token.h` and `lexer.cpp` received small targeted additions.

---

### What Was Added to the Existing Files

Before the parser could be written, one new keyword needed to be recognized by the lexer: `void`, for functions that return nothing.

**In `token.h`**, a new enum value was appended:

```cpp
VOID_TYPE = -35,
```

And the corresponding string conversion case:

```cpp
case TokenType::VOID_TYPE: return "VOID_TYPE";
```

**In `lexer.cpp`**, one line was added to `identifierOrKeyword()`, alongside the other type keywords:

```cpp
if (value == "void") return Token{TokenType::VOID_TYPE, value};
```

That is the full extent of what changed in the lexer, it was deliberately designed to be non-invasive. The lexer still does exactly what it always did; the parser sits entirely on top of it.

---

### The AST Node Hierarchy (`ast.h`)

All AST nodes share a common base class:

```cpp
struct ASTNode
{
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
protected:
    void printIndent(int indent) const {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
    }
};

using ASTNodePtr = std::unique_ptr<ASTNode>;
```

The use of `std::unique_ptr` is deliberate. It gives each node clear ownership of its children with no manual memory management required. The tree tears itself down automatically when the base goes out of scope.

From this base, **fourteen concrete node types** were implemented, organized into three categories:

**Literal nodes** — leaf nodes that hold actual values:

```cpp
struct IntLiteralNode    : ASTNode { int         value; ... };
struct FloatLiteralNode  : ASTNode { float       value; ... };
struct StringLiteralNode : ASTNode { std::string value; ... };
struct BoolLiteralNode   : ASTNode { bool        value; ... };
struct IdentifierNode    : ASTNode { std::string name;  ... };
```

**Expression nodes** — interior nodes that combine sub-expressions:

```cpp
struct BinaryOpNode : ASTNode {
    std::string op;
    ASTNodePtr  left;
    ASTNodePtr  right;
    ...
};

struct UnaryOpNode : ASTNode {
    std::string op;
    ASTNodePtr  operand;
    ...
};

struct FunctionCallNode : ASTNode {
    std::string             name;
    std::vector<ASTNodePtr> args;
    ...
};
```

**Statement nodes** — nodes that represent full instructions:

```cpp
struct AssignNode    : ASTNode { std::string name; ASTNodePtr value; ... };
struct VarDeclNode   : ASTNode { std::string typeName, varName; ASTNodePtr initializer; ... };
struct ReturnNode    : ASTNode { ASTNodePtr value; ... };
struct ExprStmtNode  : ASTNode { ASTNodePtr expr; ... };
struct BlockNode     : ASTNode { std::vector<ASTNodePtr> statements; ... };
```

And the two nodes added specifically for function support:

```cpp
struct ParameterNode : ASTNode {
    std::string typeName;
    std::string paramName;
    ...
};

struct FunctionDeclNode : ASTNode {
    std::string                                  returnType;
    std::string                                  name;
    std::vector<std::unique_ptr<ParameterNode>>  params;
    ASTNodePtr                                   body;
    ...
};
```

Finally, the root of every parsed program:

```cpp
struct ProgramNode : ASTNode {
    std::vector<ASTNodePtr> statements;
    ...
};
```

---

### The Grammar

The parser implements the following grammar using **recursive descent**: one function per grammar rule, calling each other in a chain that naturally encodes operator precedence. Higher in the list means lower precedence (evaluated last); lower in the list means higher precedence (evaluated first):

```
program     → (funcDecl | statement)* EOF
statement   → varDecl | returnStmt | block | exprStmt
funcDecl    → TYPE IDENTIFIER "(" paramList ")" block
varDecl     → TYPE IDENTIFIER ("=" expression)? ";"
returnStmt  → "return" expression? ";"
block       → "{" statement* "}"
exprStmt    → expression ";"

expression  → assignment
assignment  → IDENTIFIER "=" expression        (right-associative)
            | logicalOr
logicalOr   → logicalAnd  ("OR"  logicalAnd)*
logicalAnd  → equality    ("AND" equality)*
equality    → comparison  ("==" | "!=" comparison)*
comparison  → term        ("<" | ">" | "<=" | ">=" term)*
term        → factor      ("+" | "-" factor)*
factor      → power       ("*" | "/" power)*
power       → unary       ("^" power)?          (right-associative)
unary       → ("-" | "NOT") unary | primary
primary     → INT | FLOAT | STRING | "true" | "false"
            | IDENTIFIER "(" argList ")"
            | IDENTIFIER
            | "(" expression ")"
```

The depth of this precedence chain means that an expression like `NOT a OR b AND c == d + e * f ^ 2` is parsed correctly without any ambiguity — exponentiation binds tighter than multiplication, which binds tighter than addition, and so on up to logical OR at the top.

---

### The Three-Token Lookahead Problem

The most technically interesting design challenge was **disambiguation at the top level**. Both a variable declaration and a function declaration begin identically:

```
int result = ...    variable declaration
int add(   ...      function declaration
```

Both start with `TYPE IDENTIFIER`. The parser cannot decide which rule to apply until it sees the *third* token. The original parser stored only two tokens at a time (`m_current` and `m_next`). To solve this, a third slot was added:

```cpp
// parser.h
Token m_current;
Token m_next;
Token m_nextNext;  // THIS is the deciding token
```

The constructor and `advance()` were updated to keep all three slots filled:

```cpp
Parser::Parser(Lexer& lexer) : m_lexer(lexer)
{
    m_current  = m_lexer.getNextToken();
    m_next     = m_lexer.getNextToken();
    m_nextNext = m_lexer.getNextToken();
}

Token Parser::advance()
{
    Token prev = m_current;
    m_current  = m_next;
    m_next     = m_nextNext;
    m_nextNext = m_lexer.getNextToken();
    return prev;
}
```

The top-level dispatch in `parse()` could then make the correct decision in one glance:

```cpp
if (isTypeKeyword()
    && checkNext(TokenType::IDENTIFIER)
    && checkNextNext(TokenType::LPAREN))
{
    stmts.push_back(parseFunctionDecl());
}
else
{
    stmts.push_back(parseStatement());
}
```

A similar lookahead is used to distinguish assignment (`x = expr`) from equality comparison (`x == expr`). Since the lexer already handles `=` vs `==` as separate token types, the parser only needs to check `m_current == IDENTIFIER` and `m_next == ASSIGN`:

```cpp
ASTNodePtr Parser::parseAssignment()
{
    if (check(TokenType::IDENTIFIER) && checkNext(TokenType::ASSIGN))
    {
        std::string name = m_current.lexeme;
        advance(); // consume IDENTIFIER
        advance(); // consume '='
        ASTNodePtr value = parseExpression();
        return std::make_unique<AssignNode>(name, std::move(value));
    }
    return parseLogicalOr();
}
```

---

### What the Parser Now Supports

It is worth appreciating the full scope of what this parser handles:

- **Typed variable declarations** with optional initialization: `int x = 5;`, `bool flag;`
- **Assignment statements**: `x = x + 1;`
- **Full function declarations** with typed parameters, any return type including `void`, and a block body
- **Function calls** as expressions, including nested calls: `add(square(x), 3)`
- **Return statements**, both with and without a value: `return x + 1;` / `return;`
- **Blocks** as first-class statements: `{ int local = 0; return local; }`
- **Arithmetic**: `+`, `-`, `*`, `/`, `^` with correct precedence and right-associative exponentiation
- **Comparisons**: `<`, `>`, `<=`, `>=`, `==`, `!=`
- **Logical operators**: `AND`, `OR`, `NOT` with correct short-circuit ordering in the grammar
- **Unary minus**: `-x`, `-(a + b)`
- **All four literal types**: integers, floats, strings, booleans
- **Grouped expressions**: `(a + b) * c`

---

### Difficulties Encountered

#### The Forgotten Third Slot

The most concrete bug encountered during development was a direct consequence of the lookahead expansion. After adding `m_nextNext` to the constructor, `advance()` was initially left unchanged:

```cpp
// Old advance ignores m_nextNext entirely
Token Parser::advance()
{
    Token prev = m_current;
    m_current  = m_next;
    m_next     = m_lexer.getNextToken(); // skips m_nextNext!
    return prev;
}
```

The effect was subtle but catastrophic. Given the input `int add(int a, int b)`:

```
Constructor:  m_current=int  m_next=add  m_nextNext=(

advance() #1: m_current=add  m_next=[reads from lexer]="int"   - LPAREN skipped
advance() #2: m_current=int  - should be "(" but isn't

expect(LPAREN) - sees "int" - ParseError: Expected '(' after function name
```

The `(` token was silently swallowed during construction and then never retrieved because `advance()` bypassed the third slot. The fix was simply making sure all three positions shift correctly, as shown in the updated `advance()` above. This was a good reminder that expanding a buffer requires updating *every* place that interacts with it, not just the constructor.

#### Nullable Nodes and `nullptr` as a Value

Several AST nodes have optional children: a variable declaration may or may not have an initializer, a return statement may or may not return a value. This was handled by allowing `ASTNodePtr` fields to be `nullptr`:

```cpp
// Both of these are valid:
int x;          // VarDeclNode with initializer == nullptr
int x = 5;      // VarDeclNode with initializer pointing to IntLiteralNode(5)
```

The `print()` method of each such node checks before dereferencing:

```cpp
void print(int indent = 0) const override
{
    printIndent(indent);
    std::cout << "VarDecl(" << typeName << " " << varName << ")\n";
    if (initializer)
        initializer->print(indent + 1);
}
```

Early versions did not include this guard and crashed immediately on uninitialized declarations.

#### Right-Associativity of `^` and `=`

Most binary operators are left-associative: `a - b - c` means `(a - b) - c`. Exponentiation and assignment are right-associative: `a ^ b ^ c` should mean `a ^ (b ^ c)`, and `x = y = 5` should mean `x = (y = 5)`. Left-associativity is implemented with a `while` loop; right-associativity requires a recursive call instead:

```cpp
// Left-associative (e.g., addition)
ASTNodePtr Parser::parseTerm()
{
    ASTNodePtr left = parseFactor();
    while (check(TokenType::PLUS) || check(TokenType::MINUS))
    {
        std::string op = m_current.lexeme;
        advance();
        left = std::make_unique<BinaryOpNode>(op, std::move(left), parseFactor());
    }
    return left;
}

// Right-associative (exponentiation)
ASTNodePtr Parser::parsePower()
{
    ASTNodePtr base = parseUnary();
    if (check(TokenType::EXPONENT))
    {
        std::string op = m_current.lexeme;
        advance();
        return std::make_unique<BinaryOpNode>(op, std::move(base), parsePower()); // recursive
    }
    return base;
}
```

---

### New Output vs Old Output

Previously, the only output from the program was the token stream from the lexer. For the input:

```
int add(int a, int b) {
    int result = a + b;
    return result;
}

int main() {
    int x = add(3, 4);
    return x;
}
```

The **lexer alone** produced a flat list:

```
INT_TYPE
IDENTIFIER(add)
LPAREN
INT_TYPE
IDENTIFIER(a)
COMMA
INT_TYPE
IDENTIFIER(b)
RPAREN
LEFT_CURLY_BRACKET
INT_TYPE
IDENTIFIER(result)
ASSIGN
IDENTIFIER(a)
PLUS
IDENTIFIER(b)
SEMICOLON
RETURN
IDENTIFIER(result)
SEMICOLON
RIGHT_CURLY_BRACKET
...
```

This output contains all the right characters but conveys no structure: `a`, `PLUS`, and `b` appear as three equal things in a list, with no indication that they form a sum, or that the sum is being assigned to `result`, or that any of this is inside a function.

The **parser and AST** now produce:

```
Program
  FunctionDecl(int add)
    Params
      Param(int a)
      Param(int b)
    Block
      VarDecl(int result)
        BinaryOp(+)
          Identifier(a)
          Identifier(b)
      Return
        Identifier(result)
  FunctionDecl(int main)
    Params
    Block
      VarDecl(int x)
        FunctionCall(add)
          IntLiteral(3)
          IntLiteral(4)
      Return
        Identifier(x)
```

Every relationship in the source code is now explicit: the `+` node owns `a` and `b` as children; the `VarDecl` owns the `+` node as its initializer; the `Block` owns both statements; and the `FunctionDecl` owns the parameter list and the block. The tree can be walked, analyzed, or transformed, all the groundwork needed for semantic analysis or code generation.

---

### Conclusion

The addition of the parser and AST represents a qualitative leap in what the program understands about its input. The lexer knew *what* the tokens were; the parser now knows *what they mean*. A flat list of 30+ tokens has been turned into a structured object that directly encodes the programmer's intent, which functions exist, what they accept, what they compute, and what they return.

The implementation handles a genuinely expressive subset of a typed imperative language: full function declarations with parameters, nested expressions with correct precedence across eight levels, all four primitive types, logical and comparison operators, blocks, and return statements. All of this was achieved with a clean recursive-descent design that mirrors the grammar almost one-to-one, making it easy to extend further — adding `if`/`while` statements, for instance, would each require adding a single token, a single AST node, and a single parse function.

Next steps could include a **semantic analysis pass** (type checking, undefined variable detection) that walks the AST, or a **code generation** stage that emits output from the tree nodes. Both would consume the AST produced here without any changes to the lexer or parser.
