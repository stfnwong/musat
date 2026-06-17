#include "solver.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Parse DIMACS CNF from any input stream into the solver.
// Format: 'c' comment lines, one 'p cnf <vars> <clauses>' header,
// then clauses as space-separated literals terminated by 0.
static bool parseDimacs(std::istream& in, Solver& s) {
    std::string line;
    std::vector<int> clause;
    bool headerSeen = false;

    while (std::getline(in, line)) {
        std::istringstream ls(line);
        char first;
        if (!(ls >> first)) continue;        // blank line

        if (first == 'c') continue;          // comment
        if (first == 'p') {                  // header: p cnf <n_vars> <nClauses>
            std::string fmt;
            int nClauses;
            ls >> fmt >> s.n_vars >> nClauses;
            if (fmt != "cnf") {
                std::cerr << "error: expected 'cnf', got '" << fmt << "'\n";
                return false;
            }
            s.value.assign(s.n_vars + 1, UNDEF);   // 1-indexed; slot 0 unused
            headerSeen = true;
            continue;
        }

        // Otherwise it's clause data. Put the first token back and read ints.
        ls.putback(first);
        int lit;
        while (ls >> lit) {
            if (lit == 0) {                  // clause terminator
                s.clauses.push_back(clause);
                clause.clear();
            } else {
                clause.push_back(lit);
            }
        }
    }

    // A clause may span the EOF without a trailing newline; flush any remainder.
    if (!clause.empty()) s.clauses.push_back(clause);

    if (!headerSeen) {
        std::cerr << "error: no 'p cnf' header found\n";
        return false;
    }
    return true;
}

int main(int argc, char** argv) {
    Solver s;
    s.clauses.clear();

    bool ok;
    if (argc > 1) {                          // read from file
        std::ifstream file(argv[1]);
        if (!file) {
            std::cerr << "error: cannot open '" << argv[1] << "'\n";
            return 2;
        }
        ok = parseDimacs(file, s);
    } else {                                 // read from stdin
        ok = parseDimacs(std::cin, s);
    }
    if (!ok) return 2;

    if (s.solve()) {
        std::cout << "s SATISFIABLE\n";
        std::cout << "v ";
        for (int v = 1; v <= s.n_vars; v++)
            std::cout << (s.value[v] == TRUE ? v : -v) << ' ';
        std::cout << "0\n";
        return 10;                           // DIMACS convention: 10 = SAT
    } else {
        std::cout << "s UNSATISFIABLE\n";
        return 20;                           // 20 = UNSAT
    }
}
