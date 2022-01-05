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
assert 1 ' a =1; b= 6; c=7 ; a== (c-b); '
assert 1 ' a =1; b= 6*(a+1); c=7 ; a !=(c-b); '
assert 1 ' adb =1; varb= 6*(adb+1); cull=7 ; adb !=(cull-varb); '
assert 12 ' adb =1; return varb= 6*(adb+1); cull=7 ; adb !=(cull-varb); '
assert 7 ' adb =1; varb= 6*(adb+1); return cull=7 ; adb !=(cull-varb); '
assert 1 " if(1) return 1; else return 0; "

echo OK