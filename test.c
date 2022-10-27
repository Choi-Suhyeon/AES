#include <stdio.h>

#define MUL2(x) (((x) << 1) ^ ((x) & 0x80 ? 0x1B : 0))
#define MUL3(x) (MUL2((x)) ^ (x))
#define MUL4(x) (MUL2(MUL2((x))))
#define MUL8(x) (MUL2(MUL4((x))))

#define MULb(a) (MUL8((a)) ^ MUL2((a)) ^ (a))
#define MULd(a) (MUL8((a)) ^ MUL4((a)) ^ (a))
#define MULe(a) (MUL8((a)) ^ MUL4((a)) ^ MUL2((a)))

#define MULB(x) (MUL3(MUL3((x))) ^ MUL2((x)))
#define MULD(x) (MUL3(MUL3((x))) ^ MUL4((x)))
#define MULE(x) (MUL3(MUL2((x))) ^ MUL8((x)))

typedef unsigned char byte;

int main(void) {
    /*
    byte stt[16] = { 
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B,
        0x0C, 0x0D, 0x0E, 0x0F,
    };
    
    byte key[17] = {
        0x1, 0x1, 0x1, 0x1,
        0x1, 0x1, 0x1, 0x1,
        0x1, 0x1, 0x1, 0x1,
        0x1, 0x1, 0x1, 0x1,
    };
    */

    byte tmp[2] = { 0xD4, 0xE0 };

    printf("%#x\n", MULB(tmp[0]));
    printf("%#x\n", MULb(tmp[0]));
    puts("");

    printf("%#x\n", MULD(tmp[0]));
    printf("%#x\n", MULd(tmp[0]));
    puts("");

    printf("%#x\n", MULE(tmp[0]));
    printf("%#x\n", MULe(tmp[0]));
    puts("");

    return 0;
}