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

#include "./n_cipher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <math.h>
#include <glib.h>

int strrep(char* src, char* haystack, char* needle)
{
    char*   find = NULL;

    if (src == NULL || haystack == NULL || needle == NULL) {

        return 1;
    }

    /* seach strings */
    if ((find = strstr(src, haystack)) == NULL) {

        return 2;       /* word not found */
    }
    if (strlen(haystack) < strlen(needle)) {
        /* reallocate memory */
        if ((src = (char*)realloc(
                        src,
                        strlen(src) + strlen(needle) + 1 - strlen(haystack))
            ) == NULL) {

            return 3;
        }
        /* move match word to specified location in memory */
        memmove(
            find + strlen(needle),
            find + strlen(haystack),
            strlen(src) - strlen(haystack) - (find - src) + 1
        );
        memcpy(find, haystack, strlen(needle));
    } else {
        memcpy(find, needle, strlen(needle));
        /* move match word to specified location in memory */
        if (strlen(haystack) > strlen(needle)) {
            memmove(
                find + strlen(needle),
                find + strlen(haystack),
                strlen(src) - strlen(haystack) - (find - src) + 1
            );
        }
    }

    return 0;
}

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

int create_table(char* seed, list_t** dest_table, list_t** dest_start)
{
    int         i       = 0,
                code    = 0;
    list_t*     table   = NULL,
          *     start;

    if (mbstrlen(seed) < 2 || mbstrlen(seed) > 36)
        return 0;

    if ((table = malloc(sizeof(list_t))) == NULL)
        return 0;
    else
        start = table;

    while (*seed != '\0') {
        code = (unsigned char)*seed;

        if (code < 0x80) {
            table->next = malloc(sizeof(list_t));
            table = table->next;

            table->number = i;
            table->character = (char*)malloc(2 * sizeof(char));

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
            table->next = malloc(sizeof(list_t));
            table = table->next;

            table->number = i;
            table->character = (char*)malloc(3 * sizeof(char));

            sprintf(table->character, "%c%c", *seed, *(seed + 1));

            table->next = NULL;
            if (i == 0)
                start = table;

            i++;
            seed += 3;
        } else if (code < 0xF0) {
            table->next = malloc(sizeof(list_t));
            table = table->next;

            table->number = i;
            table->character = (char*)malloc(4 * sizeof(char));

            sprintf(table->character, "%c%c%c",
                    *seed, *(seed + 1), *(seed + 2));

            table->next = NULL;
            if (i == 0)
                start = table;

            i++;
            seed += 3;
        } else {
            table->next = malloc(sizeof(list_t));
            table = table->next;

            table->number = i;
            table->character = (char*)malloc(5 * sizeof(char));

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

char* encode_table(unsigned int number, int base, list_t* table, list_t* start)
{
    if (table == NULL || start == NULL)
        return NULL;

    int     i       = 0,
            j       = 0,
            y       = 0;
    size_t  size    = 0;
    char*   dest    = NULL,
        **  tmp     = NULL;

    if ((tmp = (char**)malloc(sizeof(char*) * 64)) == NULL)
        return NULL;

    while (number > 0) {
        j = number % base;

        table = start;
        while (j != table->number)
            table = table->next;

        tmp[y] = table->character;
        size += strlen(table->character);

        number /= base;
        y++;
    }
    y--;
    dest = (char*)malloc(sizeof(char) * size);
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
}

long decode_table(char* string, int base, list_t* table, list_t* start)
{
    if (string == NULL || string == NULL)
        return 0;

    long    sum     = 0;
    int     i       = 0,
            j       = 0,
            code    = 0;
    char**  tmp     = NULL;

    tmp = (char**)malloc(sizeof(char*) * 128);
    while (*string != '\0') {
        int code = (unsigned char)*string;

        if (code < 0x80) {
            tmp[i] = (char*)malloc(sizeof(char) * 2);
            sprintf(tmp[i], "%c", *string);
            i++;
            string += 1;
        } else if (code < 0xC0) {
            string += 1;
        } else if (code < 0xE0) {
            tmp[i] = (char*)malloc(sizeof(char) * 3);
            sprintf(tmp[i], "%c%c", *string, *(string + 1));
            i++;
            string += 2;
        } else if (code < 0xF0) {
            tmp[i] = (char*)malloc(sizeof(char) * 4);
            sprintf(tmp[i], "%c%c%c", *string, *(string + 1), *(string + 2));
            i++;
            string += 3;
        } else {
            tmp[i] = (char*)malloc(sizeof(char) * 5);
            sprintf(tmp[i], "%c%c%c%c",
                    *string, *(string + 1), *(string + 2), *(string + 3));
            i++;
            string += 4;
        }
    }
    i -= 1;

    for (j = 0; i >= 0; i--, j++) {
        table = start;

        while(strstr(tmp[i], table->character) == NULL)
            table = table->next;

        sum += table->number * (double)pow((double)base, (double)j);
        free(tmp[i]);
    }
    free(tmp);

    return sum;
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
        tmp = encode_table((unsigned int)cpoints[i], decimal, table, start);
        if (i == 0) {
            dest = (char*)malloc(sizeof(char) * (strlen(tmp) + strlen(delimiter) + 1));
            strcpy(dest, tmp);
            strcat(dest, delimiter);
        } else {
            dest = (char*)realloc(
                    dest,
                    sizeof(char) * (strlen(dest) + strlen(tmp) + strlen(delimiter) + 1)
                    );
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
}

char* decode_n_cipher(char* string, char* seed, char* delimiter)
{
    int         decimal = 0;
    gunichar    cpoints = 0;
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

    strtmp = (char*)malloc(sizeof(char) * (strlen(string) + strlen(delimiter)));
    strcpy(strtmp, string);

    while (strrep(strtmp, delimiter, " ") == 0);

    token = strtok(strtmp, " ");
    buf = (gchar*)malloc(sizeof(gchar));
    dest = (char*)malloc(sizeof(char) * 640);
    strcpy(dest, "");

    while (token != NULL) {
        cpoints = (gunichar)decode_table(token, decimal, table, start);
        c_size = g_unichar_to_utf8(cpoints, buf);

        sprintf(dest, "%s%.*s", dest, c_size, buf);

        token = strtok(NULL, " ");
    }

    /* release memory */
    free(buf);
    free(strtmp);
    release_table(start);

    return dest;
}
