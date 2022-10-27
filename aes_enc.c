#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MUL2(x) (((x) << 1) ^ ((x) & 0x80 ? 0x1B : 0))
#define MUL3(x) (MUL2((x)) ^ (x))
#define MUL4(x) (MUL2(MUL2((x))))
#define MUL8(x) (MUL2(MUL4((x))))
#define MULB(x) (MUL3(MUL3((x))) ^ MUL2((x)))
#define MULD(x) (MUL3(MUL3((x))) ^ MUL4((x)))
#define MULE(x) (MUL3(MUL2((x))) ^ MUL8((x)))

#define RND_KEY_IDX(r, i) ((r) + ((i) << 4))

#define ADD_RND_KEY(s, k)       \
__asm__ __volatile__ (          \
    "movq (%%rsi), %%rbx  \n\t" \
    "xorq %%rbx, (%%rdi)  \n\t" \
    "movq 8(%%rsi), %%rbx \n\t" \
    "xorq %%rbx, 8(%%rdi) \n\t" \
    :: "D"((s)), "S"((k))       \
    : "rbx"                     \
)

#define ROT_WORD(s, n)         \
__asm__ __volatile__ (         \
    "xorq %%rbx, %%rbx   \n\t" \
    "movl (%%rdi), %%ebx \n\t" \
    "rorl $"#n", %%ebx   \n\t" \
    "movl %%ebx, (%%rdi) \n\t" \
    :: "D"((s))                \
    : "rbx"                    \
)

#define MIX_COL_0X(w, x, y, z) (MUL2((w)) ^ MUL3((x)) ^ (y) ^ (z))                        
#define MIX_COL_1X(w, x, y, z) (MUL2((x)) ^ MUL3((y)) ^ (z) ^ (w))
#define MIX_COL_2X(w, x, y, z) (MUL2((y)) ^ MUL3((z)) ^ (w) ^ (x))
#define MIX_COL_3X(w, x, y, z) (MUL2((z)) ^ MUL3((w)) ^ (x) ^ (y))

typedef unsigned char byte, * pbyte;
typedef unsigned long u32,  * pu32;

typedef struct {
    pbyte memory;
    u32   length;
} AllocBytes, * pAllocBytes;

typedef struct {
    pbyte keys;
    u32   number;
} Round, * pRound;

const byte kRCon[0xA] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36 };

const byte kSBox[0x100] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

void printInterim(pbyte bytes, char * title) {
    printf("------ %s ------\n", title);
    for (unsigned i = 0; i < 16; i++) {
        printf("0x%02x ", bytes[i]);
        if ((i + 1) % 4 == 0) puts("");
    }
    puts("");
}

void updateAllocBytes(pAllocBytes obj, pbyte bytes, u32 len) {
    free(obj->memory);
    obj->memory = bytes;
    obj->length = len;
}

void swap(pbyte x, pbyte y) {
    *x ^= *y;
    *y ^= *x;
    *x ^= *y;
}

void toggleState(pbyte block) {
    swap(block + 0x1, block + 0x4);
    swap(block + 0x2, block + 0x8);
    swap(block + 0x3, block + 0xC);
    swap(block + 0x6, block + 0x9);
    swap(block + 0x7, block + 0xD);
    swap(block + 0xB, block + 0xE);
}

void subBytes(pbyte stt) {
    stt[0x0] = kSBox[stt[0x0]]; stt[0x1] = kSBox[stt[0x1]]; stt[0x2] = kSBox[stt[0x2]]; stt[0x3] = kSBox[stt[0x3]];
    stt[0x4] = kSBox[stt[0x4]]; stt[0x5] = kSBox[stt[0x5]]; stt[0x6] = kSBox[stt[0x6]]; stt[0x7] = kSBox[stt[0x7]]; 
    stt[0x8] = kSBox[stt[0x8]]; stt[0x9] = kSBox[stt[0x9]]; stt[0xA] = kSBox[stt[0xA]]; stt[0xB] = kSBox[stt[0xB]]; 
    stt[0xC] = kSBox[stt[0xC]]; stt[0xD] = kSBox[stt[0xD]]; stt[0xE] = kSBox[stt[0xE]]; stt[0xF] = kSBox[stt[0xF]]; 
}

void shiftRows(pbyte stt) {
    ROT_WORD(stt + 0x4, 0x08);
    ROT_WORD(stt + 0x8, 0x10);
    ROT_WORD(stt + 0xC, 0x18);
}

void mixColumns(pbyte stt) {
    byte tmp[0x10];

    memmove(tmp, stt, 0x10);
    toggleState(tmp);

    stt[0x0] = MIX_COL_0X(tmp[0x0], tmp[0x1], tmp[0x2], tmp[0x3]);
    stt[0x1] = MIX_COL_1X(tmp[0x0], tmp[0x1], tmp[0x2], tmp[0x3]);
    stt[0x2] = MIX_COL_2X(tmp[0x0], tmp[0x1], tmp[0x2], tmp[0x3]);
    stt[0x3] = MIX_COL_3X(tmp[0x0], tmp[0x1], tmp[0x2], tmp[0x3]);

    stt[0x4] = MIX_COL_0X(tmp[0x4], tmp[0x5], tmp[0x6], tmp[0x7]);
    stt[0x5] = MIX_COL_1X(tmp[0x4], tmp[0x5], tmp[0x6], tmp[0x7]);
    stt[0x6] = MIX_COL_2X(tmp[0x4], tmp[0x5], tmp[0x6], tmp[0x7]);
    stt[0x7] = MIX_COL_3X(tmp[0x4], tmp[0x5], tmp[0x6], tmp[0x7]);

    stt[0x8] = MIX_COL_0X(tmp[0x8], tmp[0x9], tmp[0xA], tmp[0xB]);
    stt[0x9] = MIX_COL_1X(tmp[0x8], tmp[0x9], tmp[0xA], tmp[0xB]);
    stt[0xA] = MIX_COL_2X(tmp[0x8], tmp[0x9], tmp[0xA], tmp[0xB]);
    stt[0xB] = MIX_COL_3X(tmp[0x8], tmp[0x9], tmp[0xA], tmp[0xB]);

    stt[0xC] = MIX_COL_0X(tmp[0xC], tmp[0xD], tmp[0xE], tmp[0xF]);
    stt[0xD] = MIX_COL_1X(tmp[0xC], tmp[0xD], tmp[0xE], tmp[0xF]);
    stt[0xE] = MIX_COL_2X(tmp[0xC], tmp[0xD], tmp[0xE], tmp[0xF]);
    stt[0xF] = MIX_COL_3X(tmp[0xC], tmp[0xD], tmp[0xE], tmp[0xF]);

    toggleState(stt);
}

