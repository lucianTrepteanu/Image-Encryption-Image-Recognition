#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

typedef struct Bitmap{
    unsigned int image_width;
    unsigned int image_height;
    unsigned char *header;
    unsigned int *linearized_bmp; //pixelii sub forma linearizata.
}Bitmap;

int get_padding(int); //furnizeaza paddingul imaginii, primint ca parametru latimea.

Bitmap load_bitmap(char *); //Returneaza o structura de tip Bitmap pe baza fisierului
                            //de la calea data ca parametru.

void free_bitmap(Bitmap); //Elibereaza memoria alocata dinamic in cadrul unui Bitmap.

void write_bmp(Bitmap, char *); //Scrie in memoria externa imaginea retinuta intr-un Bitmap.
                                //Primeste ca parametrii Bitmapul ce urmeaza sa fie scris si
                                //calea la care se va scrie imaginea.
unsigned int** load_array2D(Bitmap); //Incarca sub forma unei matrice continutul liniarizat
                                //al unei imagini Bitmap. Primeste ca parametru Bitmapul respectiv.
#endif //COMMON_H_INCLUDED
