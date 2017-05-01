/*
 * sample.c - n_cipher sample program
 */

#include "../src/n_cipher.h"
#include "./file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#define PROGNAME    "n_cipher sample program"

void print_usage(N_CIPHER* n_cipher)
{
    init_n_cipher(&n_cipher);
    n_cipher->config(&n_cipher, NULL, NULL);
    fprintf(stdout, "The %s, with %s\n\
Usage: sample --encode [OPTION]... [FILE]...\n\
       sample --decode [OPTION]... [FILE]...\n\
\n\
Mandatory arguments to long options are mandatory for short options too.\n\
\n\
  -e,  --encode              encode n_cipher (default)\n\
  -d,  --decode              decode n_cipher\n\
  -s,  --seed=STR            specify seed string (default = %s)\n\
  -m,  --delimiter=STR       specify delimiter string (default = %s)\n\
\n\
       --help                display this help and exit\n\
       --version             output version infomation and exit\n",
       PROGNAME, n_cipher->version(), n_cipher->seed, n_cipher->delimiter);
    n_cipher->release(n_cipher);

    exit(0);
}

void print_version(N_CIPHER* n_cipher)
{
    init_n_cipher(&n_cipher);
    fprintf(stdout, "%s with %s\n",
            PROGNAME, n_cipher->version());
    n_cipher->release(n_cipher);

    exit(0);
}

int do_proc(N_CIPHER* n_cipher, FILE* fp, short mode)
{
    int     i       = 0,
            lines   = 0;

    char*   str     = NULL,
        **  buf     = NULL,
        *   (*proc)(N_CIPHER** n_cipher, const char* string);

    if (mode == 1)
        proc = n_cipher->encode;
    else
        proc = n_cipher->decode;

    if ((lines = p_read_file_char(&buf, 128, 128, fp, 0)) < 0) {
        fprintf(stderr, "%s: do_proc(): p_read_file_char() failure\n",
                PROGNAME);

        return -1;
    }
    i = 0;
    while (i < lines) {
        if ((str = proc(&n_cipher, buf[i])) == NULL) {
            fprintf(stderr, "%s: %.*s: invalid input\n",
                    PROGNAME, strlen(buf[i]) - 1, buf[i]);

            goto ERR;
        } else {
            fprintf(stdout, "%s",
                    str);
            free(str);
            str = NULL;
        }
        free(buf[i]);
        i++;
    }
    if (mode == 1)
        putchar('\n');
    free(buf);

    return 0;

ERR:
    if (buf != NULL) {
        lines--;
        while (lines >= 0) {
            if (buf[lines] != NULL)
                free(buf[lines]);
            lines--;
        }
        free(buf);
    }

    return -2;
}

int main(int argc, char* argv[])
{
    int         res         = 0,
                index       = 0,
                status      = 0;

    FILE*       fp          = NULL;

    short       mode        = 1;

    char*       seed        = NULL,
        *       delimiter   = NULL;

    N_CIPHER*   n_cipher    = NULL;

    struct  option opts[] = {
        {"seed",        required_argument,  NULL, 's'},
        {"delimiter",   required_argument,  NULL, 'm'},
        {"encode",      no_argument,        NULL, 'e'},
        {"decode",      no_argument,        NULL, 'd'},
        {"help",        no_argument,        NULL,  0 },
        {"version",     no_argument,        NULL,  1 },
        {0, 0, 0, 0},
    };

    while ((res = getopt_long(argc, argv, "s:m:ed", opts, &index)) != -1) {
        switch (res) {
            case    'e':
                mode = 1;
                break;
            case    'd':
                mode = 0;
                break;
            case    's':
                seed = optarg;
                break;
            case    'm':
                delimiter = optarg;
                break;
            case    0:
                print_usage(n_cipher);
            case    1:
                print_version(n_cipher);
            case    '?':
                return -1;
        }
    }

    /* initialize n_cipher */
    init_n_cipher(&n_cipher);

    /* checking manually specifies, seed and delimiter */
    if (seed != NULL) {
        if (n_cipher->check_seed(seed) != 0) {
            fprintf(stderr, "%s: invalid seed: \n",
                    PROGNAME, seed);
            status = 1; goto RELEASE;
        }
    }

    /* configure seed and delimiter */
    n_cipher->config(&n_cipher, seed, delimiter);

    /* do processing */
    if (optind < argc) {
        /* [FILE]... */
        while (optind < argc) {
            if ((fp = fopen(argv[optind], "r")) == NULL) {
                perror("n_cipher sample program: fopen()");
                status = errno; goto RELEASE;
            }
            if (do_proc(n_cipher, fp, mode) < 0) {
                status = 2; goto RELEASE;
            }
            fclose(fp);
            fp = NULL;
            optind++;
        }
    } else {
        /* stdin */
        if (do_proc(n_cipher, stdin, mode) < 0) {
            status = 3; goto RELEASE;
        }
    }

RELEASE:
    if (fp != NULL)
        fclose(fp);

    /* release n_cipher */
    n_cipher->release(n_cipher);

    return status;
}
