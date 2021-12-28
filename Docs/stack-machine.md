# スタックマシン

## [逆ポーランド記法](https://www.wikiwand.com/ja/%E9%80%86%E3%83%9D%E3%83%BC%E3%83%A9%E3%83%B3%E3%83%89%E8%A8%98%E6%B3%95)

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
