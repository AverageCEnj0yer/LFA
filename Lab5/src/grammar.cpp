#include "grammar.h"

#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <utility>
#include <algorithm>

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------
namespace {

struct ProductionLess
{
    bool operator()(const Production& a, const Production& b) const
    {
        return std::tie(a.lhs, a.rhs) < std::tie(b.lhs, b.rhs);
    }
};
using ProdSet = std::set<Production, ProductionLess>;

bool isTerm(const Symbol& s, const std::set<Symbol>& T) { return T.count(s) > 0; }
bool isNT  (const Symbol& s, const std::set<Symbol>& N) { return N.count(s) > 0; }

Production mkProd(const Symbol& lhs, RHS rhs)
{
    return Production{LHS{lhs}, std::move(rhs)};
}

void dedupe(std::vector<Production>& P)
{
    ProdSet u(P.begin(), P.end());
    P.assign(u.begin(), u.end());
}

Symbol freshSymbol(const std::string& prefix,
                   const std::set<Symbol>& T,
                   const std::set<Symbol>& N)
{
    Symbol c = prefix;
    for (std::size_t i = 0; T.count(c) || N.count(c); ++i)
        c = prefix + std::to_string(i);
    return c;
}

//remove epsilon-productions
std::set<Symbol> computeNullable(const std::vector<Production>& P,
                                 const std::set<Symbol>& N)
{
    std::set<Symbol> nul;
    for (bool changed = true; changed;)
    {
        changed = false;
        for (const auto& p : P)
        {
            if (p.lhs.size() != 1 || !isNT(p.lhs.front(), N)) continue;
            bool eps = p.rhs.empty() ||
                       std::all_of(p.rhs.begin(), p.rhs.end(),
                                   [&](const Symbol& s){ return isNT(s,N) && nul.count(s); });
            if (eps && nul.insert(p.lhs.front()).second) changed = true;
        }
    }
    return nul;
}

void rhsVariants(const RHS& rhs, const std::set<Symbol>& nul,
                 std::size_t i, RHS& cur, std::set<RHS>& out)
{
    if (i == rhs.size()) { out.insert(cur); return; }
    if (nul.count(rhs[i])) rhsVariants(rhs, nul, i + 1, cur, out); // omit
    cur.push_back(rhs[i]);
    rhsVariants(rhs, nul, i + 1, cur, out);                         // keep
    cur.pop_back();
}

void removeEpsilon(std::vector<Production>& P,
                   const std::set<Symbol>& N, const Symbol& S)
{
    const auto nul = computeNullable(P, N);
    ProdSet out;
    for (const auto& p : P)
    {
        if (p.lhs.size() != 1 || !isNT(p.lhs.front(), N) || p.rhs.empty()) continue;
        std::set<RHS> variants; RHS cur;
        rhsVariants(p.rhs, nul, 0, cur, variants);
        for (const auto& v : variants)
            if (!v.empty() || p.lhs.front() == S)
                out.insert(mkProd(p.lhs.front(), v));
    }
    P.assign(out.begin(), out.end());
}

//remove unit productions (A -> B, B in N)
void removeUnit(std::vector<Production>& P,
                const std::set<Symbol>& N, const Symbol& S)
{
    std::map<Symbol, std::set<Symbol>> edges;
    std::map<Symbol, std::vector<Production>> byLhs;
    for (const auto& nt : N) edges[nt];

    for (const auto& p : P)
    {
        if (p.lhs.size() != 1 || !isNT(p.lhs.front(), N)) continue;
        byLhs[p.lhs.front()].push_back(p);
        if (p.rhs.size() == 1 && isNT(p.rhs.front(), N))
            edges[p.lhs.front()].insert(p.rhs.front());
    }

    // BFS transitive closure per NT.
    auto unitClosure = [&](const Symbol& src) {
        std::set<Symbol> reach{src};
        for (std::queue<Symbol> q({src}); !q.empty();)
        {
            Symbol u = q.front(); q.pop();
            for (const auto& v : edges[u])
                if (reach.insert(v).second) q.push(v);
        }
        return reach;
    };

    ProdSet out;
    for (const auto& nt : N)
        for (const auto& tgt : unitClosure(nt))
            for (const auto& p : byLhs[tgt])
            {
                if (p.rhs.size() == 1 && isNT(p.rhs.front(), N)) continue; // skip unit
                if (p.rhs.empty() && nt != S)                     continue;
                out.insert(mkProd(nt, p.rhs));
            }

    P.assign(out.begin(), out.end());
}

//isolate terminals in mixed RHS (|rhs| >= 2)
void isolateTerminals(std::vector<Production>& P,
                      std::set<Symbol>& N, const std::set<Symbol>& T)
{
    std::map<Symbol, Symbol> proxy;
    ProdSet out;
    for (const auto& p : P)
    {
        if (p.lhs.size() != 1 || !isNT(p.lhs.front(), N)) continue;
        if (p.rhs.size() <= 1) { out.insert(p); continue; }

        RHS rhs = p.rhs;
        for (auto& s : rhs)
        {
            if (!isTerm(s, T)) continue;
            auto [it, inserted] = proxy.emplace(s, Symbol{});
            if (inserted)
            {
                it->second = freshSymbol("T", T, N);
                N.insert(it->second);
                out.insert(mkProd(it->second, RHS{s}));
            }
            s = it->second;
        }
        out.insert(mkProd(p.lhs.front(), rhs));
    }
    P.assign(out.begin(), out.end());
}

//binarise productions with |rhs| > 2
void binarise(std::vector<Production>& P,
              std::set<Symbol>& N, const std::set<Symbol>& T)
{
    ProdSet out;
    for (const auto& p : P)
    {
        if (p.lhs.size() != 1 || !isNT(p.lhs.front(), N)) continue;
        if (p.rhs.size() <= 2) { out.insert(p); continue; }

        Symbol cur = p.lhs.front();
        for (std::size_t i = 0; i + 2 < p.rhs.size(); ++i)
        {
            Symbol next = freshSymbol("X", T, N);
            N.insert(next);
            out.insert(mkProd(cur, RHS{p.rhs[i], next}));
            cur = next;
        }
        out.insert(mkProd(cur, RHS{p.rhs[p.rhs.size() - 2], p.rhs.back()}));
    }
    P.assign(out.begin(), out.end());
}

// cleanup – remove non-productive symbols
void removeNonProductive(std::vector<Production>& P,
                         std::set<Symbol>& N,
                         const std::set<Symbol>& T,
                         const Symbol& S)
{
    std::set<Symbol> prod;
    for (bool changed = true; changed;)
    {
        changed = false;
        for (const auto& p : P)
        {
            if (p.lhs.size() != 1 || !isNT(p.lhs.front(), N)) continue;
            bool ok = std::all_of(p.rhs.begin(), p.rhs.end(), [&](const Symbol& s){
                return isTerm(s, T) || (isNT(s, N) && prod.count(s));
            });
            if (ok && prod.insert(p.lhs.front()).second) changed = true;
        }
    }

    ProdSet out;
    for (const auto& p : P)
    {
        if (p.lhs.size() != 1 || !isNT(p.lhs.front(), N)) continue;
        if (p.lhs.front() != S && !prod.count(p.lhs.front())) continue;
        bool ok = std::all_of(p.rhs.begin(), p.rhs.end(), [&](const Symbol& s){
            return isTerm(s, T) || (isNT(s, N) && prod.count(s));
        });
        if (ok) out.insert(p);
    }
    P.assign(out.begin(), out.end());

    std::set<Symbol> keep{S};
    for (const auto& nt : N)  if (prod.count(nt))   keep.insert(nt);
    for (const auto& p  : P)
    {
        keep.insert(p.lhs.front());
        for (const auto& s : p.rhs) if (isNT(s, N)) keep.insert(s);
    }
    N = std::move(keep);
}

// cleanup – remove inaccessible symbols
void removeInaccessible(std::vector<Production>& P,
                        std::set<Symbol>& N,
                        std::set<Symbol>& T,
                        const Symbol& S)
{
    std::set<Symbol> reachN{S}, reachT;
    for (bool changed = true; changed;)
    {
        changed = false;
        for (const auto& p : P)
        {
            if (p.lhs.size() != 1 || !reachN.count(p.lhs.front())) continue;
            for (const auto& s : p.rhs)
            {
                if      (isNT(s, N) && reachN.insert(s).second) changed = true;
                else if (isTerm(s, T))                           reachT.insert(s);
            }
        }
    }

    ProdSet out;
    for (const auto& p : P)
    {
        if (p.lhs.size() != 1 || !reachN.count(p.lhs.front())) continue;
        bool ok = std::all_of(p.rhs.begin(), p.rhs.end(), [&](const Symbol& s){
            return !isNT(s, N) || reachN.count(s);
        });
        if (ok) out.insert(p);
    }
    P.assign(out.begin(), out.end());

    std::set<Symbol> kN;
    for (const auto& nt : reachN) if (isNT(nt, N)) kN.insert(nt);
    kN.insert(S);
    N = std::move(kN);

    for (const auto& p : P)           // sweep to capture any remaining terminals
        for (const auto& s : p.rhs)
            if (isTerm(s, T)) reachT.insert(s);
    T = std::move(reachT);
}

} 

