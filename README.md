# Currently a C superset compiler

Started as a C compiler based on [Rui Ueyama](https://www.sigbus.info/compilerbook)'s book. Currently slowly implementing Functional Programming features, including:

* if-expressions
* pattern matching
* ADTs

Other features I'm working on:

* Global scope as `main`, inspired by Nim
* Rust-like type syntax (i32, i64, etc)
* LLVM integration (one day)

## Get Started

Run `make .PHONY` from root, see which of the tests pass
