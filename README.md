libncipher
===

[![license](https://img.shields.io/badge/License-MIT-blue.svg?style=flat)](https://raw.githubusercontent.com/sasairc/libncipher/master/LICENSE)
[![build](https://img.shields.io/travis/sasairc/libncipher.svg?style=flat)](https://travis-ci.org/sasairc/libncipher)

C/C++向け[n_cipher](https://github.com/844196/n_cipher)（にゃんぱす暗号）ライブラリ。


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

char* encode_n_cipher(const char* string, char* seed, char* delimiter);
char* decode_n_cipher(const char* string, char* seed, char* delimiter);
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
    if ((enc = encode_n_cipher(msg, "おうどん", "そば")) == NULL)
        return 3;
    
    if ((dec = decode_n_cipher(enc, "おうどん", "そば")) == NULL) {
        free(enc);

        return 4;
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


## License

[MIT](https://github.com/sasairc/libncipher/blob/master/LICENSE)


## Author

sasairc (https://github.com/sasairc)
