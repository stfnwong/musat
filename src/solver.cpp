#include <cmath>
#include "solver.hpp"


void Solver::init(void) {
	this->clauses.clear();
}


Val Solver::lit_value(int lit) const {
	Val v = this->value[abs(lit)];
	if( v == Val::UNDEF ) 
		return Val::UNDEF;
	bool pos = lit > 0;
	return (Val)(( v == Val::TRUE ) == pos);
}


void Solver::assign(int lit) {
	this->value[abs(lit)] = (lit > 0) ? Val::TRUE : Val::FALSE;
	this->trail.push_back(lit);
}


bool Solver::propagate(void) {
	bool changed = true;

	while( changed ) {
		changed = false;
		for( auto& c: this->clauses) {
			int unassigned = 0;
			int last_free = 0;
			bool satisfied = false;

			for( int lit: c ) {
				Val v = this->lit_value(lit);
				if( v == Val::TRUE ) {
					satisfied = true;
					break;
				}

				if( v == Val::UNDEF ) { 
					unassigned++;
					last_free = lit;
				}
			}

			if( satisfied )
				continue;

			if( unassigned == 0)
				return true;   // conflict: clause fully false

			if( unassigned  == 1 ) {
				this->assign(last_free);
				changed = true;
			}
		}
	}

	return false;
}


void Solver::backtrack_to(int level) { 
	int target = this->trail_lim[level];
	while( (int) trail.size() > target ) { 
		this->value[std::abs(this->trail.back())] = Val::UNDEF;
		this->trail.pop_back();
	}
	
	this->trail_lim.resize(level);
}


int Solver::pick_var(void) {
	for( int v = 1; v <= this->n_vars; v++) {
		if(value[v] == Val::UNDEF )
			return v;
	}

	return 0;
}


bool Solver::solve(void) {
	if( this->propagate() ) 
		return false;

	return this->search();
}


bool Solver::search(void) { 
	int v = this->pick_var();
	if( v == 0 )
		return true;

	// Try true, then false
	for( int sign: {+1, -1} ) {
		// open a new decision level
		this->trail_lim.push_back(this->trail.size());
		this->assign(sign * v);

		if( !this->propagate() && this->search() )
			return true;

		this->backtrack_to(this->trail_lim.size());   // undo this decision
	}

	return false;
}
