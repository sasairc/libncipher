libncipher
===

[![version](http://img.shields.io/github/tag/sasairc/libncipher.svg?style=flat&label=version)](https://github.com/sasairc/libncipher/releases)
[![license](https://img.shields.io/badge/License-MIT-blue.svg?style=flat)](https://raw.githubusercontent.com/sasairc/libncipher/master/LICENSE)
[![issues](http://img.shields.io/github/issues/sasairc/libncipher.svg?style=flat)](https://github.com/sasairc/libncipher/issues)
[![build](https://img.shields.io/travis/sasairc/libncipher.svg?style=flat)](https://travis-ci.org/sasairc/libncipher)

C/C++向け[n_cipher](https://github.com/844196/n_cipher)（にゃんぱす暗号）ライブラリ。	
seed値は128文字まで確認済みです。


## Requirements

* GNU Make
* gcc or clang/llvm
* pkg-config
* glib-2.0


## Install

```shellsession
% make
# make install
```


## Usage

```c
#include <n_cipher.h>

#define SEED        "にゃんぱす\0"
#define DELIMITER   "〜\0"

int check_seed_overlap_n_cipher(const char* seed);
char* encode_n_cipher(const char* string, const char* seed, const char* delimiter);
char* decode_n_cipher(const char* string, const char* seed, const char* delimiter);
```

エラー時には`NULL`を返却します。


## Example

```shellsession
% cat example.c
#include <n_cipher.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    char*   msg = "戊辰War\0",
        *   enc = NULL,
        *   dec = NULL;

    /* default seed and delimiter */
    if ((enc = encode_n_cipher(msg, SEED, DELIMITER)) == NULL)
        return 1;
    
    if ((dec = decode_n_cipher(enc, SEED, DELIMITER)) == NULL) {
        free(enc);

        return 2;
    }
    fprintf(stdout, "origin: %s\nencode: %s\ndecode: %s\nstrcmp: %d\n",
            msg, enc, dec, strcmp(msg, dec));
    free(enc);
    free(dec);

    /* manually specifies, seed and delimiter */
    if (check_seed_overlap_n_cipher("おうどん") != 0)
        return 3;

    if ((enc = encode_n_cipher(msg, "おうどん", "そば")) == NULL)
        return 4;
    
    if ((dec = decode_n_cipher(enc, "おうどん", "そば")) == NULL) {
        free(enc);

        return 5;
    }
    fprintf(stdout, "origin: %s\nencode: %s\ndecode: %s\nstrcmp: %d\n",
            msg, enc, dec, strcmp(msg, dec));
    free(enc);
    free(dec);

    return 0;
}
% gcc example.c -o example -lncipher
% ./example
origin: 戊辰War
encode: ゃぱににぱすぱ〜んゃぱすゃゃす〜ぱんん〜ぱすん〜すんす〜
decode: 戊辰War
strcmp: 0
origin: 戊辰War
encode: うどおどおおどどそばどおんんどんおおそばうううんそばうどおうそばうんおどそば
decode: 戊辰War
strcmp: 0
```


## Original

[844196/n_cipher](https://github.com/844196/n_cipher) - for Ruby


## License

[MIT](https://github.com/sasairc/libncipher/blob/master/LICENSE)


## Author

sasairc (https://github.com/sasairc)
