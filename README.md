libncipher
===

[![license](https://img.shields.io/badge/License-MIT-blue.svg?style=flat)](https://raw.githubusercontent.com/sasairc/libncipher/master/LICENSE)
[![build](https://img.shields.io/travis/sasairc/libncipher.svg?style=flat)](https://travis-ci.org/sasairc/libncipher)

C/C++向け[n_cipher](https://github.com/844196/n_cipher)ライブラリ。

## Requirements

* glib-2.0


## Install

```shellsession
$ make
# make install
```


## Usage

```c
#include <n_cipher.h>

#define SEED        "にゃんぱす\0"
#define DELIMITER   "〜\0"

char* encode_n_cipher(char* string, char* seed, char* delimiter);
char* decode_n_cipher(char* string, char* seed, char* delimiter);
```

エラー時には`NULL`を返却します。


## Bug

* encode_n_cipher()のシード値に4文字指定するとコケる #10


## License

[MIT](https://github.com/sasairc/libncipher/blob/master/LICENSE)
