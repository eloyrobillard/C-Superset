#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
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

assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"
assert 32 " 12 + 4 * 5; "
assert 80 " (12 + 4) * 5 ;"
assert 6  " (2 + 1) * (4 / (3 - 1)); "
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 252 '-(3+5)/2;' # 256 - 4 wrap
assert 4 ' ((1 < 2) + (3 > 2)) * 2 ;'
assert 1 ' ((1 < 2) + (3 > 2)) * 20 < 100; '
assert 1 ' 1 < 2 < 3 ;'
assert 0 ' 1 < 2 > 3; '
assert 1 ' 1 < 2 > 0; '
assert 1 'a=1; a;'

echo OK