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

#include "./table.h"
#include "./misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

int create_table(char* seed, list_t** dest)
{
    int             i       = 0,
                    no      = 0;

    size_t          byte    = 0;

    list_t*         table   = NULL,
          *         start   = NULL;

    if (mbstrlen(seed) < 2)
        return -1;

    if ((table = (list_t*)
                malloc(sizeof(list_t))) == NULL) {
        fprintf(stderr, "%s: %d: create_table(): malloc(): %s\n",
                __FILE__, __LINE__, strerror(errno));

        return -2;
    } else {
        table->next = NULL;
        table->prev = NULL;
        start = table;
    }

    while (*seed != '\0') {
        /*
         * get character size
         */
        if ((byte = get_character_size((unsigned char)*seed)) < 0)
            goto ERR;

        /*
         * set table
         */
        table->number = no;
        if ((table->character = (char*)
                    malloc(sizeof(char) * (byte + 1))) == NULL) {
            fprintf(stderr, "%s: %d: create_table(): malloc(): %s\n",
                    __FILE__, __LINE__, strerror(errno));

            goto ERR;
        } else {
            i = 0;
            while (i < byte) {
                *(table->character + i) = *(seed + i);
                i++;
            }
            *(table->character + i) = '\0';
        }

        seed += byte;
        if (*seed != '\0') {
            if ((table->next = (list_t*)
                        malloc(sizeof(list_t))) == NULL) {
                fprintf(stderr, "%s: %d: create_table(): malloc(): %s\n",
                        __FILE__, __LINE__, strerror(errno));

                goto ERR;
            }
            table->next->prev = table;
            table = table->next;
            table->next = NULL;
        }
        no++;
    }
    *dest = start;

    return no;

ERR:
    release_table(start);

    return -3;
}

char* encode_table(int cpoint, int base, list_t* start, list_t* end)
{
    if (start == NULL || end == NULL)
        return NULL;

    int         y       = 0,
                fragmnt = 0;

    size_t      y_bufl  = BUFLEN,
                len     = 0;

    char*       p       = NULL,
        *       dest    = NULL,
        **      tmp     = NULL;

    list_t*     t1      = NULL,
          *     t2      = NULL;

    if ((tmp = (char**)
                malloc(sizeof(char*) * y_bufl)) == NULL) {
        fprintf(stderr, "%s: %d: encode_table(): malloc(): %s\n",
                __FILE__, __LINE__, strerror(errno));

        return NULL;
    }

    /*
     * conversion, decimal to any radix base number
     */
    while (cpoint > 0) {
        if (y_bufl <= y) {
            y_bufl += BUFLEN;
            if ((tmp = (char**)
                        realloc(tmp, sizeof(char*) * y_bufl)) == NULL) {
                fprintf(stderr, "%s: %d: encode_table(): realloc(): %s\n",
                        __FILE__, __LINE__, strerror(errno));

                goto ERR;
            }
        }
        fragmnt = cpoint % base;
        t1 = start;
        t2 = end;
        while (t1 != NULL && t2 != NULL) {
            if (fragmnt == t1->number) {
                p = t1->character;
                break;
            }
            if (fragmnt == t2->number) {
                p = t2->character;
                break;
            }
            t1 = t1->next;
            t2 = t2->prev;
        }
        *(tmp + y) = p;
        len += strlen(p);
        cpoint /= base;
        y++;
    }

    /*
     * concat string
     */
    if ((dest = (char*)
                malloc(sizeof(char) * (len + 1))) == NULL) {
        fprintf(stderr, "%s: %d: encode_table(): malloc(): %s\n",
                __FILE__, __LINE__, strerror(errno));

        goto ERR;
    } else {
        y--;
        len = 0;
        while (y >= 0) {
            memcpy(dest + len, *(tmp + y), strlen(*(tmp + y)) + 1);
            len = strlen(dest);
            y--;
        }
        *(dest + len) = '\0';
    }
    free(tmp);

    return dest;

ERR:
    if (tmp != NULL) {
        free(tmp);
        tmp = NULL;
    }
    if (dest != NULL) {
        free(dest);
        dest = NULL;
    }

    return NULL;
}

int decode_table(char* string, double base, list_t* start, list_t* end)
{
    int             i       = 0,
                    no      = 0;

    double          digit   = 0;

    size_t          byte    = 0,
                    sum     = 0;

    list_t*         t1      = NULL,
          *         t2      = NULL;

    digit = mbstrlen(string) - 1;
    while (*string != '\0') {
        /*
         * get character size
         */
        if ((byte = get_character_size((unsigned char)*string)) < 0)
            return -1;

        /*
         * search character
         */
        t1 = start;
        t2 = end;
        while (t1 != NULL && t2 != NULL) {
            i = 0;
            while (*(string + i) == *(t1->character + i))
                i++;
            if (i >= byte) {
                no = t1->number;
                break;
            }
            i = 0;
            while (*(string + i) == *(t2->character + i))
                i++;
            if (i >= byte) {
                no = t2->number;
                break;
            }
            if (t1 == t2) {
                break;
            }
            t1 = t1->next;
            t2 = t2->prev;
        }
        /* invalid string */
        if (t1 == t2 && i < byte)
            return -2;

        /*
         * conversion, any radix base number to decimal
         */
        sum += no * (int)pow((double)base, (double)digit);
        string += byte;
        digit--;
    }

    return sum;
}

list_t* seek_table_end(list_t* start)
{
    list_t* t1  = start;

    while (t1->next != NULL)
        t1 = t1->next;

    return t1;
}

void release_table(list_t* table)
{
    list_t* tmp = NULL;

    while (table != NULL) {
        tmp = table->next;

        if (table->character != NULL)
            free(table->character);
        free(table);

        table = tmp;
    }

    return;
}
