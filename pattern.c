#include <math.h>

#ifndef CRYPT_H_INCLUDED
#include "crypt.c"
#endif

#ifndef PATTERN_H_INCLUDED
#include "pattern.h"
#endif

void write_grayscale(char * source_file, char * destination_file) {
    Bitmap res;
    Bitmap bmp = load_bitmap(source_file);

    res.image_width = bmp.image_width;
    res.image_height = bmp.image_height;
    res.header = bmp.header;
    res.linearized_bmp = (unsigned int * ) malloc(res.image_height * res.image_width * sizeof(unsigned int));

    for (int i = 0; i < bmp.image_width * bmp.image_height; i++) {
        unsigned char pRGB[3];
        pRGB[0] = (bmp.linearized_bmp[i] >> 16) & 255;
        pRGB[1] = (bmp.linearized_bmp[i] >> 8) & 255;
        pRGB[2] = (bmp.linearized_bmp[i]) & 255;

        unsigned char aux = 0.299 * pRGB[2] + 0.587 * pRGB[1] + 0.114 * pRGB[0];
        pRGB[0] = pRGB[1] = pRGB[2] = aux;

        res.linearized_bmp[i] = pRGB[0];
        res.linearized_bmp[i] |= (pRGB[1] << 8);
        res.linearized_bmp[i] |= (pRGB[2] << 16);
    }

    write_bmp(res, destination_file);

    free_bitmap(res);
}

Window * cross_corelation(Bitmap image, Bitmap templateImg, int * howMany, double prag, int whichTemplate) {
    unsigned int ** array_image = load_array2D(image);
    unsigned int ** array_template = load_array2D(templateImg);

    unsigned int templateImg_size = templateImg.image_height * templateImg.image_width;

    double templateImg_average = 0;
    for (int i = 0; i < templateImg.image_height; i++) {
        for (int j = 0; j < templateImg.image_width; j++) {
            templateImg_average += (array_template[i][j] & 255);
        }
    }
    templateImg_average /= templateImg_size;

    double templateImg_deviation = 0;
    for (int i = 0; i < templateImg.image_height; i++) {
        for (int j = 0; j < templateImg.image_width; j++) {
            templateImg_deviation += ((array_template[i][j] & 255) - templateImg_average) * ((array_template[i][j] & 255) - templateImg_average);
        }
    }

    templateImg_deviation /= (templateImg_size - 1);
    templateImg_deviation = sqrt(templateImg_deviation);

    unsigned int arraySize = (image.image_height - templateImg.image_height);
    arraySize *= (image.image_width - templateImg.image_width);

    int idx = 0;
    Window * res = (Window * ) malloc(arraySize * sizeof(Window));
    for (int i = 0; i < image.image_height - templateImg.image_height; i++) {
        for (int j = 0; j < image.image_width - templateImg.image_width; j++) {

            Window window;
            window.pxSize = templateImg_size;
            window.up_line = i;
            window.left_line = j;
            window.down_line = window.up_line + templateImg.image_height;
            window.right_line = window.left_line + templateImg.image_width;

            window.average = 0;
            window.deviation = 0;

            for (int p = window.up_line; p < window.down_line; p++) {
                for (int q = window.left_line; q < window.right_line; q++) {
                    window.average += (array_image[p][q] & 255);
                }
            }
            window.average /= window.pxSize;

            for (int p = window.up_line; p < window.down_line; p++) {
                for (int q = window.left_line; q < window.right_line; q++) {
                    window.deviation += ((array_image[p][q] & 255) - window.average) * ((array_image[p][q] & 255) - window.average);
                }
            }
            window.deviation /= (window.pxSize - 1);
            window.deviation = sqrt(window.deviation);

            window.correlation = 0;
            for (int p = window.up_line; p < window.down_line; p++) {
                for (int q = window.left_line; q < window.right_line; q++) {
                    window.correlation += ((array_image[p][q] & 255) - window.average) * ((array_template[p - i][q - j] & 255) - templateImg_average) / (templateImg_deviation * window.deviation);
                }
            }
            window.correlation /= templateImg_size;
            if (window.correlation > prag) {
                window.whichTemplate = whichTemplate;
                res[idx++] = window;
            }
        }
    }

    ( * howMany) = idx;
    res = realloc(res, idx * sizeof(Window));

    for (int i = 0; i < image.image_height; i++)
        free(array_image[i]);
    free(array_image);

    for (int i = 0; i < templateImg.image_height; i++)
        free(array_template[i]);
    free(array_template);

    return res;
}

