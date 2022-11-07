#ifndef AES_FILE_IO_H
#define AES_FILE_IO_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "aes_enc_dec.h"

typedef struct {
    char *     name;
    AllocBytes content;
} File, * pFile;

void addEncryptedSuffix(char *, char *);
void delEncryptedSuffix(char *, char *);
bool readCypherFile(pFile);
bool readPlainFile(pFile);
bool writeCypherFile(pFile);
bool writePlainFile(pFile);

#endif