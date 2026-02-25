#include <iostream>
#include <set>
#include <vector>
#include "grammar.h"
#include "cassert"

int main()
{
    std::set<Symbol> nonterminals{"S", "P", "Q"};
    std::set<Symbol> terminals{"a", "b", "c", "d", "e", "f"};
    std::vector<Production> productions = {
        {{"S"}, {"a", "P"}}, {{"S"}, {"b", "Q"}}, {{"P"}, {"b", "P"}},
        {{"P"}, {"c", "P"}}, {{"P"}, {"d", "Q"}}, {{"P"}, {"e"}},
        {{"Q"}, {"e", "Q"}}, {{"Q"}, {"f", "Q"}}, {{"Q"}, {"a"}}
    };
    Symbol start = "S";
    Grammar g{terminals, nonterminals, start, productions};
    g.print();
    std::cout << "\n\n =========================\n\n";
    g.toFiniteAutomaton().print();

    std::cout << "\n\n =========================\n\n";
    for (int i = 0; i < 5; ++i)
    {
        g.generateWord();
        std::cout << "\n";
    }

    FiniteAutomaton fa = g.toFiniteAutomaton();
    // tests to be inserted here

    // ===== ACCEPTED STRINGS =====
    assert(fa.stringBelongsToLanguage("ae"));
    assert(fa.stringBelongsToLanguage("abcdea"));
    assert(fa.stringBelongsToLanguage("adeffea"));
    assert(fa.stringBelongsToLanguage("abbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbccce"));
    assert(fa.stringBelongsToLanguage("ada"));
    assert(fa.stringBelongsToLanguage("befea"));
    assert(fa.stringBelongsToLanguage("ba"));
    assert(fa.stringBelongsToLanguage("bffffa"));
    assert(fa.stringBelongsToLanguage("beeeffa"));

    // ===== FAILED STRINGS =====
    assert(!fa.stringBelongsToLanguage("zzzz"));
    assert(!fa.stringBelongsToLanguage(""));
    assert(!fa.stringBelongsToLanguage("test"));
    assert(!fa.stringBelongsToLanguage("aaa"));
    assert(!fa.stringBelongsToLanguage("abcd"));
    assert(!fa.stringBelongsToLanguage("eeeeeeeeeeeeeee"));
    assert(!fa.stringBelongsToLanguage("helpmeiamtired"));
    assert(!fa.stringBelongsToLanguage("bfee"));
    assert(!fa.stringBelongsToLanguage("befx"));

    return 0;
}
