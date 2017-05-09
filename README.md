libncipher
===

[![version](http://img.shields.io/github/tag/sasairc/libncipher.svg?style=flat&label=version)](https://github.com/sasairc/libncipher/releases)
[![license](https://img.shields.io/badge/License-MIT-blue.svg?style=flat)](https://raw.githubusercontent.com/sasairc/libncipher/master/LICENSE)
[![issues](http://img.shields.io/github/issues/sasairc/libncipher.svg?style=flat)](https://github.com/sasairc/libncipher/issues)
[![language](https://img.shields.io/badge/language-C%2FC%2B%2B-lightgrey.svg)](https://github.com/sasairc/libncipher/blob/master/src/n_cipher.h)
[![build](https://img.shields.io/travis/sasairc/libncipher.svg?style=flat)](https://travis-ci.org/sasairc/libncipher)

C/C++向け[Nyanpasu Cipher (N-Cipher)](https://github.com/844196/n_cipher)ライブラリ。	
seed値は128文字まで確認済みです。


## Requirements

### libncipher

* GNU Make (gmake)
* gcc or clang/llvm
* pkg-config >= 0.26
* GLib >= 2.0

### Sample program

* libncipher >= 2.1.0

## Install

```shellsession
% make
# make install
% make shared-sample    # optional
% cp command/sample ~/local/bin/n_cipher_sample # optional
```


## Usage

```c
#include <n_cipher.h>

#define SEED        "にゃんぱす\0" /* default seed */
#define DELIMITER   "〜\0"         /* default delimiter */

/*
 * return value of check_argument()
 */
#define S_TOO_SHORT -1  /* seed too short */
#define S_TOO_LONG  -2  /* seed too long */
#define D_TOO_SHORT -3  /* delimiter too long */

struct TABLE {
    int             decimal;
    struct _LIST_T* start;	/* table.h */
};

typedef struct N_CIPHER {
    char*   seed;
    char*   delimiter;
    int     (*check_argument)(const char* seed, const char* delimiter);
    int     (*config)(struct N_CIPHER** n_cipher, const char* seed, const char* delimiter);
    int     (*ready)(struct N_CIPHER** n_cipher);
    char*   (*encode)(struct N_CIPHER** n_cipher, const char* string);
    char*   (*decode)(struct N_CIPHER** n_cipher, const char* string);
    char*   (*version)(void);
    void    (*release)(struct N_CIPHER* n_cipher);
    struct TABLE* table;
} N_CIPHER;

int init_n_cipher(N_CIPHER** n_cipher);
```

### int init_n_cipher(N_CIPHER\*\* n_cipher)

`N_CIPHER`の初期化処理を行います。戻り値は、成功の場合は0、失敗の場合は負の整数です。

### N_CIPHER

#### int check_argument(const char\* seed, const char\* delimiter)

seed値および、delimiter値の有効性をチェックします。ここでの有効な値とは、文字集合において重複文字が存在せず、seed値は2文字かそれ以上、delimiter値は1文字かそれ以上の文字列の事を指します。
戻り値は、成功の場合は0、重複する文字がある場合は正の整数、その他のエラーの場合は負の整数です。

#### int config(N_CIPHER\*\* n_cipher, const char\* seed, const char\* delimiter)

seed値ならびに、delimiter値のセット(設定)を行います。引数として`NULL`ポインタが与えられた場合、デフォルトの値がセットされます。戻り値は、成功の場合は0、失敗の場合は負の整数です。

#### int ready(N_CIPHER\*\* n_cipher)

`N_CIPHER`が設定済みで、暗号化および復号化を行える状態かを確かめる関数です。戻り値は、成功の場合は0、失敗の場合は負の整数です。

#### char\* encode(N_CIPHER\*\* n_cipher, const char\* string)

N暗号で暗号化を行います。成功した場合の戻り値は、暗号化された文字列(配列)の先頭アドレスであり、失敗の場合は`NULL`ポインタを返却します。また、返却されたアドレスが指すメモリ領域は適宜解放して下さい。

#### char\* decode(N_CIPHER\*\* n_cipher, const char\* string)

N暗号の復号化を行います。成功した場合の戻り値は、復号化された文字列(配列)の先頭アドレスであり、失敗の場合は`NULL`ポインタを返却します。また、返却されたアドレスが指すメモリ領域は適宜解放して下さい。

#### char\* version(void)

戻り値としてlibncipherのバージョン情報文字列(配列)の先頭アドレスを返却します。

#### void release(N_CIPHER\* n_cipher)

`N_CIPHER`のメモリ解放を行います。

-

詳しい説明や追加情報については`libncipher.3`をご覧下さい。

## Example

利用法はC/C++共に共通です。ここでは、凡例としてCによるサンプルコードを掲載します。

### Source code

```c
/*
 * example.c
 */

#include <n_cipher.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    char*       msg         = "サンプルテキストって何にするか結構悩むよね…\0",
        *       enc         = NULL,
        *       dec         = NULL;

    N_CIPHER*   n_cipher    = NULL;

    /* initialize n_cipher */
    init_n_cipher(&n_cipher);

    /*
     * configure default seed and delimiter
     */
    n_cipher->config(&n_cipher, NULL, NULL);

    /* encode */
    if ((enc = n_cipher->encode(&n_cipher, msg)) == NULL)
        goto ERR;
    
    /* decode */
    if ((dec = n_cipher->decode(&n_cipher, enc)) == NULL)
        goto ERR;

    /* print result */
    fprintf(stdout, "origin: %s\nencode: %s\ndecode: %s\nstrcmp: %d\n",
            msg, enc, dec, strcmp(msg, dec));
    free(enc);
    enc = NULL;
    free(dec);
    dec = NULL;

    /*
     * reconfigure, manually specifies, seed and delimiter
     */
    fprintf(stdout, "\n*** checking seed / delimiter ***\n\
good = %d (おうどん)\n\
bad  = %d (てんぷらうどん)\n\
bad  = %d (うどん, たんめん)\n\
bad  = %d (ん)\n\n",
            n_cipher->check_argument("おうどん", NULL),
            n_cipher->check_argument("てんぷらうどん", NULL),
            n_cipher->check_argument("うどん", "たんめん"),
            n_cipher->check_argument("ん", NULL));

    n_cipher->config(&n_cipher, "おうどん", "そば");

    /* encode */
    if ((enc = n_cipher->encode(&n_cipher, msg)) == NULL)
        goto ERR;

    /* decode */
    if ((dec = n_cipher->decode(&n_cipher, enc)) == NULL)
        goto ERR;

    /* print result */
    fprintf(stdout, "origin: %s\nencode: %s\ndecode: %s\nstrcmp: %d\n",
            msg, enc, dec, strcmp(msg, dec));
    free(enc);
    free(dec);

    /* release n_cipher */
    n_cipher->release(n_cipher);

    return 0;

ERR:
    if (enc != NULL)
        free(enc);
    if (dec != NULL)
        free(dec);

    n_cipher->release(n_cipher);

    return 1;
}
```

### Result

```shellsession
% gcc example.c -o example -lncipher
% ./example
origin: サンプルテキストって何にするか結構悩むよね…
encode: ぱすすぱぱす〜すににゃゃゃ〜すににににぱ〜すにににすぱ〜ぱすすすんゃ〜ぱすすぱんゃ〜ぱすすぱすぱ〜ぱすすすんぱ〜ぱすすにんん〜ぱすすにぱに〜ゃゃんんんゃす〜ぱすすにすに〜ぱすすににん〜ぱすすんにん〜ぱすぱすんぱ〜んにゃゃぱゃに〜ゃぱぱゃぱゃぱ〜ゃんすんすすに〜ぱすすゃぱゃ〜ぱすすゃすす〜ぱすすにすん〜んぱにすゃに〜
decode: サンプルテキストって何にするか結構悩むよね…
strcmp: 0

*** checking seed / delimiter ***
good = 0 (おうどん)
bad  = 2 (てんぷらうどん)
bad  = 2 (うどん, たんめん)
bad  = -1 (ん)

origin: サンプルテキストって何にするか結構悩むよね…
encode: んおおどんううそばんおおんんおんそばんおおんううんそばんおおんどどんそばんおおんおうどそばんおおどどんうそばんおおどんどうそばんおおんおどおそばんおおうどおんそばんおおうどうどそばうおんんううううそばんおおうどどんそばんおおううどうそばんおおどおどんそばんおおうおどんそばうんんうううおおそばうどどうんおどんそばうどおおどどどうそばんおおどおおおそばんおおどおどおそばんおおうどんうそばどおおおどうどそば
decode: サンプルテキストって何にするか結構悩むよね…
strcmp: 0
```

## Original

[844196/n_cipher](https://github.com/844196/n_cipher) - for Ruby

[844196/neo_ncipher](https://github.com/844196/neo_ncipher) - for Go

## Documentation

[844196/ncp_spec](https://github.com/844196/ncp_spec) - Nyanpasu Cipher (N-Cipher) specification

## License

[MIT](https://github.com/sasairc/libncipher/blob/master/LICENSE)


## Author

sasairc (https://github.com/sasairc)		

Masaya Tk (https://github.com/844196) - ncp_spec
