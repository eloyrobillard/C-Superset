# 文法の定義

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

### PEG

``` purescript
PEG = Parsing Expression Grammar
```

PEGとは、唯一、再起下降構文解析の文法を示すための形式文法。

まず、文脈自由文法法との違いから言うと：
> PEGに曖昧さはない

_曖昧さの例_：

``` ebnf
A → A + A | A − A | a
```

この場合、`a + a + a` に当たる解釈は2つある：

|   1   |   2   |
| ----- | ----- |
| a + A | A + A + A |
| a + A + A | a + A + A |
| a + a + A | a + a + A |
| a + a + a | a + a + a |

PEGの場合：

``` peg
A → A + A / A − A / a
```

'|' に対して '/' は順番に試すため、使うと最初にマッチされる表現の値が返される。そのため、`a + a + a` の解釈はこうなる：

| a + a + a | 解釈 |
| --------- | ---- |
| A + A     ||
| A + A + A | A + A は優先される |
| a + A + A ||
| a + a + A ||
| a + a + a ||

#### PEGの新記法一覧

| 書き方 | 意味 | 例 |
| ------ | ---- | ---- |
| e~1/e~2 | 選択 | A = for[a-z]+ / for -> 'fore' の場合 'fore' になる |
| &e     | 成功テスト | foo &(bar) = "bar" というテキストが後に続く場合のみ "foo" というテキストにマッチし消費する |
| !e     | 失敗テスト | foo !(bar) = "bar" というテキストが後にない場合のみ "foo" というテキストにマッチし消費する |
