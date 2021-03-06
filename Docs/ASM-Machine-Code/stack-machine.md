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

## x86-64 レジスタマシン翻訳

``` rust
3 4 + 1 2 - /：

push 3
push 4

// 左辺と右辺をRAXとRDIにポップして足す
pop rdi
pop rax
add rax, rdi
// 足した結果をスタックにプッシュ
push rax

push 1
push 2

// 左辺と右辺をRAXとRDIにポップして割る
pop rdi
pop rax
imul rdi
// 足した結果をスタックにプッシュ
push rax
```

## 制御構造

以下により、XXXは通し番号などにして、全てのラベルがユニークにするためにある。

### if (A) B

``` nasm
  Aをコンパイルしたコード // スタックトップに結果が入っているはず
  pop rax
  cmp rax, 0
  je  .LendXXX
  Bをコンパイルしたコード
.LendXXX:
```

### if (A) B else C

``` nasm
  Aをコンパイルしたコード // スタックトップに結果が入っているはず
  pop rax
  cmp rax, 0
  je  .LelseXXX
  Bをコンパイルしたコード
  jmp .LendXXX
.LelseXXX
  Cをコンパイルしたコード
.LendXXX
```

### while (A) B

``` nasm
.LbeginXXX:
  ; Aをコンパイルしたコード
  pop rax
  cmp rax, 0
  je .LendXXX
  ; Bをコンパイルしたコード
  jmp .LbeginXXX
.LendXXX:
```

### for (A; B; C) D

``` nasm
  Aをコンパイルしたコード
.LbeginXXX:
  Bをコンパイルしたコード
  pop rax
  cmp rax, 0
  je  .LendXXX
  Dをコンパイルしたコード
  Cをコンパイルしたコード
  jmp .LbeginXXX
.LendXXX:
```
