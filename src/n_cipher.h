/*
 * libncipher - n_cipher library for C.
 *
 * Copyright (c) 2015 sasairc
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef N_CIPHER_H
#define N_CIPHER_H
#ifdef  __cplusplus
extern "C" {
/* __cplusplus */
#endif

#define SEED        "にゃんぱす\0" /* default seed */
#define DELIMITER   "〜\0"         /* default delimiter */

/*
 * return value of check_argument()
 */
#define S_TOO_SHORT -1  /* seed too short */
#define S_TOO_LONG  -2  /* seed too long */
#define D_TOO_SHORT -3  /* delimiter too long */

typedef struct N_CIPHER {
    char*   seed;
    char*   delimiter;
    int     (*check_argument)(const char* seed, const char* delimiter);
    int     (*config)(struct N_CIPHER** n_cipher, const char* seed, const char* delimiter);
    char*   (*encode)(struct N_CIPHER** n_cipher, const char* string);
    char*   (*decode)(struct N_CIPHER** n_cipher, const char* string);
    char*   (*version)(void);
    void    (*release)(struct N_CIPHER* n_cipher);
} N_CIPHER;

extern int init_n_cipher(N_CIPHER** n_cipher);

#ifdef __cplusplus
}
/* __cplusplus */
#endif
/* N_CIPHER_H */
#endif
