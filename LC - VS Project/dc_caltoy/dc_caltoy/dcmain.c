#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "caltoy.h"

#define kNumOfKey   0x100
#define kNumOfPlain 0x10000

typedef uint8_t  u8,  * pu8;
typedef uint16_t u16, * pu16;
typedef uint32_t u32, * pu32;
typedef int16_t  i16, * pi16;
typedef int32_t  i32, * pi32;
typedef struct {
    u8  key;
    u16 val;
} KeyValPair, * pKeyValPair;
typedef struct {
    u8 shift_p;
    u8 shift_c;
    u8 filter_p;
    u8 filter_c;
} LinearApproxParam, * pLinearApproxParam;

const u8 kInvSBox[kNumOfKey] = {
    0xff, 0xf2, 0xf0, 0xfc, 0xfa, 0xf7, 0xf6, 0xfb, 0xf4, 0xf1, 0xf9, 0xfe, 0xfd, 0xf8, 0xf3, 0xf5,
    0x2f, 0x22, 0x20, 0x2c, 0x2a, 0x27, 0x26, 0x2b, 0x24, 0x21, 0x29, 0x2e, 0x2d, 0x28, 0x23, 0x25,
    0x0f, 0x02, 0x00, 0x0c, 0x0a, 0x07, 0x06, 0x0b, 0x04, 0x01, 0x09, 0x0e, 0x0d, 0x08, 0x03, 0x05,
    0xcf, 0xc2, 0xc0, 0xcc, 0xca, 0xc7, 0xc6, 0xcb, 0xc4, 0xc1, 0xc9, 0xce, 0xcd, 0xc8, 0xc3, 0xc5,
    0xaf, 0xa2, 0xa0, 0xac, 0xaa, 0xa7, 0xa6, 0xab, 0xa4, 0xa1, 0xa9, 0xae, 0xad, 0xa8, 0xa3, 0xa5,
    0x7f, 0x72, 0x70, 0x7c, 0x7a, 0x77, 0x76, 0x7b, 0x74, 0x71, 0x79, 0x7e, 0x7d, 0x78, 0x73, 0x75,
    0x6f, 0x62, 0x60, 0x6c, 0x6a, 0x67, 0x66, 0x6b, 0x64, 0x61, 0x69, 0x6e, 0x6d, 0x68, 0x63, 0x65,
    0xbf, 0xb2, 0xb0, 0xbc, 0xba, 0xb7, 0xb6, 0xbb, 0xb4, 0xb1, 0xb9, 0xbe, 0xbd, 0xb8, 0xb3, 0xb5,
    0x4f, 0x42, 0x40, 0x4c, 0x4a, 0x47, 0x46, 0x4b, 0x44, 0x41, 0x49, 0x4e, 0x4d, 0x48, 0x43, 0x45,
    0x1f, 0x12, 0x10, 0x1c, 0x1a, 0x17, 0x16, 0x1b, 0x14, 0x11, 0x19, 0x1e, 0x1d, 0x18, 0x13, 0x15,
    0x9f, 0x92, 0x90, 0x9c, 0x9a, 0x97, 0x96, 0x9b, 0x94, 0x91, 0x99, 0x9e, 0x9d, 0x98, 0x93, 0x95,
    0xef, 0xe2, 0xe0, 0xec, 0xea, 0xe7, 0xe6, 0xeb, 0xe4, 0xe1, 0xe9, 0xee, 0xed, 0xe8, 0xe3, 0xe5,
    0xdf, 0xd2, 0xd0, 0xdc, 0xda, 0xd7, 0xd6, 0xdb, 0xd4, 0xd1, 0xd9, 0xde, 0xdd, 0xd8, 0xd3, 0xd5,
    0x8f, 0x82, 0x80, 0x8c, 0x8a, 0x87, 0x86, 0x8b, 0x84, 0x81, 0x89, 0x8e, 0x8d, 0x88, 0x83, 0x85,
    0x3f, 0x32, 0x30, 0x3c, 0x3a, 0x37, 0x36, 0x3b, 0x34, 0x31, 0x39, 0x3e, 0x3d, 0x38, 0x33, 0x35,
    0x5f, 0x52, 0x50, 0x5c, 0x5a, 0x57, 0x56, 0x5b, 0x54, 0x51, 0x59, 0x5e, 0x5d, 0x58, 0x53, 0x55,
};

u16 concatNum(u8 h, u8 l) {
    return h << 8 | l;
}

u8 calcHW(u8 n) {
    int cnt = 0;

    for (; n; n &= n - 1, cnt++);

    return cnt;
}

void applyInvSBox(pu16 c) {
    *((pu8)c + 0) = kInvSBox[*((pu8)c + 0)];
    *((pu8)c + 1) = kInvSBox[*((pu8)c + 1)];
}

void applyInvPerm(pu16 c) {
    const u32 kIdx[0x10] = {
        0x0, 0x5, 0xA, 0xF, 0x4, 0x1, 0xE, 0xB,
        0x8, 0xD, 0x2, 0x7, 0xC, 0x9, 0x6, 0x3,
    };

    u16 filter = 1,
        result = 0;

    for (u32 i = 0; i < 0x10; i++, filter <<= 1) {
        result |= (*c & filter) >> i << kIdx[i];
    }

    *c = result;
}

u16 applyLinearApprox(u16 plain, u16 cypher, u8 key, pLinearApproxParam param) {
    return calcHW(plain >> param->shift_p & param->filter_p) +
           calcHW(kInvSBox[cypher >> param->shift_c & 0xFF ^ key] & param->filter_c);
}

void throwBackRnd4(pu16 cypher) {
    *cypher ^= 0x5BD6;
        
    applyInvSBox(cypher);
    applyInvPerm(cypher);
}

