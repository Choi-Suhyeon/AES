#include "file_io.h"

void addEncryptedSuffix(char * dst, char * src) {
    char * suffix = ".encrypted";

    memmove(dst, src, strlen(src));
    strncat(dst, suffix, strlen(suffix));
}

void delEncryptedSuffix(char * dst, char * src) {
    char * suffix       = ".encrypted";
    u32    suffix_len   = strlen(suffix),
           src_len      = strlen(src),
           pos_replaced = src_len - suffix_len;

    memmove(dst, src, src_len);

    if (!strncmp(dst + pos_replaced, suffix, suffix_len)) {
        dst[pos_replaced] = '\0';
    }
}

bool readCypherFile(pFile file) {
    FILE * fp;
    u32    length;
    
    if (!(fp = fopen(file->name, "rb"))) return false;

    fseek(fp, 0, SEEK_END);

    file->content.length = length = ftell(fp);
    file->content.memory = malloc(length);

    fseek(fp, 0, SEEK_SET);
    fread(file->content.memory, 1, length, fp);
    fclose(fp);

    return true;
}

bool readPlainFile(pFile file) {
    if(!readCypherFile(file)) return false;
    file->content = padding(&file->content);

    return true;
}

bool writeCypherFile(pFile file) {
    FILE * fp       = NULL;
    char * new_name = malloc(strlen(file->name) + 11);

    addEncryptedSuffix(new_name, file->name);

    if (!(fp = fopen(new_name, "wb"))) return false;

    fwrite(file->content.memory, 1, file->content.length, fp);
    free(file->content.memory);
    free(new_name);
    fclose(fp);

    return true;
}

bool writePlainFile(pFile file) {
    FILE *      fp           = NULL;
    char *      new_name     = malloc(strlen(file->name) + 1);
    AllocBytes to_be_written = unpadding(&file->content);

    delEncryptedSuffix(new_name, file->name);

    if (!(fp = fopen(new_name, "wb"))) return false;

    fwrite(to_be_written.memory, 1, to_be_written.length, fp);
    free(to_be_written.memory);
    free(new_name);
    fclose(fp);

    return true;
}
