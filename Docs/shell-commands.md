# シェルコマンド表

> 作成日時 水曜日 22 12月 2021

## objdump

* -d /bin/ls
   > lsアプリの逆アセンブリ
* -D
   > Like -d, but disassemble the contents of all sections, not just those expected to contain instructions.  \
   > `char main[] = "\x48\xc7\xc0\x2a\x00\x00\x00\xc3";`みたいなデータを関数として逆アセンブルに用いる
* -S
   > C言語のソースコード付きで逆アセンブル
* -M intel
   > x64セットの構文を表示する
* --disassemble=<関数名>
   > 特定な関数を逆アセンブル（関数一つ対応のみ？）

## gdb

* -ex '<命令string>'
   > 'file <ファイル名>' 対象ファイル決定  \
   >'disassemble <関数名>' 関数逆アセンブリ（複数OK！）

## gcc

```bash
$ cc -o test1 test1.c
$ ./test1
$ echo $?
42
```

上記における `$ echo $?` の意味とは？
終了コードは暗黙のうちにシェルの$?という変数にセットされているので、echo で表示できる。

* -g：gdbで使用可能なデバッグ情報を生成します
* -pg：gprofで使用可能なプロファイル情報を作成するコードを生成します
* -Wl,<オプション>：リンカにオプションを渡すため
  * --omagic：データが実行禁止領域にマップされないとのこと

## chmod

* ユーザー許可モード：ugoa
  * u：ファイルの所有者
  * g：所有者以外、ファイルと同グループに属しているユーザー
  * o：ファイルのグループに*属していない*ユーザー
  * a：すべてのユーザー
* 使い方：`chmod a+w <ファイル名>`
  * すべてのユーザーに書き込み許可を与える
* 数値モード：`chmod 7621`
  * u：全許可
  * g：実行以外の許可
  * o：書き込みだけ
  * a：実行だけ

## bash

* -x：bashスクリプトのトレースを表示する

```Bash
bash -x <ファイル名>
```
