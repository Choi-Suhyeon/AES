#include <stdio.h>

typedef unsigned char byte, * pbyte;

void dc() {
    byte s_box[0x10] = {
        0xE, 0x4, 0xD, 0x1,
        0x2, 0xF, 0xB, 0x8,
        0x3, 0xA, 0x6, 0xC,
        0x5, 0x9, 0x0, 0x7
    };

    puts("    00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f");
    for (unsigned indiff = 0; indiff < 0x10; indiff++) {
        byte diffout[0x10] = { 0 };

        printf("%02x ", indiff);

        for (unsigned in1 = 0; in1 < 0x10; in1++) {
            unsigned in2 = in1 ^ indiff;
            diffout[s_box[in1] ^ s_box[in2]]++;
        }

        for (unsigned i = 0; i < 16; i++) {
            printf(" %02x", diffout[i]);
        }

        puts("");
    }
}

int main(void) {
    dc();
}