// --------------------------
// Grammar member definitions
// --------------------------
Grammar::Grammar(std::set<Symbol> terminals, std::set<Symbol> nonterminals,
                 Symbol start, std::vector<Production> productions)
    : m_terminals{std::move(terminals)}
    , m_nonterminals{std::move(nonterminals)}
    , m_start{std::move(start)}
    , m_productions{std::move(productions)}
{}

void Grammar::print() const
{
    std::cout << "Terminals: { ";
    for (const auto& t : m_terminals)     std::cout << t << " ";
    std::cout << "}\nNonterminals: { ";
    for (const auto& nt : m_nonterminals) std::cout << nt << " ";
    std::cout << "}\nStart symbol: " << m_start << "\nProductions:\n";
    for (const auto& p : m_productions)
    {
        if (p.lhs.empty()) continue;
        std::cout << "  " << p.lhs.front() << " -> ";
        if (p.rhs.empty()) std::cout << "eps";
        else for (const auto& s : p.rhs) std::cout << s;
        std::cout << "\n";
    }
}

const std::set<Symbol>&        Grammar::terminals()    const { return m_terminals;   }
const std::set<Symbol>&        Grammar::nonterminals() const { return m_nonterminals;}
const Symbol&                  Grammar::start()        const { return m_start;       }
const std::vector<Production>& Grammar::productions()  const { return m_productions; }


