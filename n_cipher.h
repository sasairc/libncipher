/*
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

#define LOCALE      ""

/* default */
#define SEED        "にゃんぱす"
#define DELIMITER   "〜"

typedef struct _LIST_T {
    int     number;
    char*   character;
    struct  _LIST_T*    next;
} list_t;

extern int strrep(char* src, char* haystack, char* needle);
extern int mbstrlen(char* src);
extern int create_table(char* seed, list_t** dest_table, list_t** dest_start);
extern void release_table(list_t* table);
extern char* encode_table(unsigned int number, int base, list_t* table, list_t* start);
extern long decode_table(char* string, int base, list_t* table, list_t* start);
extern char* encode_n_cipher(char* string, char* seed, char* delimiter);
extern char* decode_n_cipher(char* string, char* seed, char* delimiter);

#endif
