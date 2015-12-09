/*
 * sample.c
 */

#include "./n_cipher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

int p_count_file_lines(char** buf);
char** p_read_file_char(int t_lines, size_t t_length, FILE* fp);

int main(int argc, char* argv[])
{
    int     i       = 0,
            res     = 0,
            index   = 0,
            lines   = 0,
            eflag   = 0,
            dflag   = 0;
    char*   str     = NULL,
        **  buf     = NULL;

    struct  option opts[] = {
        {"encode",  no_argument,    NULL, 'e'},
        {"decode",  no_argument,    NULL, 'd'},
        {0, 0, 0, 0},
    };

    while ((res = getopt_long(argc, argv, "ed", opts, &index)) != -1) {
        switch (res) {
            case    'e':
                eflag = 1;
                break;
            case    'd':
                dflag = 1;
                break;
            case    '?':
                return -1;
        }
    }

    if ((buf = p_read_file_char(128, 128, stdin)) == NULL) {
        fprintf(stderr, "p_read_file_char() failure\n");

        return 1;
    }
    lines = p_count_file_lines(buf);

    for (i = 0; i < lines; i++) {
        if (eflag == 1)
            str = encode_n_cipher(buf[i], SEED, DELIMITER);
        else if (dflag == 1)
            str = decode_n_cipher(buf[i], SEED, DELIMITER);

        if (str != NULL) {
            fprintf(stdout, "%s", str);
            free(str);
            str = NULL;
        }
    }

    for (i = 0; i < lines; i++) {
        if (buf[i] != NULL) {
            free(buf[i]);
            buf[i] = NULL;
        }
    }
    free(buf);

    return 0;
}

int p_count_file_lines(char** buf)
{
    int i;

    for (i = 0; buf[i] != NULL; i++);

    return i;
}

char** p_read_file_char(int t_lines, size_t t_length, FILE* fp)
{
    int     lines   = t_lines,
            length  = t_length,
            i       = 0,
            x       = 0,
            y       = 0,
            c       = 0;
    char*   str     = (char*)malloc(sizeof(char) * t_length),   /* allocate temporary array */
        **  buf     = (char**)malloc(sizeof(char*) * t_lines);  /* allocate array of Y coordinate */

    if (str == NULL || buf == NULL) {

        return NULL;
    } else if (t_lines == 0 || t_length == 0 || fp == NULL) {
        free(str);
        free(buf);

        return NULL;
    }

    while ((c = fgetc(fp)) != EOF) {
        switch (c) {
            case    '\n':
                str[x] = c;
                /* reallocate array of Y coordinate */
                if (y == (lines - 1)) {
                    lines += t_lines;
                    if ((buf = (char**)realloc(buf, sizeof(char*) * lines)) == NULL) {

                        goto ERR;
                    }
                }
                /* allocate array for X coordinate */
                if ((buf[y] = (char*)malloc(sizeof(char) * (strlen(str) + 1))) == NULL) {

                    goto ERR;
                }
                strcpy(buf[y], str);    /* copy, str to buffer */
                for (i = 0; i < length; i++) {
                    str[i] = '\0';      /* refresh temporary array */
                }
                x = 0;
                y++;
                break;
            default:
                /* reallocate temporary array */
                if (x == (length - 1)) {
                    length += t_length;
                    if ((str = (char*)realloc(str, length)) == NULL) {

                        goto ERR;
                    }
                }
                str[x] = c;
                x++;
                continue;
        }
    }
    /* no data */
    if (x == 0 && y == 0) {
        buf[y] = NULL;
        free(str);

        return buf;
    }

    if (str[0] != '\0') {
        if (y == (lines - 1)) {
            str[x] = c;
            lines += t_lines;
            if ((buf = (char**)realloc(buf, sizeof(char*) * lines)) == NULL) {

                goto ERR;
            }
        }
        if ((buf[y] = (char*)malloc(sizeof(char) * (strlen(str) + 1))) == NULL) {

            goto ERR;
        }
        strcpy(buf[y], str);
        y++;
    }
    buf[y] = NULL;
    free(str);

    return buf;


ERR:
    lines   -= t_lines;
    length  -= t_length;

    if (buf != NULL) {
        for (i = 0; i < lines; i++) {
            if (buf[i] != NULL) {
                free(buf[i]);
                buf[i] = NULL;
            }
        }
        free(buf);
    }
    if (str != NULL)
        free(str);

    return NULL;
}
