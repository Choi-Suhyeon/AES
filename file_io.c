#include "file_io.h"

void read_cypher_file(pFile file) {
    FILE * fp = fopen(file->name, "rb");
    
    if (!fp) return;

    fseek(fp, 0, SEEK_END);

    u32         length  = ftell(fp);
    
    file->content.memory = malloc(length);
    file->content.length = length;

    fseek(fp, 0, SEEK_SET);
    fread(file->content.memory, 1, length, fp);

    fclose(fp);
}

void read_plain_file(pFile file) {
    read_cypher_file(file);
    file->content = padding(&file->content);
}

u8 write_cypher_file(pFile file) {
    char * suffix         = ".encrypted";
    u32    with_suffix_sz = strlen(file->name) + strlen(suffix) + 1;
    char * nm_with_suffix = malloc(with_suffix_sz);

    memmove(nm_with_suffix, file->name, strlen(file->name));
    strncat(nm_with_suffix, suffix, strlen(suffix));

    FILE * fp = fopen(nm_with_suffix, "wb");

    if (!fp) return 0;

    fwrite(file->content.memory, sizeof(u8), file->content.length, fp);
    free(file->content.memory);
    free(nm_with_suffix);
    fclose(fp);

    return 1;
}

u8 write_plain_file(pFile file) {
    char *      suffix       = ".encrypted";
    u32         suffix_len   = strlen(suffix),
                pos_replaced = strlen(file->name) - suffix_len;
    char *      new_name     = calloc(strlen(file->name) + 1, 1);
    AllocBytes to_be_written = unpadding(&file->content);

    memmove(new_name, file->name, strlen(file->name));

    if (!strncmp(new_name + pos_replaced, suffix, suffix_len)) {
        new_name[pos_replaced] = '\0';
    }

    FILE * fp = fopen(new_name, "wb");

    if (!fp) return 0;

    fwrite(to_be_written.memory, sizeof(u8), to_be_written.length, fp);
    free(to_be_written.memory);
    free(new_name);
    fclose(fp);

    return 1;
}