void makeT128(pbyte pre_word, u32 r_con) {
   ROT_WORD(pre_word, 8);

    pre_word[0] = kSBox[pre_word[0]] ^ r_con;
    pre_word[1] = kSBox[pre_word[1]];
    pre_word[2] = kSBox[pre_word[2]];
    pre_word[3] = kSBox[pre_word[3]];
}

pbyte expandKey128(pbyte master_key, u32 rnd_num) {
    pbyte pre_word   = malloc(4),
          round_keys = malloc((rnd_num + 1) * 0x10);

    memmove(round_keys, master_key, 0x10);

    for (u32 i = 0x10; i < rnd_num; i += 0x10) {
        memmove(pre_word, round_keys + i - 0x4, 4);
        makeT128(pre_word, kRCon[(i >> 4) - 1]);

        u32 word1 = *(pu32)pre_word ^ *(pu32)round_keys[i - 0x10];
        u32 word2 = *(pu32)word1    ^ *(pu32)round_keys[i - 0x0C];
        u32 word3 = *(pu32)word2    ^ *(pu32)round_keys[i - 0x08];
        u32 word4 = *(pu32)word3    ^ *(pu32)round_keys[i - 0x04];

        memmove(round_keys + i + 0x0, &word1, 4);
        memmove(round_keys + i + 0x4, &word2, 4);
        memmove(round_keys + i + 0x8, &word3, 4);
        memmove(round_keys + i + 0xC, &word4, 4);
    }

    free(pre_word);
    return round_keys;
}

pbyte expandKey192(pbyte master_key, u32 rnd_num) {}

pbyte expandKey256(pbyte master_key, u32 rnd_num) {}

pRound keySchedule(pAllocBytes master_key) {
    pRound result = (pRound) malloc(sizeof(Round));

    switch(master_key->length) {
    case 16: *result = (Round) { expandKey128(master_key->memory, 10), 10 }; break;
    case 24: *result = (Round) { expandKey192(master_key->memory, 12), 12 }; break;
    case 32: *result = (Round) { expandKey256(master_key->memory, 14), 14 }; break;
    }

    return result;
}

void encryptAES(pbyte inout, pAllocBytes master_key) {
    toggleState(master_key->memory);
    toggleState(inout);

    u32    idx   = 0;
    pRound round = keySchedule(master_key);
    
    ADD_RND_KEY(inout, round->keys);

    printInterim(inout, "0 Round addRndKey");

    puts("\nDEBUG");

    while (++idx < round->number) {
        printf("\n--- Round %lu ---\n", idx);
        subBytes(inout);
        printInterim(inout, "SubBytes");
        shiftRows(inout);
        printInterim(inout, "ShiftRows");
        mixColumns(inout);
        printInterim(inout, "MixColumns");
        ADD_RND_KEY(inout, RND_KEY_IDX(round->keys, idx));
        printInterim(inout, "AddRoundKey");
        printInterim(RND_KEY_IDX(round->keys, idx), "KeySchedule");
    }

    puts("-----------------\n");

    subBytes(inout);
    printInterim(inout, "SubBytes");
    shiftRows(inout);
    printInterim(inout, "ShiftRows");
    ADD_RND_KEY(inout, RND_KEY_IDX(round->keys, idx));
    printInterim(inout, "AddRoundKey");

    free(round->keys);
    free(round);
}

void padding(pAllocBytes plain) {
    u32   padding_len = 0x10 - plain->length % 0x10,
          result_len  = plain->length + padding_len;
    pbyte result      = malloc(result_len);
    
    memset(result + plain->length, padding_len, padding_len);
    updateAllocBytes(plain, result, result_len);
}

void encryptWithECB(pAllocBytes plain, pAllocBytes master_key) {
    for (u32 i = 0; i < plain->length; i += 0x10) {
        encryptAES(plain->memory + i, master_key);
    }
}

int main(void) {
    AllocBytes key = {
        .memory = (byte[16]) {
            0x2B, 0x7E, 0x15, 0x16,
            0x28, 0xAE, 0xD2, 0xA6,
            0xAB, 0xF7, 0x15, 0x88,
            0x09, 0xCF, 0x4F, 0x3C
        },
        .length = 16
    };

    byte plain[16] = {
        0x32, 0x43, 0xF6, 0xA8,
        0x88, 0x5A, 0x30, 0x8D,
        0x31, 0x31, 0x98, 0xA2,
        0xE0, 0x37, 0x07, 0x34
    };

    encryptAES(plain, &key);

    puts("--------- Result -----------");

    for (unsigned i = 0; i < 16; i++) {
        printf("%#x ", plain[i]);
        if ((i + 1) % 4 == 0) puts("");
    }

    return 0;
}