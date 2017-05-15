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
#include <errno.h>

#define LIBNAME         "libncipher"
#define SEED_MAX_LEN    128

static int check_argument_n_cipher(const char* seed, const char* delimiter);
static int config_n_cipher(N_CIPHER** n_cipher, const char* seed, const char* delimiter);
static int ready_n_cipher(N_CIPHER** n_cipher);
static char* encode_n_cipher(N_CIPHER** n_cipher, const char* string);
static char* decode_n_cipher(N_CIPHER** n_cipher, const char* string);
static char* version_n_cipher(void);
static void release_n_cipher(N_CIPHER* n_cipher);

int init_n_cipher(N_CIPHER** n_cipher)
{
    N_CIPHER*   nc  = NULL;

    if ((nc = (N_CIPHER*)
                malloc(sizeof(N_CIPHER))) == NULL) {
        fprintf(stderr, "%s: %s: %d: init_n_cipher(): malloc(): %s\n",
                LIBNAME, __FILE__, __LINE__, strerror(errno));

        return -1;
    }

    /*
     * config variable & functions
     */
    nc->seed            = NULL;
    nc->delimiter       = NULL;
    nc->table           = NULL;
    nc->check_argument  = check_argument_n_cipher;
    nc->config          = config_n_cipher;
    nc->ready           = ready_n_cipher;
    nc->encode          = encode_n_cipher;
    nc->decode          = decode_n_cipher;
    nc->version         = version_n_cipher;
    nc->release         = release_n_cipher;

    *n_cipher = nc;

    return 0;
}   

static
int check_argument_n_cipher(const char* seed, const char* delimiter)
{
    int         i       = 0,
                j       = 0,
                ret     = 0,
                decimal = 0;

    char*       s       = SEED,         /* default seed */
        *       d       = DELIMITER,    /* default delimiter */
        *       d2      = NULL;

    list_t*     t1      = NULL,
          *     t2      = NULL,
          *     start   = NULL;

    /*
     * specfiles arguments?
     */
    if (seed != NULL) {
        s = (char*)seed;
        if (mbstrlen(s) < 2)
            return S_TOO_SHORT;
        if (mbstrlen(s) > SEED_MAX_LEN)
            return S_TOO_LONG;
    }
    if (delimiter != NULL) {
        d = (char*)delimiter;
        if (mbstrlen(d) < 1)
            return D_TOO_SHORT;
    }

    /*
     * create seed table
     */
    if ((decimal = create_table(s, &start)) < 0)
        return -5;

    /*
     * check seed
     */
    t1 = start;
    while (i < decimal) {
        j = 0;
        t2 = start;
        while (j < decimal) {
            if (strcmp_lite(t1->character, t2->character) == 0)
                ret++;
            t2 = t2->next;
            j++;
        }
        t1 = t1->next;
        i++;
    }
    ret -= decimal;

    /*
     * check delimiter
     */
    d2 = d;
    i = j = 0;
    t1 = start;
    while (t1 != NULL) {
        d = d2;
        while (*d != '\0') {
            j = mblen(d, MB_CUR_MAX);
            if (memcmp(t1->character, d, j) == 0)
                ret++;
            d += j;
        }
        t1 = t1->next;
        i++;
    }
    release_table(start);

    return ret;
}

