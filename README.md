libncipher
===

C/C++向け[n_cipher](https://github.com/844196/n_cipher)ライブラリ。

## Requirements

* glib-2.0

## Usage

```c
#include "./n_cipher.h"

char* encode_n_cipher(char* string, char* seed, char* delimiter);
char* decode_n_cipher(char* string, char* seed, char* delimiter);
```

エラー時には`NULL`を返却します。

## License

[MIT](https://github.com/sasairc/libncipher/blob/master/LICENSE)
