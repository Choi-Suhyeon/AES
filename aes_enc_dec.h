#ifndef AES_ENC_DEC_H
#define AES_ENC_DEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char u8,  * pu8;
typedef unsigned int  u32, * pu32;

typedef struct {
    pu8 memory;
    u32 length;
} AllocBytes, * pAllocBytes;

const u8 kCTRInitialVec[16];
const u8 kRawMasterKey[16];

void encryptWithECB(pAllocBytes, pAllocBytes);
void decryptWithECB(pAllocBytes, pAllocBytes);
void encryptWithCBC(pAllocBytes, pAllocBytes);
void decryptWithCBC(pAllocBytes, pAllocBytes);
void encryptWithCTR(pAllocBytes, pAllocBytes);
void decryptWithCTR(pAllocBytes, pAllocBytes);
AllocBytes padding(pAllocBytes);
AllocBytes unpadding(pAllocBytes);

#endif