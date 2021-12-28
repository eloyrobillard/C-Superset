# スタックマシン

## 逆ポーランド記法

``` rust
(4 + 3) * (2 - 1) -> 3 4 + 1 2 - *
```

## スタックマシン動作

``` rust
3 4 + 1 2 - *：

PUSH 3
PUSH 4
ADD

PUSH 1
PUSH 2
SUB

MUL
```
