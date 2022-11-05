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

// const byte kCTRInitialVec[16] = { 0 };
/*
const byte kRawMasterKey[]    = {
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};*/

void encryptWithECB(pAllocBytes, pAllocBytes);
void decryptWithECB(pAllocBytes, pAllocBytes);
void encryptWithCBC(pAllocBytes, pAllocBytes);
void decryptWithCBC(pAllocBytes, pAllocBytes);
void encryptWithCTR(pAllocBytes, pAllocBytes);
void decryptWithCTR(pAllocBytes, pAllocBytes);
pAllocBytes padding(pAllocBytes);
pAllocBytes unpadding(pAllocBytes);

#endif