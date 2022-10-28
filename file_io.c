#include "file_io.h"

pFile read_file(char * file_name) {
    FILE * fp = fopen(file_name, "rb");
    
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);

    u32         length  = ftell(fp);
    pFile       result  = malloc(sizeof(pFile));
    pAllocBytes interim = malloc(sizeof(pAllocBytes));
    
    *interim = (AllocBytes) { malloc(length), length };

    fseek(fp, 0, SEEK_SET);
    fread(interim->memory, 1, length, fp);
    fclose(fp);

    *result = (File) { file_name, padding(interim) };

    return result;
}

byte write_cypher_file(pFile file) {
    char * suffix         = ".encrypted";
    u32    with_suffix_sz = strlen(file->name) + strlen(suffix) + 1;
    char * nm_with_suffix = malloc(with_suffix_sz);

    memmove(nm_with_suffix, file->name, strlen(file->name));
    strncat(nm_with_suffix, suffix, strlen(suffix));

    FILE * fp = fopen(nm_with_suffix, "wb");

    if (!fp) return NULL;

    fwrite(file->content->memory, sizeof(byte), file->content->length, fp);
    free(nm_with_suffix);
    fclose(fp);

    return 1;
}

byte write_plain_file(pFile file) {
    char *      suffix        = ".encrypted";
    u32         suffix_len    = strlen(suffix),
                pos_replaced  = strlen(file->name) - suffix_len;
    pAllocBytes to_be_written = unpadding(file->content);

    if (!strncmp(file + pos_replaced, suffix, suffix_len)) {
        file->name[pos_replaced] = '\0';
    }

    FILE * fp = fopen(file->name, "wb");

    if (!fp) return NULL;

    fwrite(to_be_written->memory, sizeof(byte), to_be_written->length, fp);
    fclose(fp);

    free(to_be_written->memory);
    free(to_be_written);

    return 1;
}
