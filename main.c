/*
    TODO: Updatat Documentatie2.
    TODO: Valgrind?
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define PRAG_SUPR 0.2

#include "common.h"
#include "common.c"

#include "crypt.h"
#include "crypt.c"

#include "pattern.h"
#include "pattern.c"

void encryption() {
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
}

void patternMatching() {
    FILE * fPaths = fopen("matchingPaths.txt", "r");

    char * test_color = (char * ) malloc(200 * sizeof(char));
    fscanf(fPaths, "%s\n", test_color);
    test_color = realloc(test_color, (1 + strlen(test_color)) * sizeof(char));

    char * test_gray = (char * ) malloc(200 * sizeof(char));
    fscanf(fPaths, "%s\n", test_gray);
    test_gray = realloc(test_gray, (1 + strlen(test_gray)) * sizeof(char));

    char * dest = (char * ) malloc(200 * sizeof(char));
    fscanf(fPaths, "%s\n", dest);
    dest = realloc(dest, (1 + strlen(dest)) * sizeof(char));

    int cntTemplates;
    fscanf(fPaths, "%d\n", & cntTemplates);
    char ** templates = (char ** ) malloc(cntTemplates * sizeof(char * ));
    for (int i = 0; i < cntTemplates; i++) {
        templates[i] = (char * ) malloc(200 * sizeof(char));
        fscanf(fPaths, "%s\n", templates[i]);
        templates[i] = realloc(templates[i], (1 + strlen(templates[i])) * sizeof(char));
    }

    char ** templates_gray = (char ** ) malloc(cntTemplates * sizeof(char * ));

    char * aux = (char * ) malloc(200 * sizeof(char));
    for (int i = 0; i < cntTemplates; i++) {
        strncpy(aux, templates[i], strlen(templates[i]) - 4);
        aux[strlen(templates[i]) - 4] = '\0';
        strcat(aux, "GRAY.bmp");

        templates_gray[i] = (char * ) malloc((1 + strlen(aux) * sizeof(char)));
        strcpy(templates_gray[i], aux);
    }

    free(aux);

    for (int i = 0; i < cntTemplates; i++) {
        write_grayscale(templates[i], templates_gray[i]);
    }

    unsigned int * colors = (unsigned int * ) malloc(cntTemplates * sizeof(unsigned int));

    for (int i = 0; i < cntTemplates; i++) {
        unsigned char x;
        colors[i] = 0;
        for (int j = 0; j < 3; j++) {
            fscanf(fPaths, "%d", & x);
            colors[i] = (colors[i] << 8) | x;
        }
    }

    write_grayscale(test_color, test_gray);

    Bitmap imagine = load_bitmap(test_gray);
    Window * allCorel = (Window * ) malloc(cntTemplates * imagine.image_height * imagine.image_width * sizeof(Window));

    int idx = 0;
    for (int step = 0; step < cntTemplates; step++) {
        int howMany = 0;
        double prag = 0.5;
        Bitmap template_image = load_bitmap(templates_gray[step]);
        Window * fer = cross_corelation(imagine, template_image, & howMany, prag, step);

        for (int i = 0; i < howMany; i++) {
            allCorel[idx++] = fer[i];
        }
        free_bitmap(template_image);

        free(fer);
    }
    allCorel = realloc(allCorel, idx * sizeof(Window));

    int len = 0;
    Window * rectangles = non_maximal_erase(allCorel, idx, PRAG_SUPR, & len);

    Bitmap color_image = load_bitmap(test_color);
    draw_rectangles( & color_image, rectangles, len, colors);

    write_bmp(color_image, dest);

    free_bitmap(imagine);

    free(allCorel);
    free(test_color);
    free(test_gray);
    free(dest);

    for (int i = 0; i < cntTemplates; i++) {
        free(templates[i]);
        free(templates_gray[i]);
    }

    free(colors);
    free(templates);
    free(templates_gray);
}

int main() {
    encryption();
    patternMatching();

    return 0;
}
