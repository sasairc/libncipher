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
#include "./table.h"
#include "./misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

int check_seed_overlap_n_cipher(char* seed)
{
    int         i       = 0,
                j       = 0,
                ret     = 0,
                decimal = 0;

    list_t*     t1      = NULL,
          *     t2      = NULL,
          *     start   = NULL;

    if (mbstrlen_without_byte(seed) < 1)
        return -1;

    if ((decimal = create_table(seed, &t1, &start)) == 0)
        return -2;

    t1 = start;
    while (i < decimal) {
        j = 0;
        t2 = start;
        while (j < decimal) {
            if (strcmp(t1->character, t2->character) == 0)
                ret++;

            t2 = t2->next;
            j++;
        }
        t1 = t1->next;
        i++;
    }
    free(start);

    return ret -= decimal;
}

char* encode_n_cipher(const char* string, char* seed, char* delimiter)
{
    if (string == NULL || seed == NULL || delimiter == NULL)
        return NULL;

    int         i       = 0,
                decimal = 0;

    size_t      x_bufl  = BUFLEN,
                delmlen = 0;

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

    /* allocate memory */
    if ((dest = (char*)malloc(sizeof(char) * x_bufl)) == NULL)
        goto ERR;
    else
        strcpy(dest, "\0");

    delmlen = strlen(delimiter);

    /* convert ucs4 to table */
    for (i = 0; i < length; i++) {
        if ((tmp = encode_table((unsigned int)cpoints[i], decimal, table, start)) == NULL)
            goto ERR;

        /* reallocate memory */
        if (x_bufl <= strlen(dest) + strlen(tmp) + delmlen) {
            x_bufl += BUFLEN;
            if ((dest = (char*)realloc(dest, sizeof(char) * x_bufl)) == NULL)
                goto ERR;
        }

        /* concat string */
        strcat(dest, tmp);
        strcat(dest, delimiter);

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
    if (tmp != NULL) {
        free(tmp);
        tmp = NULL;
    }
    if (dest != NULL) {
        free(dest);
        dest = NULL;
    }
    g_free(cpoints);
    release_table(start);

    return NULL;
}

char* decode_n_cipher(const char* string, char* seed, char* delimiter)
{
    int         decimal = 0;

    size_t      x_bufl  = BUFLEN,
                destlen = 0,
                blklen  = 0;

    char*       strtmp  = NULL,
        *       token   = NULL,
        *       dest    = NULL;

    gunichar    cpoint  = 0;
    gchar*      buf     = NULL;

    list_t*     table   = NULL,
          *     start   = NULL;
 
    /* convert seed to table */
    if ((decimal = create_table(seed, &table, &start)) == 0)
        return NULL;

    /* allocate memory */
    if ((buf = (gchar*)malloc(sizeof(gchar))) == NULL)
        goto ERR;

    if ((dest = (char*)malloc(sizeof(char) * x_bufl)) == NULL)
        goto ERR;
    else
        strcpy(dest, "\0");

    if ((strtmp = (char*)malloc(sizeof(char) * (strlen(string) + 1))) == NULL)
        goto ERR;
    else
        memcpy(strtmp, string, strlen(string));

    /*
     * decode
     */
    for (token = mbstrtok(strtmp, delimiter); token; token = mbstrtok(NULL, delimiter)) {
        /* get ucs4 codepint */
        cpoint = (gunichar)decode_table(token, decimal, table, start);

        /* convert ucs4 to character */
        blklen = g_unichar_to_utf8(cpoint, buf);

        /* concat character */
        if (x_bufl <= destlen + blklen + 1) {
            x_bufl += BUFLEN;
            if ((dest = (char*)realloc(dest, sizeof(char) * x_bufl)) == NULL)
                goto ERR;
        }
        memcpy(dest + destlen , buf, blklen);
        destlen += blklen;
    }

    /* release memory */
    if (buf != NULL) {
        free(buf);
        buf = NULL;
    }
    if (strtmp != NULL) {
        free(strtmp);
        buf = NULL;
    }
    release_table(start);

    return dest;

ERR:
    if (buf != NULL) {
        free(buf);
        buf = NULL;
    }
    if (dest != NULL) {
        free(dest);
        dest = NULL;
    }
    if (strtmp != NULL) {
        free(strtmp);
        buf = NULL;
    }
    release_table(start);

    return NULL;
}
