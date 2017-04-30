#include "../src/n_cipher.h"
#include "./file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define PROGNAME    "n_cipher sample program"

void print_usage(N_CIPHER* n_cipher)
{
    init_n_cipher(&n_cipher);
    fprintf(stdout, "The %s, with %s\n\
Usage: sample --encode [OPTION]...\n\
       sample --decode [OPTION]...\n\
\n\
Mandatory arguments to long options are mandatory for short options too.\n\
\n\
  -e,  --encode              encode n_cipher\n\
  -d,  --decode              decode n_cipher\n\
  -s,  --seed=STR            specify seed string\n\
  -m,  --delimiter=STR       specify delimiter string\n\
\n\
       --help                display this help and exit\n\
       --version             output version infomation and exit\n",
       PROGNAME, n_cipher->version());
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

int main(int argc, char* argv[])
{
    int         i           = 0,
                lines       = 0,
                res         = 0,
                index       = 0;

    short       eflag       = 0,
                dflag       = 0;

    char*       seed        = NULL,
        *       delimiter   = NULL,
        *       str         = NULL,
        **      buf         = NULL;

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
                eflag = 1;
                break;
            case    'd':
                dflag = 1;
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

    init_n_cipher(&n_cipher);
    if (seed != NULL) {
        if (n_cipher->check_seed(seed) != 0) {
            fprintf(stderr, "invalid seed\n");
            n_cipher->release(n_cipher);

            return 1;
        }
    }
    n_cipher->config(&n_cipher, seed, delimiter);

    if ((lines = p_read_file_char(&buf, 128, 128, stdin, 0)) < 0) {
        fprintf(stderr, "p_read_file_char() failure\n");

        return 1;
    }
    i = 0;
    while (i < lines) {
        if (eflag == 1)
            str = n_cipher->encode(&n_cipher, buf[i]);
        else if (dflag == 1)
            str = n_cipher->decode(&n_cipher, buf[i]);

        if (str != NULL) {
            fprintf(stdout, "%s", str);
            free(str);
            str = NULL;
        }
        free(buf[i]);
        i++;
    }
    n_cipher->release(n_cipher);
    free(buf);

    return 0;
}
