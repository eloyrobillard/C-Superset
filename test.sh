#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./math-rd "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => expected $expected, but got $actual"
    exit 1
  fi
}

assert 0 0
assert 42 42
assert 123 123
assert 21 "5+20-4"
assert 41 " 12 + 34 - 5 "
assert 32 " 12 + 4 * 5 "
assert 80 " (12 + 4) * 5 "
assert 6  " (2 + 1) * (4 / (3 - 1)) "

echo OK