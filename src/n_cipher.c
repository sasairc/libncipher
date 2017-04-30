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

#define LIBNAME         "libncipher"
#define VERSION         1
#define PATCHLEVEL      3
#define SUBLEVEL        1
#define EXTRAVERSION    "-devel"

static int check_seed_overlap_n_cipher(const char* seed);
static int config_n_cipher(N_CIPHER** n_cipher, const char* seed, const char* delimiter);
static char* encode_n_cipher(N_CIPHER** n_cipher, const char* string);
static char* decode_n_cipher(N_CIPHER** n_cipher, const char* string);
static char* version_n_cipher(void);
static void release_n_cipher(N_CIPHER* n_cipher);

int init_n_cipher(N_CIPHER** n_cipher)
{
    N_CIPHER*   nc  = NULL;

    if ((nc = (N_CIPHER*)
                malloc(sizeof(N_CIPHER))) == NULL) {
        fprintf(stderr, "%s: init_n_cipher(): malloc() falure\n",
                LIBNAME);

        return -1;
    }

    nc->seed = NULL;
    nc->delimiter = NULL;
    nc->check_seed = check_seed_overlap_n_cipher;
    nc->config = config_n_cipher;
    nc->encode = encode_n_cipher;
    nc->decode = decode_n_cipher;
    nc->version = version_n_cipher;
    nc->release = release_n_cipher;

    *n_cipher = nc;

    return 0;
}   

static
int check_seed_overlap_n_cipher(const char* seed)
{
    if (seed == NULL)
        return -1;

    int         i       = 0,
                j       = 0,
                ret     = 0,
                decimal = 0;

    char*       tmp     = NULL;

    list_t*     t1      = NULL,
          *     t2      = NULL,
          *     start   = NULL;

    if (mbstrlen_without_byte((char*)seed) < 1)
        return -2;

    if ((tmp = (char*)
                malloc(sizeof(char) * (strlen(seed) + 1))) == NULL)
        return -3;

    memcpy(tmp, seed, strlen(seed));
    tmp[strlen(tmp)] = '\0';

    if ((decimal = create_table(tmp, &t1, &start)) == 0) {
        free(tmp);

        return -4;
    }

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
    free(tmp);
    release_table(start);

    return ret - decimal;
}

static
int config_n_cipher(N_CIPHER** n_cipher, const char* seed, const char* delimiter)
{
    if (n_cipher == NULL)
        return -1;

    char*   p   = NULL;

    size_t  len;

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

    /*
     * configure seed
     */
    if (seed == NULL)
        p = SEED;
    else
        p = (char*)seed;

    len = strlen(p);
    if (((*n_cipher)->seed = (char*)
                malloc(sizeof(char) * (len + 1))) == NULL) {
        fprintf(stderr, "%s: config_n_cipher(): malloc() failure\n",
                LIBNAME);

        goto ERR;
    } else {
        memcpy((*n_cipher)->seed, p, len);
        (*n_cipher)->seed[len] = '\0';
    }

    /*
     * configure delimiter
     */
    if (delimiter == NULL)
        p = DELIMITER;
    else
        p = (char*)delimiter;

    len = strlen(p);
    if (((*n_cipher)->delimiter = (char*)
                malloc(sizeof(char) * (len + 1))) == NULL) {
        fprintf(stderr, "%s: config_n_cipher(): malloc() failure\n",
                LIBNAME);

        goto ERR;
    } else {
        memcpy((*n_cipher)->delimiter, p, len);
        (*n_cipher)->delimiter[len] = '\0';
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

    return -2;
}

static
char* encode_n_cipher(N_CIPHER** n_cipher, const char* string)
{
    if ((n_cipher == NULL || string == NULL)
            || (*n_cipher)->seed == NULL || (*n_cipher)->delimiter == NULL)
        return NULL;

    int         i       = 0,
                decimal = 0;

    size_t      x_bufl  = BUFLEN,
                delmlen = 0,
                destlen = 0,
                blklen  = 0;

    char*       dest    = NULL,
        *       tmp     = NULL;

    glong       length  = 0;
    gunichar*   cpoints = NULL;

    list_t*     table   = NULL,
          *     start   = NULL;

    /* convert seed to table */
    if ((decimal = create_table((*n_cipher)->seed, &table, &start)) == 0)
        return NULL;

    /* get string length */
    length = g_utf8_strlen(string, -1);

    /* convert string to ucs4 */
    cpoints = g_utf8_to_ucs4_fast(string, -1, NULL);

    /* allocate memory */
    if ((dest = (char*)
                malloc(sizeof(char) * x_bufl)) == NULL)
        goto ERR;
    else
        memset(dest, '\0', x_bufl);

    delmlen = strlen((*n_cipher)->delimiter);

    /* convert ucs4 to table */
    i = 0;
    while (i < length) {
        if ((tmp = encode_table(
                        (unsigned int)cpoints[i], decimal, table, start)) == NULL)
            goto ERR;

        destlen = strlen(dest);
        blklen = strlen(tmp);

        /* reallocate memory */
        if (x_bufl < (destlen + blklen + delmlen)) {
            x_bufl += BUFLEN;
            if ((dest = (char*)
                        realloc(dest, sizeof(char) * x_bufl)) == NULL)
                goto ERR;
        }

        /* concat string */
        memcpy(dest + destlen, tmp, blklen);
        destlen += blklen;
        memcpy(dest + destlen, (*n_cipher)->delimiter, delmlen + 1);
        destlen += delmlen;

        if (tmp != NULL) {
            free(tmp);
            tmp = NULL;
        }
        i++;
    }
    *(dest + destlen) = '\0';

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

static
char* decode_n_cipher(N_CIPHER** n_cipher, const char* string)
{
    if ((n_cipher == NULL || string == NULL)
            || (*n_cipher)->seed == NULL || (*n_cipher)->delimiter == NULL)
        return NULL;
    
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
    if ((decimal = create_table((*n_cipher)->seed, &table, &start)) == 0)
        return NULL;

    /* allocate memory */
    if ((buf = (gchar*)
                malloc(sizeof(gchar))) == NULL)
        goto ERR;

    if ((dest = (char*)
                malloc(sizeof(char) * x_bufl)) == NULL)
        goto ERR;
    else
        memset(dest, '\0', x_bufl);

    if ((strtmp = (char*)
                malloc(sizeof(char) * (strlen(string) + 1))) == NULL)
        goto ERR;
    else
        memcpy(strtmp, string, strlen(string));

    /*
     * decode
     */
    for (token = mbstrtok(strtmp, (*n_cipher)->delimiter);
            token;
            token = mbstrtok(NULL, (*n_cipher)->delimiter)) {
        /* get ucs4 codepint */
        cpoint = (gunichar)decode_table(token, decimal, table, start);

        /* convert ucs4 to character */
        blklen = g_unichar_to_utf8(cpoint, buf);

        /* concat character */
        if (x_bufl <= destlen + blklen + 1) {
            x_bufl += BUFLEN;
            if ((dest = (char*)
                        realloc(dest, sizeof(char) * x_bufl)) == NULL)
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

static
char* version_n_cipher(void)
{
    static char version[64];

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
    if (n_cipher != NULL) {
        free(n_cipher);
        n_cipher = NULL;
    }

    return;
}