static
int config_n_cipher(N_CIPHER** n_cipher, const char* seed, const char* delimiter)
{
    if (n_cipher == NULL)
        return -1;

    char*   s   = SEED,         /* default seed */
        *   d   = DELIMITER;    /* default delimiter */

    /*
     * release exist seed & delimiter
     */
    if ((*n_cipher)->seed != NULL) {
        free((*n_cipher)->seed);
        (*n_cipher)->seed = NULL;
    }
    if ((*n_cipher)->delimiter != NULL) {
        free((*n_cipher)->delimiter);
        (*n_cipher)->delimiter = NULL;
    }
    if ((*n_cipher)->table != NULL) {
        release_table((*n_cipher)->table->start);
        free((*n_cipher)->table);
        (*n_cipher)->table = NULL;
    }

    /*
     * configure seed
     */
    if (seed != NULL)
        s = (char*)seed;

    if (((*n_cipher)->seed = (char*)
                malloc(sizeof(char) * (strlen(s) + 1))) == NULL) {
        fprintf(stderr, "%s: %s: %d: config_n_cipher(): malloc(): %s\n",
                LIBNAME, __FILE__, __LINE__, strerror(errno));

        goto ERR;
    } else {
        memset((*n_cipher)->seed, '\0', strlen(s) + 1);
        memcpy((*n_cipher)->seed, s, strlen(s));
    }

    /*
     * configure delimiter
     */
    if (delimiter != NULL)
        d = (char*)delimiter;

    if (((*n_cipher)->delimiter = (char*)
                malloc(sizeof(char) * (strlen(d) + 1))) == NULL) {
        fprintf(stderr, "%s: %s: %d: config_n_cipher(): malloc(): %s\n",
                LIBNAME, __FILE__, __LINE__, strerror(errno));

        goto ERR;
    } else {
        memset((*n_cipher)->delimiter, '\0', strlen(d) + 1);
        memcpy((*n_cipher)->delimiter, d, strlen(d));
    }

    /*
     * configure table
     */
    if (((*n_cipher)->table = (struct TABLE*)
                malloc(sizeof(struct TABLE))) == NULL) {
        fprintf(stderr, "%s: %s: %d: config_n_cipher(): malloc(): %s\n",
                LIBNAME, __FILE__, __LINE__, strerror(errno));

        goto ERR;
    }
    if (((*n_cipher)->table->decimal = create_table((*n_cipher)->seed,
                    &(*n_cipher)->table->start)) < 0) {
        fprintf(stderr, "%s: %s: %d: config_n_cipher(): create_table(): failure\n",
                LIBNAME, __FILE__, __LINE__);

        goto ERR;
    }

    return 0;

ERR:
    if ((*n_cipher)->seed != NULL) {
        free((*n_cipher)->seed);
        (*n_cipher)->seed = NULL;
    }
    if ((*n_cipher)->delimiter != NULL) {
        free((*n_cipher)->delimiter);
        (*n_cipher)->delimiter = NULL;
    }
    if ((*n_cipher)->table != NULL) {
        release_table((*n_cipher)->table->start);
        free((*n_cipher)->table);
        (*n_cipher)->table = NULL;
    }

    return -1;
}

static
int ready_n_cipher(N_CIPHER** n_cipher)
{
    if (n_cipher == NULL)
        return -1;

    if ((*n_cipher)->seed == NULL ||
            (*n_cipher)->delimiter == NULL)
        return -2;

    if (((*n_cipher)->table == NULL) ||
            (*n_cipher)->table->start == NULL)
        return -3;

    /* ready */
    return 0;
}

static
char* encode_n_cipher(N_CIPHER** n_cipher, const char* string)
{
    if ((n_cipher == NULL || string == NULL) ||
                (*n_cipher)->ready(n_cipher) < 0)
        return NULL;

    size_t      x_bufl  = BUFLEN,
                delmlen = 0,
                destlen = 0,
                blklen  = 0;

    char*       dest    = NULL,
        *       tmp     = NULL;

    glong       i       = 0,
                length  = 0;

    gunichar*   cpoints = NULL;

    list_t*     t1      = NULL,
          *     t2      = NULL;

    /* get string length */
    length = g_utf8_strlen(string, -1);

    /* convert string to ucs-4 */
    cpoints = g_utf8_to_ucs4_fast(string, -1, NULL);

    /*
     * allocate memory
     */
    if ((dest = (char*)
                malloc(sizeof(char) * x_bufl)) == NULL) {
        fprintf(stderr, "%s: %s: %d: encode_n_cipher(): malloc(): %s\n",
                LIBNAME, __FILE__, __LINE__, strerror(errno));

        goto ERR;
    } else {
        memset(dest, '\0', x_bufl);
        delmlen = strlen((*n_cipher)->delimiter);
    }

    /*
     * setting table
     */
    t1 = (*n_cipher)->table->start;
    t2 = seek_table_end((*n_cipher)->table->start);

    /*
     * convert ucs-4 to n_cipher
     */
    i = 0;
    while (i < length) {
        if ((tmp = encode_table((unsigned int)cpoints[i],
                        (*n_cipher)->table->decimal, t1, t2)) == NULL)
            goto ERR;

        /*
         * reallocate memory
         */
        destlen = strlen(dest);
        blklen = strlen(tmp);
        if (x_bufl <= (destlen + blklen + delmlen)) {
            x_bufl += BUFLEN;
            if ((dest = (char*)
                        realloc(dest, sizeof(char) * x_bufl)) == NULL) {
                fprintf(stderr, "%s: %s: %d: encode_n_cipher(): realloc(): %s\n",
                        LIBNAME, __FILE__, __LINE__, strerror(errno));

                goto ERR;
            }
        }

        /*
         * concat string
         */
        memcpy(dest + destlen, tmp, blklen);
        destlen += blklen;
        memcpy(dest + destlen, (*n_cipher)->delimiter, delmlen);
        destlen += delmlen;
        *(dest + destlen) = '\0';

        if (tmp != NULL) {
            free(tmp);
            tmp = NULL;
        }
        i++;
    }
    /* release memory */
    g_free(cpoints);

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

    return NULL;
}

