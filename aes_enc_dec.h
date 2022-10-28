#ifndef AES_ENC_DEC_H
#define AES_ENC_DEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte, * pbyte;
typedef unsigned long u32,  * pu32;

typedef struct {
    pbyte memory;
    u32   length;
} AllocBytes, * pAllocBytes;

void encryptWithECB(pAllocBytes, pAllocBytes);
void decryptWithECB(pAllocBytes, pAllocBytes);
void encryptWithCBC(pAllocBytes, pAllocBytes);
void decryptWithCBC(pAllocBytes, pAllocBytes);
void encryptWithCTR(pAllocBytes, pAllocBytes);
void decryptWithCTR(pAllocBytes, pAllocBytes);
pAllocBytes padding(pAllocBytes);
pAllocBytes unpadding(pAllocBytes);

#endif