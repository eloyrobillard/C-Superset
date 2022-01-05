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
assert 1 " if(0) return 1; else 0; 1; "
assert 254 " adb = 1; if(adb < 2) if (adb == 2) return adb*4; else 256-2; else 0; "
assert 2 " adb = 1; if(adb = varb = 6 < 2) if (adb == 2) return adb*4; else 256-2; else adb = 1 + (adb = adb + 1); "
assert 255 " a = 0; while (a < 255) a = a + 1;"
assert 10 " a = 0; while (a < 255) if (a == 120) return 10; else a = a + 1;"
assert 160 " if ((a = 0) < 120) while (a < 160) a = a + 1;"
assert 6 " if ((a = 0) < 120) while (a < 160) a = a + 1; 6;"
assert 235 " for (a = 0; a < 235; a = a + 1) a;"
assert 235 " a = 0; for (; a < 235; a = a + 1) a;"
assert 0 " a = 0; for (;; a = a + 1) return a;"
assert 130 " a = 0; for (;; a = a + 1) if (a == 130) return a;"
assert 0 " a = 0; b = 0; for (;; b = b + 1) if (b > 130) return a;"
assert 122 " a = 0; b = 0; for (;; b = b + 1) if (b > 60) return a; else a = a + 2;"

echo OK