// #include <iostream>
// #include "lexer.h"
// int main()
// {
//     Lexer lex{"./tests/test2.lex"};
//     lex.fileContent();
//     Token t1{};
//     while (!lex.isAtEnd())
//     {
//         t1 = lex.getNextToken();
//         printToken(t1);
        
//     }
//     return 0;
// }



#include <iostream>
#include "lexer.h"
#include "parser.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <source file>\n";
        return 1;
    }

    try
    {
        Lexer  lexer(argv[1]);
        Parser parser(lexer);

        auto ast = parser.parse();
        ast->print();
    }
    catch (const ParseError& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}