void throwBackRnd3(pu16 cypher) {
    *cypher ^= 0x5BD6;

    applyInvSBox(cypher);

    *cypher ^= 0x3FAB;

    applyInvPerm(cypher);
    applyInvSBox(cypher);
    applyInvPerm(cypher);
}

void getCypherArr(pu16 out) {
    for (u32 i = 0; i < kNumOfPlain; i++) {
        caltoy_enc(out + i, i);
    }
}

KeyValPair calcLC8(void (* throw_back) (pu16), pu32 cypher, pLinearApproxParam param) {
    pi32 count  = calloc(kNumOfKey, sizeof(i32));

    if (!(count && cypher)) {
        puts("Error Occurred");
        exit(1);
    }

    for (u32 i = 0; i < kNumOfPlain; i++) {
        caltoy_enc(cypher + i, i);
        if (throw_back) throw_back(cypher + i);
    }

    for (u16 key = 0; key < kNumOfKey; key++) {
        for (u32 plain = 0; plain < kNumOfPlain; plain++) {
            if (!(applyLinearApprox(plain, cypher[plain], key, param) & 1)) {
                count[key]++;
            }
        }

        count[key] -= kNumOfPlain >> 1;
    }

    i16 max_key = 0,
        max_val = 0,
        reserve = 0;
    
    for (u16 key = 0; key < kNumOfKey; key++) {
        if ((reserve = abs(count[key])) > max_val) {
            max_val = reserve;
            max_key = key;
        }
    }

    free(count);

    return (KeyValPair) { max_key, max_val };
}

int main(void) {
    pLinearApproxParam round_h = malloc(sizeof(LinearApproxParam)),
                       round_l = malloc(sizeof(LinearApproxParam));

    pu32 cypher_original = calloc(kNumOfPlain, sizeof(u32));
    pu32 cypher1         = calloc(kNumOfPlain, sizeof(u32));
    pu32 cypher2         = calloc(kNumOfPlain, sizeof(u32));

    if (!(!NULL
        && round_h 
        && round_l 
        && cypher1 
        && cypher2 
        && cypher_original)
    ) {
        puts("Error Occurred");
        return 1;
    }

    getCypherArr(cypher_original);
    memmove(cypher1, cypher_original, kNumOfPlain * sizeof(u32));
    memmove(cypher2, cypher_original, kNumOfPlain * sizeof(u32));

    *round_h = (LinearApproxParam){ 0x0C, 0x08, 0x05, 0x92 };
    *round_l = (LinearApproxParam){ 0x04, 0x00, 0x0F, 0x92 };

    KeyValPair rk_5h = calcLC8(NULL, cypher1, round_h),
               rk_5l = calcLC8(NULL, cypher2, round_l);
    u16        rk_5  = concatNum(rk_5h.key, rk_5l.key);

    memmove(cypher1, cypher_original, kNumOfPlain * sizeof(u32));
    memmove(cypher2, cypher_original, kNumOfPlain * sizeof(u32));

    *round_h = (LinearApproxParam){ 0x08, 0x08, 0x02, 0x92 },
    *round_l = (LinearApproxParam){ 0x00, 0x00, 0x02, 0x92 };

    KeyValPair rk_4h = calcLC8(throwBackRnd4, cypher1, round_h),
               rk_4l = calcLC8(throwBackRnd4, cypher2, round_l);
    u16        rk_4  = concatNum(rk_4h.key, rk_4l.key);
    
    applyInvPerm(&rk_4);

    memmove(cypher1, cypher_original, kNumOfPlain * sizeof(u32));
    memmove(cypher2, cypher_original, kNumOfPlain * sizeof(u32));

    *round_h = (LinearApproxParam){ 0x04, 0x08, 0x0B, 0x92 },
    *round_l = (LinearApproxParam){ 0x0C, 0x00, 0x0B, 0x92 };

    KeyValPair rk_3h = calcLC8(throwBackRnd3, cypher1, round_h),
               rk_3l = calcLC8(throwBackRnd3, cypher2, round_l);
    u16        rk_3  = concatNum(rk_3h.key, rk_3l.key);

    applyInvPerm(&rk_3);

    free(round_h);
    free(round_l);
    free(cypher1);
    free(cypher2);
    free(cypher_original);

    u16 mk_0 = rk_3 ^ 0x3333,
        mk_1 = rk_4 ^ 0x4444,
        mk_2 = rk_5 ^ 0x5555;

    puts("======================================");
    printf(" UPPER ROUND 5  : 0x%02hX   | %+4hu |\n", rk_5h.key, rk_5h.val);
    printf(" LOWER ROUND 5  : 0x%02hX   | %+4hu |\n", rk_5l.key, rk_5l.val);
    printf(" UPPER ROUND 4  : 0x%02hX   | %+4hu |\n", rk_4h.key, rk_4h.val);
    printf(" LOWER ROUND 4  : 0x%02hX   | %+4hu |\n", rk_4l.key, rk_4l.val);
    printf(" UPPER ROUND 3  : 0x%02hX   | %+4hu |\n", rk_3h.key, rk_3h.val);
    printf(" LOWER ROUND 3  : 0x%02hX   | %+4hu |\n", rk_3l.key, rk_3l.val);
    puts("======================================");
    printf(" KEY of ROUND 5 : 0x%04hX\n", rk_5);
    printf(" KEY of ROUND 4 : 0x%04hX\n", rk_4);
    printf(" KEY of ROUND 3 : 0x%04hX\n", rk_3);
    puts("======================================");
    printf(" MASTER KEY     : 0x%04hX%04hX%04hX\n", mk_0, mk_1, mk_2);
    puts("======================================");

    return 0;
}