# VS Code

## Makeを用いるデバッグ

tasks.jsonに次の分を変化する必要がある：

``` json
"command": "/usr/bin/make",
"args": [
  "9cc"
],
```

`9cc` は任意のmakeコマンド名
