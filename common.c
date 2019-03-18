#include "common.h"

int get_padding(int image_width) {
    int padding = 0;
    if (image_width % 4 != 0) {
        padding = 4 - (3 * image_width) % 4;
    }

    return padding;
}

Bitmap load_bitmap(char * source_file) {
    Bitmap bmp;
    FILE * fin = fopen(source_file, "rb");

    bmp.header = (unsigned char * ) malloc(54 * sizeof(unsigned char));

    for (int i = 0; i < 54; i++) {
        unsigned char current;
        fread( & current, 1, 1, fin);
        bmp.header[i] = current;
    }

    fseek(fin, 18, SEEK_SET);
    fread( & bmp.image_width, sizeof(unsigned int), 1, fin);
    fread( & bmp.image_height, sizeof(unsigned int), 1, fin);

    int padding = get_padding(bmp.image_width);
    //printf("%d %d %d",padding,bmp.latime_imagine,bmp.inaltime_imagine);

    int actPos = 0;
    bmp.linearized_bmp = (unsigned int * ) malloc(sizeof(unsigned int) * bmp.image_width * bmp.image_height);

    fseek(fin, 54, SEEK_SET);

    for (int i = 0; i < bmp.image_height; i++) {
        for (int j = 0; j < bmp.image_width; j++) {
            unsigned char pRGB[3];
            fread(pRGB, 3 * sizeof(unsigned char), 1, fin);

            unsigned int current = pRGB[0];
            current |= (pRGB[1] << 8);
            current |= (pRGB[2] << 16);

            bmp.linearized_bmp[actPos++] = current;
        }
        if (padding != 0) {
            fseek(fin, padding, SEEK_CUR);
        }
    }

    fclose(fin);

    return bmp;
}


void free_bitmap(Bitmap bmp) {
    free(bmp.header);
    free(bmp.linearized_bmp);
}


void write_bmp(Bitmap bmp, char * dest_file) {
    FILE * fout;
    fout = fopen(dest_file, "wb+");

    fwrite(bmp.header, sizeof(unsigned char), 54, fout);

    int padding = get_padding(bmp.image_width);

    for (int i = 0; i < bmp.image_height; i++) {
        for (int j = 0; j < bmp.image_width; j++) {
            int idx = i * bmp.image_width + j;

            unsigned char rByte = (bmp.linearized_bmp[idx] >> 16) & 255;
            unsigned char gByte = (bmp.linearized_bmp[idx] >> 8) & 255;
            unsigned char bByte = bmp.linearized_bmp[idx] & 255;

            fwrite( & bByte, sizeof(unsigned char), 1, fout);
            fwrite( & gByte, sizeof(unsigned char), 1, fout);
            fwrite( & rByte, sizeof(unsigned char), 1, fout);
        }
        if (padding != 0) {
            unsigned char aux = 0;
            for (int p = 0; p < padding; p++) {
                fwrite( & aux, sizeof(unsigned char), 1, fout);
            }
            //fseek(fout,padding,SEEK_CUR);
        }
    }

    fclose(fout);
}


unsigned int ** load_array2D(Bitmap bmp) {
    unsigned int ** res = (unsigned int ** ) malloc(bmp.image_height * bmp.image_width * sizeof(unsigned int * ));

    for (int i = 0; i < bmp.image_height; i++)
        res[i] = (unsigned int * ) malloc(bmp.image_width * sizeof(unsigned int));

    int idx = 0;
    for (int i = bmp.image_height - 1; i >= 0; i--) {
        for (int j = 0; j < bmp.image_width; j++) {
            res[i][j] = bmp.linearized_bmp[idx];
            idx++;
        }
    }

    return res;
}
