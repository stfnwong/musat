#include <cstdint>
#include <vector>

// A really basic SAT solver

enum Val : int8_t {
	UNDEF = -1,
	FALSE = 0,
	TRUE = 1
};


// DPLL solver
struct Solver {
	int n_vars;
	std::vector<std::vector<int>> clauses;  // each clause - list of literals 
	std::vector<Val> value;		            // indexed by variables (1..n_vars)
	std::vector<int> trail;                 // assigned literals, in order 
	std::vector<int> trail_lim;             // trail indices where each decision level began

	public:
		void init(void);
		Val lit_value(int lit) const;
		void assign(int lit);

		bool propagate(void);
		void backtrack_to(int level);
		int pick_var(void);
		bool solve(void);
		bool search(void);
};
