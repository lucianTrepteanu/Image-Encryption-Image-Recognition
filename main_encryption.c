#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "common.c"

#include "crypt.h"
#include "crypt.c"

int main(){
    FILE * fPaths = fopen("encryptionPaths.txt", "r");

    char * source_file = (char * ) malloc(200 * sizeof(char));
    fscanf(fPaths, "%s\n", source_file);
    source_file = realloc(source_file, 1 + strlen(source_file) * sizeof(char));

    char * dest_file = (char * ) malloc(200 * sizeof(char));
    fscanf(fPaths, "%s\n", dest_file);
    dest_file = realloc(dest_file, 1 + strlen(dest_file) * sizeof(char));

    char * decrypted_file = (char * ) malloc(200 * sizeof(char));
    fscanf(fPaths, "%s\n", decrypted_file);
    decrypted_file = realloc(decrypted_file, 1 + strlen(decrypted_file) * sizeof(char));

    char * key_file = (char * ) malloc(200 * sizeof(char));
    fscanf(fPaths, "%s\n", key_file);
    key_file = realloc(key_file, 1 + strlen(key_file) * sizeof(char));

    fclose(fPaths);

    write_encrypted(source_file, dest_file, key_file);

    write_decrypted(dest_file, decrypted_file, key_file);

    Bitmap encrypted = load_bitmap(dest_file);
    Bitmap decrypted = load_bitmap(decrypted_file);

    print_chi_square(decrypted_file);
    print_chi_square(dest_file);

    free_bitmap(encrypted);
    free_bitmap(decrypted);

    free(key_file);
    free(decrypted_file);
    free(dest_file);
    free(source_file);

    return 0;
}
