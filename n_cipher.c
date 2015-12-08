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

#include "./n_cipher.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <math.h>
#include <glib.h>

int mbstrlen(char* src);
char* mbstrtok(char* str, char* delimiter);

int mbstrlen(char* src)
{
    int         i   = 0,
                ch  = 0,
                len = 0;
    gunichar*   cpoints;

    setlocale(LC_CTYPE, LOCALE);

    while (src[i] != '\0') {
        /* get string length */
        if ((ch = mblen(&src[i], MB_CUR_MAX)) < 0)
            return 0;

        if (ch > 1) {
            cpoints = g_utf8_to_ucs4_fast(&src[i], sizeof(src[i]), NULL);

            /*
             * multi byte
             * true : hankaku kana
             * false: other
             */
            if (cpoints[0] >= 0xff65 && cpoints[0] <= 0xff9f) {
                len++;
            } else {
                len += 2;
            }

            g_free(cpoints);
        } else {
            len++;
        }
        i += ch;
    }

    return len;
}

char* mbstrtok(char* str, char* delimiter)
{
    static  char*   ptr = NULL;
            char*   bdy = NULL;

    if (!str)
        str = ptr;

    if (!str)
        return NULL;

    if ((bdy = strstr(str, delimiter)) != NULL) {
        *bdy = '\0';
        ptr = bdy + strlen(delimiter);
    } else {
        ptr = NULL;
    }

    return str;
}

