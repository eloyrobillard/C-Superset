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

assert 252 '-(3+5)/2;' # 256 - 4 wrap
assert 4 ' ((1 < 2) + (3 > 2)) * 2 ;'
assert 1 ' ((1 < 2) + (3 > 2)) * 20 < 100; '
assert 1 ' 1 < 2 < 3 ;'
assert 0 ' 1 < 2 > 3; '
assert 1 ' 1 < 2 > 0; '
assert 1 'a=1; a;'

echo OK