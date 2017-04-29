/*
 * n_cipher - sample.c
 */

#include "../src/n_cipher.h"
#include "./file.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

int main(int argc, char* argv[])
{
    int         i           = 0,
                res         = 0,
                index       = 0,
                lines       = 0;

    short       eflag       = 0,
                dflag       = 0;

    char*       str         = NULL,
        **      buf         = NULL;

    N_CIPHER*   n_cipher    = NULL;

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

    if ((lines = p_read_file_char(&buf, 128, 128, stdin, 0)) < 0) {
        fprintf(stderr, "p_read_file_char() failure\n");

        return 1;
    }

    init_n_cipher(&n_cipher);

    i = 0;
    while (i < lines) {
        if (eflag == 1)
            str = n_cipher->encode(buf[i], SEED, DELIMITER);
        else if (dflag == 1)
            str = n_cipher->decode(buf[i], SEED, DELIMITER);

        if (str != NULL) {
            fprintf(stdout, "%s", str);
            free(str);
            str = NULL;
        }
        free(buf[i]);
        i++;
    }
    free(buf);

    return 0;
}
