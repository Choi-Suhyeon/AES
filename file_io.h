#ifndef AES_FILE_IO_H
#define AES_FILE_IO_H

#include <stdlib.h>
#include <string.h>
#include "aes_enc_dec.h"

typedef struct {
    char *      name;
    pAllocBytes content;
} File, * pFile;

pFile read_file(char *);
byte write_cypher_file(pFile);
byte write_plain_file(pFile);

#endif