static
char* decode_n_cipher(N_CIPHER** n_cipher, const char* string)
{
    if ((n_cipher == NULL || string == NULL) ||
                (*n_cipher)->ready(n_cipher) < 0)
        return NULL;
    
    int         ret     = 0;

    size_t      x_bufl  = BUFLEN,
                destlen = 0,
                blklen  = 0;

    char*       strtmp  = NULL,
        *       token   = NULL,
        *       dest    = NULL;

    gunichar    cpoint  = 0;
    gchar*      buf     = NULL;

    list_t*     t1      = NULL,
          *     t2      = NULL;

    /*
     * allocate memory
     */
    if ((buf = (gchar*)
                malloc(sizeof(gchar))) == NULL) {
        fprintf(stderr, "%s: %s: %d: decode_n_cipher(): malloc(): %s\n",
                LIBNAME, __FILE__, __LINE__, strerror(errno));

        goto ERR;
    }
    if ((dest = (char*)
                malloc(sizeof(char) * x_bufl)) == NULL) {
        fprintf(stderr, "%s: %s: %d: decode_n_cipher(): malloc(): %s\n",
                LIBNAME, __FILE__, __LINE__, strerror(errno));

        goto ERR;
    }
    if ((strtmp = (char*)
                malloc(sizeof(char) * (strlen(string) + 1))) == NULL) {
        fprintf(stderr, "%s: %s: %d: decode_n_cipher(): malloc(): %s\n",
                LIBNAME, __FILE__, __LINE__, strerror(errno));

        goto ERR;
    }

    /*
     * initialize variable
     */
    memset(dest, '\0', x_bufl);
    memset(strtmp, '\0', strlen(string) + 1);
    memcpy(strtmp, string, strlen(string));

    /*
     * setting table
     */
    t1 = (*n_cipher)->table->start;
    t2 = seek_table_end((*n_cipher)->table->start);
    
    /*
     * decode
     */
    token = mbstrtok(strtmp, (*n_cipher)->delimiter);
    while (token != NULL) {
        /* get ucs4 codepint */
        if ((ret = decode_table(token,
                        (*n_cipher)->table->decimal, t1, t2)) < 0)
            goto ERR;

        /*
         * convert ucs4 to character
         */
        cpoint = (gunichar)ret;
        blklen = g_unichar_to_utf8(cpoint, buf);

        /*
         * concat character
         */
        if (x_bufl <= (destlen + blklen)) {
            x_bufl += BUFLEN;
            if ((dest = (char*)
                        realloc(dest, sizeof(char) * x_bufl)) == NULL) {
                fprintf(stderr, "%s: %s: %d: decode_n_cipher(): realloc(): %s\n",
                        LIBNAME, __FILE__, __LINE__, strerror(errno));

                goto ERR;
            }
        }

        /*
         * concat string
         */
        memcpy(dest + destlen, buf, blklen);
        destlen += blklen;
        *(dest + destlen) = '\0';
        token = mbstrtok(NULL, (*n_cipher)->delimiter);
    }

    /*
     * release memory
     */
    if (buf != NULL) {
        free(buf);
        buf = NULL;
    }
    if (strtmp != NULL) {
        free(strtmp);
        buf = NULL;
    }

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

    return NULL;
}

static
char* version_n_cipher(void)
{
    static char version[64];

    /* libncipher V.P.S-E */
    memset(version, '\0', sizeof(version));
    snprintf(version, sizeof(version), "%s %d.%d.%d%s",
            LIBNAME, VERSION, PATCHLEVEL, SUBLEVEL, EXTRAVERSION);

    return version;
}

static
void release_n_cipher(N_CIPHER* n_cipher)
{
    if (n_cipher->seed != NULL) {
        free(n_cipher->seed);
        n_cipher->seed = NULL;
    }
    if (n_cipher->delimiter != NULL) {
        free(n_cipher->delimiter);
        n_cipher->delimiter = NULL;
    }
    if (n_cipher->table != NULL) {
        release_table(n_cipher->table->start);
        free(n_cipher->table);
        n_cipher->table = NULL;
    }
    if (n_cipher != NULL) {
        free(n_cipher);
        n_cipher = NULL;
    }

    return;
}
