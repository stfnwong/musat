# MUSAT
_A small SAT solver._


## Getting started

To start do 

```bash
$ cmake -S . -B build
$ cmake --build build 
$ ./build/sat that.cnf
```

This repo has `that.cnf` as an example, but the `sat` program takes a `DIMACS` CNF [like this](https://jix.github.io/varisat/manual/0.2.0/formats/dimacs.html) as the first argument.


## Description
That's it. Its just a SAT solver. I wanted to learn more about SAT solvers so I decided to write one. Thousands of these things have been written and there is nothing special about my one, its just me trying to work out how its done.

At the time of writing this implements a DPLL (Davis-Putnam-Logeman-Loveland) solver which basically does

```
DPLL(formula F, assignment A):
    A ← BCP(F, A)                    # propagate all forced consequences
    if a clause is empty under A:    # conflict — a clause is fully falsified
        return UNSAT
    if all clauses satisfied:        # nothing left to do
        return SAT (A)
    x ← choose an unassigned variable   # the decision / branching
    if DPLL(F, A ∪ {x = true}):  return SAT
    return DPLL(F, A ∪ {x = false})     # backtrack and try the other way
```


## Example problem 
The file `that.cnf` contains a `DIMACS` spec for the pigeonhole problem [see here for an example](https://www.cs.ubc.ca/~hoos/SATLIB/Benchmarks/SAT/DIMACS/PHOLE/descr.html)


```
c pigeonhole: 2 pigeons, 1 hole — must be UNSAT
p cnf 2 3
1 0
2 0
-1 -2 0
```

After building the project run this with

```bash
$ ./build/sat that.cnf
```

and get 

```bash
s UNSATISFIABLE
```
