#include "crypt.h"

unsigned int xor_shift(unsigned int * state) {
    unsigned int res = ( * state);

    res ^= (res << 13);
    res ^= (res >> 17);
    res ^= (res << 5);

    ( * state) = res;
    return res;
}


unsigned int * encrypt(Bitmap bmp, char * key_file) {
    unsigned int dim_bmp = bmp.image_width * bmp.image_height;

    unsigned int R0, SV;
    FILE * fkey = fopen(key_file, "rb");
    fread( & R0, sizeof(unsigned int), 1, fkey);
    fread( & SV, sizeof(unsigned int), 1, fkey);
    fclose(fkey);

    unsigned int state = R0;
    unsigned int * R = (unsigned int * ) malloc(sizeof(unsigned int) * 2 * dim_bmp);
    for (int i = 1; i < 2 * dim_bmp; i++)
        R[i] = xor_shift( & state);

    unsigned int * perm = (unsigned int * ) malloc(sizeof(unsigned int) * dim_bmp);
    for (int i = 0; i < dim_bmp; i++)
        perm[i] = (unsigned int) i;

    for (int i = dim_bmp - 1; i > 0; i--) {
        unsigned int j = R[i] % (i + 1);

        unsigned int aux = perm[i];
        perm[i] = perm[j];
        perm[j] = aux;
    }

    unsigned int * interm = (unsigned int * ) malloc(sizeof(unsigned int) * dim_bmp);
    for (int i = 0; i < dim_bmp; i++)
        interm[perm[i]] = bmp.linearized_bmp[i];

    unsigned int * crypted = (unsigned int * ) malloc(sizeof(unsigned int) * dim_bmp);
    crypted[0] = SV ^ interm[0] ^ R[dim_bmp];
    for (int i = 1; i < dim_bmp; i++)
        crypted[i] = crypted[i - 1] ^ interm[i] ^ R[dim_bmp + i];

    free(interm);
    free(perm);
    free(R);

    return crypted;
}

void write_encrypted(char * image_file, char * dest_file, char * key_file) {
    Bitmap bmp = load_bitmap(image_file);

    unsigned int * enc = encrypt(bmp, key_file);

    Bitmap encrypted = bmp;
    encrypted.linearized_bmp = enc;

    write_bmp(encrypted, dest_file);
    free(enc);
}

unsigned int * decrypt(Bitmap bmp, char * key_file) {
    int dim_bmp = bmp.image_width * bmp.image_height;

    unsigned int R0, SV;
    FILE * fkey = fopen(key_file, "rb");
    fread( & R0, sizeof(unsigned int), 1, fkey);
    fread( & SV, sizeof(unsigned int), 1, fkey);
    fclose(fkey);

    unsigned int state = R0;
    unsigned int * R = (unsigned int * ) malloc(sizeof(unsigned int) * 2 * dim_bmp);
    for (int i = 1; i < 2 * dim_bmp; i++)
        R[i] = xor_shift( & state);

    unsigned int * perm = (unsigned int * ) malloc(sizeof(unsigned int) * dim_bmp);
    for (int i = 0; i < dim_bmp; i++)
        perm[i] = i;

    for (int i = dim_bmp - 1; i > 0; i--) {
        int j = R[i] % (i + 1);

        unsigned int aux = perm[i];
        perm[i] = perm[j];
        perm[j] = aux;
    }

    unsigned int * invperm = (unsigned int * ) malloc(sizeof(unsigned int) * dim_bmp);
    for (int i = 0; i < dim_bmp; i++)
        invperm[perm[i]] = i;

    free(perm);

    unsigned int * decrypted = (unsigned int * ) malloc(sizeof(unsigned int) * dim_bmp);
    decrypted[0] = SV ^ bmp.linearized_bmp[0] ^ R[dim_bmp];
    for (int i = 1; i < dim_bmp; i++)
        decrypted[i] = bmp.linearized_bmp[i - 1] ^ bmp.linearized_bmp[i] ^ R[dim_bmp + i];

    unsigned int * res = (unsigned int * ) malloc(sizeof(unsigned int) * dim_bmp);
    for (int i = 0; i < dim_bmp; i++)
        res[invperm[i]] = decrypted[i];

    free(R);
    free(invperm);
    free(decrypted);

    return res;
}

void write_decrypted(char * source_file, char * decrypted_file, char * key_file) {
    Bitmap encrypted = load_bitmap(source_file);

    unsigned int * dec = decrypt(encrypted, key_file);

    Bitmap decrypted = encrypted;
    decrypted.linearized_bmp = dec;

    write_bmp(decrypted, decrypted_file);
    free(dec);
}


void print_chi_square(char * source_file) {
    //Bitmap bmp=load_bitmap(source_file);
    FILE * fin = fopen(source_file, "rb");
    unsigned int image_width, image_height;

    fseek(fin, 18, SEEK_SET);
    fread( & image_width, sizeof(unsigned int), 1, fin);
    fread( & image_height, sizeof(unsigned int), 1, fin);

    fclose(fin);

    double fBar = image_width * image_height / 256.0;
    unsigned int * freq = (unsigned int * ) malloc(256 * sizeof(unsigned int));

    for (int color = 2; color >= 0; color--) {
        FILE * fin = fopen(source_file, "rb");
        fseek(fin, 54, SEEK_CUR);

        for (int i = 0; i < 256; i++)
            freq[i] = 0;

        for (int i = 0; i < image_width * image_height; i++) {
            unsigned char pRGB[3];
            fread(pRGB, 3 * sizeof(unsigned char), 1, fin);
            freq[pRGB[color]]++;
        }

        double chi = 0;
        for (int i = 0; i < 256; i++) {
            chi += (double)(freq[i] - fBar) * (freq[i] - fBar) / fBar;
        }

        printf("%f\n", chi);

        fclose(fin);
    }

    free(freq);
}
