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
#include <stdlib.h>
#include <string.h>
#include <math.h>

int create_table(char* seed, list_t** dest_table, list_t** dest_start)
{
    int         i       = 0,
                j       = 0,
                code    = 0;
    size_t      byte    = 0;

    list_t*     table   = NULL,
          *     start   = NULL;

    if (mbstrlen_without_byte(seed) < 1)
        return 0;

    if ((table = malloc(sizeof(list_t))) == NULL)
        return 0;
    else
        start = table;

    while (*seed != '\0') {
        code = (unsigned char)*seed;

        if (code < 0x80) {
            byte = 1;
        } else if (code < 0xC0) {
            seed += 1;
            continue;
        } else if (code < 0xE0) {
            byte = 2;
        } else if (code < 0xF0) {
            byte = 3;
        } else {
            byte = 4;
        }

        if ((table->next = malloc(sizeof(list_t))) == NULL)
            goto ERR;

        table = table->next;
        table->number = i;

        if ((table->character = (char*)
                    malloc(sizeof(char) * (byte + 1))) == NULL)
            goto ERR;

        j = 0;
        while (j < byte) {
            *(table->character + j) = *(seed + j);
            j++;
        }
        *(table->character + j) = '\0';

        table->next = NULL;
        if (i == 0)
            start = table;

        i++;
        seed += byte;
    }
    *dest_start = start;
    *dest_table = table;

    return i;

ERR:
    release_table(start);

    return 0;
}

char* encode_table(int cpoint, int base, list_t* table, list_t* start)
{
    if (table == NULL || start == NULL)
        return NULL;

    int     y       = 0,
            fragmnt = 0;

    size_t  y_bufl  = BUFLEN,
            destlen = 0;

    char*   dest    = NULL,
        **  tmp     = NULL;

    if ((tmp = (char**)
                malloc(sizeof(char*) * y_bufl)) == NULL)
        return NULL;

    while (cpoint > 0) {
        if (y_bufl <= y) {
            y_bufl += BUFLEN;
            if ((tmp = (char**)
                        realloc(tmp, sizeof(char*) * y_bufl)) == NULL)
                goto ERR;
        }

        fragmnt = cpoint % base;

        table = start;
        while (fragmnt != table->number)
            table = table->next;

        tmp[y] = table->character;
        destlen += strlen(table->character);

        cpoint /= base;
        y++;
    }

    if ((dest = (char*)
                malloc(sizeof(char) * (destlen + 1))) == NULL)
        goto ERR;
    else
        y -= 2;

    strcpy(dest, tmp[y + 1]);
    while (y >= 0) {
        strcat(dest, tmp[y]);
        y--;
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

int decode_table(char* string, int base, list_t* table, list_t* start)
{
    int     i       = 0,
            digit   = 0,
            code    = 0;

    size_t  byte    = 0,
            sum     = 0;

    digit = mbstrlen_without_byte(string);
    while (*string != '\0') {
        i = byte = 0;
        code = (unsigned char)*string;

        if (code < 0x80) {
            byte = 1;
        } else if (code < 0xC0) {
            string += 1;
            continue;
        } else if (code < 0xE0) {
            byte = 2;
        } else if (code < 0xF0) {
            byte = 3;
        } else {
            byte = 4;
        }

        table = start;
        while (table->next != NULL) {
            while (*(string + i) == *(table->character + i))
                i++;

            if (i >= byte)
                break;
            else
                table = table->next;
        }
        sum += table->number * (int)pow((double)base, (double)digit);
        string += byte;
        digit--;
    }

    return sum;
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
