#include "./n_cipher.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char*   str     = "あなたはよいですか？こんにちは\0",
        *   cipher  = "ぱすすゃんす〜ぱすすにゃぱ〜んゃんぱにすぱ〜ぱすすにんん〜ぱすすにぱに〜ぱすぱすぱん〜ぱすすんにん〜ぱすすにんん〜\0",
        *   tmp     = NULL;

    tmp = encode_n_cipher(str, SEED, DELIMITER);
    fprintf(stdout, "encode: %s\n", tmp);
    free(tmp);

    tmp = decode_n_cipher(cipher, SEED, DELIMITER);
    fprintf(stdout, "decode: %s\n", tmp);
    free(tmp);

    return 0;
}