void draw_single(Bitmap * bmp, Window window, unsigned int color) {
    unsigned int ** array_image = load_array2D( * bmp);

    for (int i = window.left_line; i < window.right_line; i++) {
        array_image[window.up_line][i] = color;
        array_image[window.down_line][i] = color;
    }

    for (int i = window.up_line; i < window.down_line; i++) {
        array_image[i][window.left_line] = color;
        array_image[i][window.right_line] = color;
    }

    int idx = 0;
    for (int i = ( * bmp).image_height - 1; i >= 0; i--) {
        for (int j = 0; j < ( * bmp).image_width; j++) {
            ( * bmp).linearized_bmp[idx++] = array_image[i][j];
        }
    }

    for (int i = 0; i < ( * bmp).image_height; i++) {
        free(array_image[i]);
    }
    free(array_image);
}

void draw_rectangles(Bitmap * bmp, Window * windows, int howMany, unsigned int *colors) {
    unsigned int ** array_image = load_array2D( * bmp);

    for (int step = 0; step < howMany; step++) {
        unsigned int color = colors[windows[step].whichTemplate];
        for (int i = windows[step].left_line; i < windows[step].right_line; i++) {
            array_image[windows[step].up_line][i] = color;
            array_image[windows[step].down_line][i] = color;
        }

        for (int i = windows[step].up_line; i < windows[step].down_line; i++) {
            array_image[i][windows[step].left_line] = color;
            array_image[i][windows[step].right_line] = color;
        }
    }

    int idx = 0;
    for (int i = ( * bmp).image_height - 1; i >= 0; i--) {
        for (int j = 0; j < ( * bmp).image_width; j++) {
            ( * bmp).linearized_bmp[idx++] = array_image[i][j];
        }
    }

    for (int i = 0; i < ( * bmp).image_height; i++) {
        free(array_image[i]);
    }
    free(array_image);
}

int cmp(const void * a, const void * b) {
    if (((Window * ) a)->correlation < ((Window * ) b)->correlation)
        return 1;
    return -1;
}

void sort_detections(Window * arr, int array_sz) {
    qsort(arr, array_sz, sizeof(Window), cmp);
}

int maxim(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int minim(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

_Bool xIntersect(Window a, Window b) {
    return ((a.up_line >= b.up_line && a.up_line <= b.down_line) ||
        (b.up_line >= a.up_line && b.up_line <= a.down_line));
}

_Bool yIntersect(Window a, Window b) {
    return ((a.left_line >= b.left_line && a.left_line <= b.right_line) ||
        (b.left_line >= a.left_line && b.left_line <= a.right_line));
}

_Bool intersects(Window a, Window b) {
    if (xIntersect(a, b) && yIntersect(a, b)) {
        return 1;
    }
    return 0;
}

Window get_intersection(Window a, Window b) {
    if (intersects(a, b)) {
        Window intersection;

        intersection.up_line = maxim(a.up_line, b.up_line);
        intersection.down_line = minim(a.down_line, b.down_line);
        intersection.left_line = maxim(a.left_line, b.left_line);
        intersection.right_line = minim(a.right_line, b.right_line);

        return intersection;
    }
}

Window * non_maximal_erase(Window * allCorel, int len, double minSupr, int * newLen) {
    sort_detections(allCorel, len);

    _Bool * marked = (_Bool * ) malloc(len * sizeof(_Bool));
    for (int i = 0; i < len; i++)
        marked[i] = 0;

    for (int i = 0; i < len; i++) {
        if (!marked[i])
            for (int j = i + 1; j < len; j++)
                if (!marked[j]) {
                    if (intersects(allCorel[i], allCorel[j])) {
                        Window intersection = get_intersection(allCorel[i], allCorel[j]);

                        double areaBoth = (intersection.down_line - intersection.up_line) * (intersection.right_line - intersection.left_line);
                        double areaI = (allCorel[i].down_line - allCorel[i].up_line) * (allCorel[i].right_line - allCorel[i].left_line);
                        double areaJ = (allCorel[j].down_line - allCorel[j].up_line) * (allCorel[j].right_line - allCorel[j].left_line);

                        double suprapunere = areaBoth / (areaI + areaJ - areaBoth);

                        if (suprapunere > minSupr) {
                            marked[j] = 1;
                        }
                    }
                }
    }

    int aux = 0;
    Window * rectangles = (Window * ) malloc(len * sizeof(Window));
    for (int i = 0; i < len; i++) {
        if (!marked[i]) {
            rectangles[aux++] = allCorel[i];
        }
    }

    free(marked);

    ( * newLen) = aux;
    rectangles = realloc(rectangles, aux * sizeof(Window));
    return rectangles;
}