int create_table(char* seed, list_t** dest_table, list_t** dest_start)
{
    int         i       = 0,
                code    = 0;
    list_t*     table   = NULL,
          *     start   = NULL;

    if (mbstrlen(seed) < 2 || mbstrlen(seed) > 36)
        return 0;

    if ((table = malloc(sizeof(list_t))) == NULL)
        return 0;
    else
        start = table;

    while (*seed != '\0') {
        code = (unsigned char)*seed;

        if (code < 0x80) {
            if ((table->next = malloc(sizeof(list_t))) == NULL)
                goto ERR;

            table = table->next;
            table->number = i;

            if ((table->character = (char*)malloc(2 * sizeof(char))) == NULL)
                goto ERR;

            sprintf(table->character, "%c",
                    *seed);

            table->next = NULL;
            if (i == 0)
                start = table;

            i++;
            seed += 1;
        } else if (code < 0xC0) {
            seed += 1;
        } else if (code < 0xE0) {
            if ((table->next = malloc(sizeof(list_t))) == NULL)
                goto ERR;

            table = table->next;
            table->number = i;

            if ((table->character = (char*)malloc(3 * sizeof(char))) == NULL)
                goto ERR;

            sprintf(table->character, "%c%c",
                    *seed, *(seed + 1));

            table->next = NULL;
            if (i == 0)
                start = table;

            i++;
            seed += 3;
        } else if (code < 0xF0) {
            if ((table->next = malloc(sizeof(list_t))) == NULL)
                goto ERR;

            table = table->next;
            table->number = i;

            if ((table->character = (char*)malloc(4 * sizeof(char))) == NULL)
                goto ERR;

            sprintf(table->character, "%c%c%c",
                    *seed, *(seed + 1), *(seed + 2));

            table->next = NULL;
            if (i == 0)
                start = table;

            i++;
            seed += 3;
        } else {
            if ((table->next = malloc(sizeof(list_t))) == NULL)
                goto ERR;

            table = table->next;
            table->number = i;

            if ((table->character = (char*)malloc(5 * sizeof(char))) == NULL)
                goto ERR;

            sprintf(table->character, "%c%c%c%c",
                    *seed, *(seed + 1), *(seed + 2), *(seed + 3));

            table->next = NULL;
            if (i == 0)
                start = table;

            i++;
            seed += 4;
        }
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
            fragmnt = 0,
            y_bufl  = BUFLEN;
    size_t  size    = 0;
    char*   dest    = NULL,
        **  tmp     = NULL;

    if ((tmp = (char**)malloc(sizeof(char*) * y_bufl)) == NULL)
        return NULL;

    while (cpoint > 0) {
        if (y > y_bufl) {
            y_bufl += BUFLEN;
            if ((tmp = (char**)realloc(tmp, sizeof(char*) * y_bufl)) == NULL)
                goto ERR;
        }

        fragmnt = cpoint % base;

        table = start;
        while (fragmnt != table->number)
            table = table->next;

        tmp[y] = table->character;
        size += strlen(table->character);

        cpoint /= base;
        y++;
    }
    y--;

    if ((dest = (char*)malloc(sizeof(char) * size)) == NULL)
        goto ERR;
    else
        strcpy(dest, tmp[y]);

    y--;
    while (y >= 0) {
        strcat(dest, tmp[y]);
        y--;
    }
    if (tmp != NULL) {
        free(tmp);
        tmp = NULL;
    }

    return dest;

ERR:
    while (y >= 0) {
        if (tmp[y] != NULL) {
            free(tmp[y]);
            tmp[y] = NULL;
            y--;
        }
    }
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
    if (string == NULL || string == NULL || strlen(string) == 0)
        return 0;

    int     i       = 0,
            j       = 0,
            sum     = 0,
            code    = 0,
            y_bufl  = BUFLEN;
    char**  tmp     = NULL;

    if ((tmp = (char**)malloc(sizeof(char*) * y_bufl)) == NULL)
        return 0;

    while (*string != '\0') {
        if (i > y_bufl) {
            y_bufl += BUFLEN;
            if ((tmp = (char**)realloc(tmp, sizeof(char*) * y_bufl)) == NULL)
                goto ERR;
        }

        code = (unsigned char)*string;
        if (code < 0x80) {
            if ((tmp[i] = (char*)malloc(sizeof(char) * 2)) == NULL)
                goto ERR;

            sprintf(tmp[i], "%c",
                    *string);
            i++;
            string += 1;
        } else if (code < 0xC0) {
            string += 1;
        } else if (code < 0xE0) {
            if ((tmp[i] = (char*)malloc(sizeof(char) * 3)) == NULL)
                goto ERR;

            sprintf(tmp[i], "%c%c",
                    *string, *(string + 1));
            i++;
            string += 2;
        } else if (code < 0xF0) {
            if ((tmp[i] = (char*)malloc(sizeof(char) * 4)) == NULL)
                goto ERR;

            sprintf(tmp[i], "%c%c%c",
                    *string, *(string + 1), *(string + 2));
            i++;
            string += 3;
        } else {
            if ((tmp[i] = (char*)malloc(sizeof(char) * 5)) == NULL)
                goto ERR;

            sprintf(tmp[i], "%c%c%c%c",
                    *string, *(string + 1), *(string + 2), *(string + 3));
            i++;
            string += 4;
        }
    }
    i -= 1;

    for (j = 0; i >= 0; i--, j++) {
        table = start;

        while(strstr(tmp[i], table->character) == NULL && table->next != NULL)
            table = table->next;

        sum += table->number * (int)pow((double)base, (double)j);
        free(tmp[i]);
    }
    free(tmp);

    return sum;

ERR:
    while (i >= 0) {
        if (tmp[i] != NULL) {
            free(tmp[i]);
            tmp[i] = NULL;
        }
        i--;
    }
    if (tmp != NULL) {
        free(tmp);
        tmp = NULL;
    }
    
    return 0;
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

char* encode_n_cipher(char* string, char* seed, char* delimiter)
{
    if (string == NULL || seed == NULL || delimiter == NULL)
        return NULL;

    int         i       = 0,
                decimal = 0;
    char*       dest    = NULL,
        *       tmp     = NULL;

    glong       length  = 0;
    gunichar*   cpoints = NULL;

    list_t*     table   = NULL,
          *     start   = NULL;

    /* convert seed to table */
    if ((decimal = create_table(seed, &table, &start)) == 0)
        return NULL;

    /* get string length */
    length = g_utf8_strlen(string, -1);

    /* convert string to ucs4 */
    cpoints = g_utf8_to_ucs4_fast(string, -1, NULL);

    /* convert ucs4 to table */
    for (i = 0; i < length; i++) {
        if ((tmp = encode_table((unsigned int)cpoints[i], decimal, table, start)) == NULL)
            goto ERR;

        if (i == 0) {
            if ((dest = (char*)malloc(sizeof(char) * (strlen(tmp) + strlen(delimiter) + 1))) == NULL)
                goto ERR;

            strcpy(dest, tmp);
            strcat(dest, delimiter);
        } else {
            if ((dest = (char*)realloc(
                            dest, sizeof(char) * (strlen(dest) + strlen(tmp) + strlen(delimiter) + 1)
                        )) == NULL)
                goto ERR;

            strcat(dest, tmp);
            strcat(dest, delimiter);
        }
        if (tmp != NULL) {
            free(tmp);
            tmp = NULL;
        }
    }

    /* release memory */
    g_free(cpoints);
    release_table(start);

    return dest;

ERR:
    if (dest != NULL) {
        free(dest);
        dest = NULL;
    }
    if (tmp != NULL) {
        free(tmp);
        tmp = NULL;
    }
    g_free(cpoints);
    release_table(start);

    return NULL;
}

char* decode_n_cipher(char* string, char* seed, char* delimiter)
{
    if (string == NULL || seed == NULL || delimiter == NULL)
        return NULL;

    int         decimal = 0,
                x_bufl  = BUFLEN;
    gunichar    cpoint  = 0;
    char*       strtmp  = NULL,
        *       token   = NULL,
        *       dest    = NULL;
    gchar*      buf     = NULL;
    list_t*     table   = NULL,
          *     start   = NULL;
    size_t      c_size  = 0;

    /* convert seed to table */
    if ((decimal = create_table(seed, &table, &start)) == 0)
        return NULL;

    if ((strtmp = (char*)malloc(sizeof(char) * (strlen(string) + strlen(delimiter)))) == NULL)
        goto ERR;
    else
        strcpy(strtmp, string);

    if ((buf = (gchar*)malloc(sizeof(gchar))) == NULL)
        goto ERR;

    if ((dest = (char*)malloc(sizeof(char) * x_bufl)) == NULL)
        goto ERR;
    else
        strcpy(dest, "");

    /*
     * decode
     */
    for (token = mbstrtok(strtmp, delimiter); token; token = mbstrtok(NULL, delimiter)) {
        if (strlen(dest) < x_bufl) {
            x_bufl += BUFLEN;
            if ((dest = (char*)realloc(dest, sizeof(char) * x_bufl)) == NULL)
                goto ERR;
        }

        /* get ucs4 codepint */
        cpoint = (gunichar)decode_table(token, decimal, table, start);

        /* convert ucs4 to character */
        c_size = g_unichar_to_utf8(cpoint, buf);

        /* concat character */
        sprintf(dest, "%s%.*s", dest, c_size, buf);
    }

    /* release memory */
    if (buf != NULL) {
        free(buf);
        buf = NULL;
    }
    if (strtmp != NULL) {
        free(strtmp);
        strtmp = NULL;
    }
    release_table(start);

    return dest;

ERR:
    if (strtmp != NULL) {
        free(strtmp);
        strtmp = NULL;
    }
    if (buf != NULL) {
        free(buf);
        buf = NULL;
    }
    if (dest != NULL) {
        free(dest);
        dest = NULL;
    }
    release_table(start);

    return NULL;
}
