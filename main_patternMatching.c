#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef COMMON_H_INCLUDED
#include "common.h"
#include "common.c"
#endif //COMMON_H_INCLUDED

#ifndef CRYPT_H_INCLUDED
#include "crypt.h"
#include "crypt.c"
#endif // CRYPT_H_INCLUDED

#ifndef PATTERN_H_INCLUDED
#include "pattern.h"
#include "pattern.c"
#endif // PATTERN_H_INCLUDED

# define PRAG_SUPR 0.2

int main() {
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

    Bitmap image = load_bitmap(test_gray);
    Window * allCorel = (Window * ) malloc(cntTemplates * image.image_height * image.image_width * sizeof(Window));

    int idx = 0;
    for (int step = 0; step < cntTemplates; step++) {
        int howMany = 0;
        double prag = 0.5;
        Bitmap template_image = load_bitmap(templates_gray[step]);
        Window * fer = cross_corelation(image, template_image, & howMany, prag, step);

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

    free_bitmap(image);

    free(test_color);
    free(test_gray);
    free(dest);

    for (int i = 0; i < cntTemplates; i++) {
        free(templates[i]);
        free(templates_gray[i]);
    }

    free(allCorel);
    free(colors);
    free(templates);
    free(templates_gray);

    return 0;
}
