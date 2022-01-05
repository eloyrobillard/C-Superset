# Make

## make シェルコマンド

``` bash
make foo
```

以上を行うと、make が `foo:` というMakefileで定義されているルールを実行しようとする

## Makefile

1. コロンの前の名前（例：`foo:`）をターゲットファイルという。
2. コロンの後ろの名前（例: `: bar baz`）を依存ファイルという

> ターゲットファイルは存在しないか、依存ファイルより古い場合に限り、makeはターゲットのルールを実行する

``` cpp
CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
        $(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
        ./test.sh

clean:
        rm -f 9cc *.o *~ tmp*

.PHONY: test clean
```

* .PHONY：clean や test みたいなルールは同名のファイルを作成するために実行されるのではないが、make には分からないことだ
  * 偶然 clean や test ファイルが存在すれば、make clean / test は機能しなくてなる
  * そこで、**.PHONY**という特別な名前を使い、make にファイルを作成するつもりはないと伝えることができる

``` cpp
CFLAGS=-std=c11 -g -static
```

CFLAGS：自動的にCコンパイラに渡されるオプションを表している変数

``` cpp
SRCS=$(wildcard *.c)
```

SRCS：wildcardは提供する表現にマッチするファイル名を表す変数

``` cpp
OBJS=$(SRCS:.c=.o)
```

OBJS：`.c=.o`は置換ルールで、SRCSの値は foo.c bar.c であれば、OBJSは foo.o bar.o になる

``` cpp
$(CC) -o 9cc $(OBJS) $(LDFLAGS)
```

LDFLAGS：リンカオプションを表す変数（今回は空っぽ）

CC：コンパイラコマンドを表す変数。デフォルトはシステムそれぞれ（Linux じゃたぶん gcc）
