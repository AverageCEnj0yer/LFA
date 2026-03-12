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

> Note: The first prototype of the lexer is left commented at the top of lexer.cpp. It serves as a record of the many errors and edge cases missed in the initial attempt, highlighting the evolution and improvements made in the final version.
