# 生成規則による文法の定義とEBNF

## 生成規則

ノームチョムスキーの生成文法（generative grammar）における概念：
> どんな言語でも再帰的構造に基づく。「『文』とは『主語』と『述語』からなる」のようなルールによる文を展開し、なお長い文が作れるわけだ

``` cpp
赤い -> 少し赤い -> 少し赤い花がきれいだと私は思った
```

## BNFとEBNF

``` scala
BNF = Backus-Naur form
```

### BNFの生成規則

* 形式：A = $\alpha_{1}\alpha_{2}...$（0個以上の記号の列）
* 非終端記号：規則の左辺に来る展開できる記号
  > A = $\alpha$ と $\alpha$ = $\beta_1\beta_2$ ならば、$\alpha$ は展開できて非終端記号だという
* 終端記号：'k'文字のように、それ以上展開できない記号

***
生成規則による定義された文法を **「文脈自由文法」（context free grammar）** という
***

### 例

1. 任意に長い 'a' の文字列の定義：
   > A = aA -> A = aaA -> A = aaA...
2. fizzbuzz
   > A = fizzB および A = buzz$\epsilon$、B = buzz と $\epsilon$
   >> 結果: fizz / buzz / fizzbuzz

### EBNF

``` haskell
EBNF = Extended Backus-Naur form
```

EBNFでは、BNFのルールに加え、以下の記号を使って複雑なルールを簡潔に書き下すことができる

| EBNF | BNF |
| ---- | ---: |
| A = $\alpha$* | A = $\alpha$A と A = $\epsilon$ |
| A = $\alpha$? | A = $\alpha$ と A = $\epsilon$ |
| A = $\alpha$ ｜ $\beta$ | A = $\alpha$ と A = $\beta$ |
| A = $\alpha$ ($\beta_1\beta_2...$) y | A = $\alpha$ B と B = $\beta_1\beta_2...$ |
