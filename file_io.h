#ifndef AES_FILE_IO_H
#define AES_FILE_IO_H

#include <stdlib.h>
#include <string.h>
#include "aes_enc_dec.h"

typedef struct {
    char *     name;
    AllocBytes content;
} File, * pFile;

void read_cypher_file(pFile);
void read_plain_file(pFile);
u8 write_cypher_file(pFile);
u8 write_plain_file(pFile);

#endif