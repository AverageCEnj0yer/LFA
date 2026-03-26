// #include <iostream>

// #include "lexer.h"
// #include "token.h"
// #include <vector>
// int main()
// {
//     //You got this soldier
//     Lexer l{"id/+price/*"};
//     std::vector<Token> tokens{};

//     while (!l.isAtEnd())
//     {
//         tokens.push_back(l.getNextToken());
//     }

//     for(Token token : tokens)
//     {
//         printToken(token);
//         std::cout << token.lexeme << "\n";
//     }
    
//     return 0;
// }


#include <iostream>
#include <vector>
#include <string>
#include "lexer.h"
#include "parser.h"
#include "nodes.h"



void runTest(const std::string& label, const std::string& regex, int iterations = 5) {
    std::cout << "=== TEST: " << label << " ===" << std::endl;
    std::cout << "Regex: " << regex << std::endl;

    try {
        Lexer lexer(regex);
        Parser parser(lexer);
        Node* root = parser.parseExpression();

        std::cout << "Tree Structure:" << std::endl;
        root->printStructure();

        std::cout << "Generated Strings:" << std::endl;
        for (int i = 0; i < iterations; ++i) {
            std::cout << "  " << (i + 1) << ": " << root->generate() << std::endl;
        }

        delete root; // Critical: Triggers recursive cleanup
    } catch (const std::exception& e) {
        std::cerr << "Runtime Error: " << e.what() << std::endl;
    }
    std::cout << "---------------------------\n" << std::endl;
}

int main() {
    // Test 1: Simple Literals & Sequence
    runTest("Nr 1", "(a|b)(c|d)E+G?");

    // Test 2: Alternation (OR)
    runTest("Nr 2", "P(Q|R|S)T(UV|W|X)*Z+");

    // Test 3: Quantifiers
    runTest("Nr 3", "1(0|1)*2(3|4)^5 36");


    return 0;
}
