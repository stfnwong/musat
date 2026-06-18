#include "solver.hpp"

#include <gtest/gtest.h>

namespace {

Solver make(int n_vars, std::vector<std::vector<int>> clauses) {
	Solver s;
	s.n_vars = n_vars;
	s.value.assign(n_vars + 1, Val::UNDEF);
	s.clauses = std::move(clauses);

	return s;
}


// Verify a returned assignment actually satisfies every clause 
bool model_satisfies(const Solver& s) {
	for( const auto& c: s.clauses) { 
		bool clause_ok = false;
		for( int lit: c ) { 
			Val v = s.value[std::abs(lit)];
			if( v == Val::UNDEF )
				continue;
			if( (v == Val::TRUE) == (lit > 0)) {
				clause_ok = true;
				break;
			}
		}

		if( !clause_ok )
			return false;
	}

	return true;
}

// ============================================================
//  boundary cases 
// ============================================================

TEST(Boundary, EmptyFormulaIsSat) {
    // No clauses ⇒ vacuously satisfied. The empty AND is true.
    Solver s = make(0, {});
    EXPECT_TRUE(s.solve());
}

} // end namespace 
