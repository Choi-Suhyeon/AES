#include <stdio.h>
#include <unistd.h>
#include "file_io.h"
#include "aes_enc_dec.h"

int main(int argc, char ** argv) {
    const char * const kEnc = "enc";
    const char * const kDec = "dec";

    byte        raw_master_key[] = {
        0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 
        0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
    };
    pAllocBytes master_key       = malloc(sizeof(AllocBytes));
    
    u32 is_enc;
    byte (* write_file) (pFile);
    void (* process)    (pAllocBytes, pAllocBytes);

    if (NULL
        || argc != 4
        || access(argv[2], F_OK) == -1
        || ((is_enc = strcmp(argv[1], kDec)) && strcmp(argv[1], kEnc))
    ) return 1;

    switch (*(pu32)argv[3]) {
    case 0x626365: process = is_enc ? encryptWithECB : decryptWithECB; break;
    case 0X636263: process = is_enc ? encryptWithCBC : decryptWithCBC; break;
    case 0X727463: process = is_enc ? encryptWithCTR : decryptWithCTR; break;
    default:       return 1;
    }
  
    write_file  = is_enc ? write_cypher_file : write_plain_file;
    *master_key = (AllocBytes) { raw_master_key, sizeof raw_master_key };

    pFile file = read_file(argv[2]);

    process(file->content, master_key);
    write_file(file);

    free(master_key);
    free(file->content->memory);
    free(file->content);
    free(file);

    return 0;
}
