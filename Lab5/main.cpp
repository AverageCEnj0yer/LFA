#include "include/grammar.h"

#include <cassert>
#include <set>
#include <vector>
#include <iostream>

int main()
{
	// Variant 1
	{
		std::set<Symbol> terminals{"a", "b"};
		std::set<Symbol> nonterminals{"S", "A", "B", "C", "D", "E"};
		const Symbol start = "S";
		std::vector<Production> productions{
			{LHS{"S"}, RHS{"a", "B"}},
			{LHS{"S"}, RHS{"A", "C"}},
			{LHS{"A"}, RHS{"a"}},
			{LHS{"A"}, RHS{"A", "S", "C"}},
			{LHS{"A"}, RHS{"B", "C"}},
			{LHS{"A"}, RHS{"a", "D"}},
			{LHS{"B"}, RHS{"b"}},
			{LHS{"B"}, RHS{"b", "S"}},
			{LHS{"C"}, RHS{}},
			{LHS{"C"}, RHS{"B", "A"}},
			{LHS{"E"}, RHS{"a", "B"}},
			{LHS{"D"}, RHS{"a", "b", "C"}},
		};
		Grammar g(terminals, nonterminals, start, productions);
		assert(!isChomskyNormalForm(g));
		Grammar gn = normalize(g);
		assert(isChomskyNormalForm(gn));
		std::cout << "Variant 1 CNF normalization: OK\n";
	}

	// Variant 2
	{
		std::set<Symbol> terminals{"a", "b"};
		std::set<Symbol> nonterminals{"S", "A", "B", "C", "D"};
		const Symbol start = "S";
		std::vector<Production> productions{
			{LHS{"S"}, RHS{"a", "B"}},
			{LHS{"S"}, RHS{"b", "A"}},
			{LHS{"A"}, RHS{"b"}},
			{LHS{"A"}, RHS{"a", "D"}},
			{LHS{"A"}, RHS{"A", "S"}},
			{LHS{"A"}, RHS{"b", "A", "A", "B"}},
			{LHS{"A"}, RHS{}},
			{LHS{"B"}, RHS{"b"}},
			{LHS{"B"}, RHS{"b", "S"}},
			{LHS{"C"}, RHS{"A", "B"}},
			{LHS{"D"}, RHS{"B", "B"}},
		};
		Grammar g(terminals, nonterminals, start, productions);
		assert(!isChomskyNormalForm(g));
		Grammar gn = normalize(g);
		assert(isChomskyNormalForm(gn));
		std::cout << "Variant 2 CNF normalization: OK\n";
	}

	// Variant 3
	{
		std::set<Symbol> terminals{"a", "d"};
		std::set<Symbol> nonterminals{"S", "A", "B", "C", "D", "E"};
		const Symbol start = "S";
		std::vector<Production> productions{
			{LHS{"S"}, RHS{"d", "B"}},
			{LHS{"S"}, RHS{"A"}},
			{LHS{"A"}, RHS{"d"}},
			{LHS{"A"}, RHS{"d", "S"}},
			{LHS{"A"}, RHS{"a", "A", "d", "A", "B"}},
			{LHS{"B"}, RHS{"a", "C"}},
			{LHS{"B"}, RHS{"a", "S"}},
			{LHS{"B"}, RHS{"A", "C"}},
			{LHS{"C"}, RHS{}},
			{LHS{"E"}, RHS{"A", "S"}},
		};
		Grammar g(terminals, nonterminals, start, productions);
		assert(!isChomskyNormalForm(g));
		Grammar gn = normalize(g);
		assert(isChomskyNormalForm(gn));
		std::cout << "Variant 3 CNF normalization: OK\n";
	}

	// Variant 4
	{
		std::set<Symbol> terminals{"a", "b"};
		std::set<Symbol> nonterminals{"S", "A", "B", "C", "D"};
		const Symbol start = "S";
		std::vector<Production> productions{
			{LHS{"S"}, RHS{"a", "B"}},
			{LHS{"S"}, RHS{"b", "A"}},
			{LHS{"S"}, RHS{"A"}},
			{LHS{"S"}, RHS{"B"}},
			{LHS{"A"}, RHS{"A", "S"}},
			{LHS{"A"}, RHS{"b", "B", "A", "B"}},
			{LHS{"A"}, RHS{"b"}},
			{LHS{"B"}, RHS{"b"}},
			{LHS{"B"}, RHS{"b", "S"}},
			{LHS{"B"}, RHS{"a", "D"}},
			{LHS{"B"}, RHS{}},
			{LHS{"D"}, RHS{"A", "A"}},
			{LHS{"C"}, RHS{"B", "a"}},
		};
		Grammar g(terminals, nonterminals, start, productions);
		assert(!isChomskyNormalForm(g));
		Grammar gn = normalize(g);
		assert(isChomskyNormalForm(gn));
		std::cout << "Variant 4 CNF normalization: OK\n";
	}

	std::cout << "All CNF normalization tests passed.\n";
	return 0;
}
