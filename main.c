#include <stdio.h>
#include <unistd.h>
#include "file_io.h"
#include "aes_enc_dec.h"

//int main(int argc, char ** argv) {
int main(void) {
    int argc = 4;
    char * argv[4] = {"", "dec", "../1.jpg.encrypted", "CTR"};

    const char * const kEnc = "enc";
    const char * const kDec = "dec";
    
    u32 is_enc;

    if (NULL
        || argc != 4
        || access(argv[2], F_OK) == -1
        || ((is_enc = strcmp(argv[1], kDec)) && strcmp(argv[1], kEnc) != 0)
    ) return 1;

    void (* process) (pAllocBytes, pAllocBytes) =
        !strcmp(argv[3], "ECB") ? (is_enc ? encryptWithECB : decryptWithECB) :
        !strcmp(argv[3], "CBC") ? (is_enc ? encryptWithCBC : decryptWithCBC) :
        !strcmp(argv[3], "CTR") ? (is_enc ? encryptWithCTR : decryptWithCTR) :
        NULL;

    u8 (* write_file) (pFile) = is_enc
        ? write_cypher_file
        : write_plain_file;

    void (* read_file) (pFile) = is_enc
        ? read_plain_file
        : read_cypher_file;

    if (!process) return 1;

    File file = { argv[2], (AllocBytes) {} };

    AllocBytes master_key = (AllocBytes) {
        .memory = (u8 [sizeof kRawMasterKey]) { 0 },
        .length = sizeof kRawMasterKey
    };

    read_file(&file);
    memmove(master_key.memory, kRawMasterKey, master_key.length);
    process(&file.content, &master_key);
    write_file(&file);

    return 0;
}