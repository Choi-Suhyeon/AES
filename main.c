#include <stdio.h>
// #include <unistd.h>
// #include "file_io.h"
#include "aes_enc_dec.h"

int main(void) {
    byte raw_plain[16] = {
        0x32, 0x43, 0xF6, 0xA8,
        0x88, 0x5A, 0x30, 0x8D,
        0x31, 0x31, 0x98, 0xA2,
        0xE0, 0x37, 0x07, 0x34
    };

    byte raw_master[16] = {
        0x2B, 0x7E, 0x15, 0x16,
        0x28, 0xAE, 0xD2, 0xA6,
        0xAB, 0xF7, 0x15, 0x88,
        0x09, 0xCF, 0x4F, 0x3C
    };

    AllocBytes plain  = { raw_plain,  16 };
    AllocBytes master = { raw_master, 16 };

    encryptWithECB(&plain, &master);

    for (unsigned i = 0; i < 16; i++) {
        printf("0x%02X ", plain.memory[i]);
        if (!((i + 1) % 4)) printf("\n");
    }
    puts("");
}

/*
int main(int argc, char ** argv) {
    const char * const kEnc = "enc";
    const char * const kDec = "dec";

    pbyte       raw_master_key   = malloc(sizeof kRawMasterKey);
    pAllocBytes master_key       = malloc(sizeof(AllocBytes));
    
    u32 is_enc;
    byte (* write_file) (pFile);
    void (* process)    (pAllocBytes, pAllocBytes);

    if (NULL
        || argc != 4
        || access(argv[2], F_OK) == -1
        || ((is_enc = strcmp(argv[1], kDec)) && strcmp(argv[1], kEnc) != 0)
    ) return 1;

    switch (*(pu32)argv[3]) {
    case 0x626365: process = is_enc ? encryptWithECB : decryptWithECB; break;
    case 0X636263: process = is_enc ? encryptWithCBC : decryptWithCBC; break;
    case 0X727463: process = is_enc ? encryptWithCTR : decryptWithCTR; break;
    default:       return 1;
    }

    memmove(raw_master_key, kRawMasterKey, sizeof kRawMasterKey);

    write_file  = is_enc ? write_cypher_file : write_plain_file;
    *master_key = (AllocBytes) { raw_master_key, sizeof raw_master_key };

    pFile file = read_file(argv[2]);

    process(file->content, master_key);
    write_file(file);

    free(raw_master_key);
    free(master_key);
    free(file->content->memory);
    free(file->content);
    free(file);

    return 0;
}
*/