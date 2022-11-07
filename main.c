#include <stdio.h>
#include <unistd.h>
#include "file_io.h"
#include "aes_enc_dec.h"

int main(int argc, char ** argv) {
    const char * const kEnc = "enc";
    const char * const kDec = "dec";
    
    u32 is_enc;

    if (NULL
        || argc != 4
        || access(argv[2], F_OK) == -1
        || ((is_enc = strcmp(argv[1], kDec)) && strcmp(argv[1], kEnc) != 0)
    ) goto FAILURE;

    void (* procedure) (pAllocBytes, pAllocBytes) =
        !strcmp(argv[3], "ECB") ? (is_enc ? encryptWithECB : decryptWithECB) :
        !strcmp(argv[3], "CBC") ? (is_enc ? encryptWithCBC : decryptWithCBC) :
        !strcmp(argv[3], "CTR") ? (is_enc ? encryptWithCTR : decryptWithCTR) :
        NULL;

    char  *  ending_title                = "Encryption";
    bool (* read_file)  (pFile)          = readPlainFile;
    bool (* write_file) (pFile)          = writeCypherFile;
    void (* deco_name)  (char *, char *) = addEncryptedSuffix;

    if (!is_enc) {
        ending_title = "Decryption";
        read_file    = readCypherFile;
        write_file   = writePlainFile;
        deco_name    = delEncryptedSuffix;
    }

    if (!procedure) goto FAILURE;

    File       file       = { argv[2], (AllocBytes) {} };
    AllocBytes master_key = (AllocBytes) {
        .memory = (u8 [sizeof kRawMasterKey]) { 0 },
        .length = sizeof kRawMasterKey
    };

    if (!read_file(&file)) goto FAILURE;

    printf("[note]   : %s in progress.\n", ending_title);
    memmove(master_key.memory, kRawMasterKey, master_key.length);
    procedure(&file.content, &master_key);

    if (!write_file(&file)) goto FAILURE;

    char * new_name = malloc(strlen(argv[2]) + 11);

    deco_name(new_name, argv[2]);
    printf("[status] : %s -> %s\n", argv[2], new_name);
    printf("[result] : %s was successful.\n", ending_title);
    free(new_name);

    return 0;

FAILURE:
    printf("FAILED.\n");
    return 1;
}
