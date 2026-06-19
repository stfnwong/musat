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


TEST(Boundary, EmptyClauseIsUnsat) {
	// A clause with no literals can never be satisfied regardless of any other clauses 
	Solver s = make(1, {{}});
	EXPECT_FALSE(s.solve());
}


TEST(Boundary, SingleUnitClause) { 
	// (x1) forces x1 = true
	Solver s = make(1, {{1}});
	EXPECT_TRUE(s.solve());
	EXPECT_EQ(s.value[1], TRUE);
}


TEST(Boundary, DirectContradiction) {
	// (x1) ∧ (x2) (UNSAT)
	Solver s = make(1, {{1}, {-1}});
	EXPECT_FALSE(s.solve());
}

// ============================================================
//  Propagation
// ============================================================
TEST(Propagation, ChainOfUnits) { 
	// (x1) forces x1; (¬x1 ∨ x2) then forces x2; (¬x2 ∨ x3) forces x3.
	// This is just a propagation cascade with no decision required.
	Solver s = make(3, {{1}, {-1, 2}, {-2, 3}});
	EXPECT_TRUE(s.solve());
	EXPECT_EQ(s.value[1], TRUE);
	EXPECT_EQ(s.value[2], TRUE);
	EXPECT_EQ(s.value[3], TRUE);
}


TEST(Propagation, PropagationLeadsToConflict) {
	// (x1) ∧ (¬x1 ∨ x2) ∧ (¬x2): 
	// x1=T -> x2=T (from clause 2) but clause 3 demands x2=F. Conflict, therefore UNSAT
	Solver s = make(2, {{1}, {-1, 2}, {-2}});
	EXPECT_FALSE(s.solve());
}


// ============================================================
//  Backtrack
// ============================================================

TEST(Search, RequiresBacktrack) { 
	// (x1 ∨ x2) ∧ (¬x1 ∨ x2) ∧ (¬x1 ∨ ¬x2)
	// No units up front which forces a decision. If the solver guesses x1=T it will 
	// be driven to a conflict and must backtrack to x1=F.
	Solver s = make(2, {{1, 2}, {-1, 2}, {-1, -2}});
	ASSERT_TRUE(s.solve());
	EXPECT_TRUE(model_satisfies(s));
}

} // end namespace 