//converts any Grammar to Chomsky Normal Form
Grammar normalize(const Grammar& grammar)
{
    std::set<Symbol>        T = grammar.terminals();
    std::set<Symbol>        N = grammar.nonterminals();
    Symbol                  S = grammar.start();
    std::vector<Production> P = grammar.productions();

    dedupe(P);

    // 1. Fresh start symbol so the original S never appears on a RHS.
    Symbol S0 = freshSymbol("S0", T, N);
    N.insert(S0);
    P.push_back(mkProd(S0, RHS{S}));
    S = S0;
    dedupe(P);

    // 2. Remove epsilon-productions.
    removeEpsilon(P, N, S);

    // 3. Remove unit productions (A -> B, B in N).
    removeUnit(P, N, S);

    // 4. Replace terminals in mixed RHS with proxy NTs.
    isolateTerminals(P, N, T);

    // 5. Binarise RHS longer than 2.
    binarise(P, N, T);

    // Cleanup: non-productive then inaccessible.
    removeNonProductive(P, N, T, S);
    removeInaccessible(P, N, T, S);
    dedupe(P);

    return Grammar(T, N, S, P);
}

//checks if grammar is normalized (for testing)
bool isChomskyNormalForm(const Grammar& grammar)
{
    const auto& T = grammar.terminals();
    const auto& N = grammar.nonterminals();
    const auto& S = grammar.start();
    bool startHasEps = false;

    for (const auto& p : grammar.productions())
    {
        if (p.lhs.size() != 1 || !isNT(p.lhs.front(), N)) return false;
        const Symbol& lhs = p.lhs.front();

        if (p.rhs.empty())
        {
            if (lhs != S) return false;
            startHasEps = true;
        }
        else if (p.rhs.size() == 1)
        {
            if (!isTerm(p.rhs.front(), T)) return false;
        }
        else if (p.rhs.size() == 2)
        {
            if (!isNT(p.rhs[0], N) || !isNT(p.rhs[1], N)) return false;
        }
        else return false;
    }

    if (startHasEps)
        for (const auto& p : grammar.productions())
            for (const auto& s : p.rhs)
                if (s == S) return false;

    return true;
}
