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

assert 1 ' a =1; b= 6; c=7 ; a== (c-b); '
assert 1 ' a =1; b= 6*(a+1); c=7 ; a !=(c-b); '
assert 1 ' adb =1; varb= 6*(adb+1); cull=7 ; adb !=(cull-varb); '
assert 12 ' adb =1; return varb= 6*(adb+1); cull=7 ; adb !=(cull-varb); '
assert 7 ' adb =1; varb= 6*(adb+1); return cull=7 ; adb !=(cull-varb); '
assert 1 " if(1) return 1; else 0; "
assert 0 " if(0) return 1; else 0; "
assert 254 " adb = 1; if(adb < 2) if (adb == 2) return adb*4; else 256-2; else 0; "
assert 0 " adb = 1; if(adb = varb = 6 < 2) if (adb == 2) return adb*4; else 256-2; else adb-varb; "

echo OK