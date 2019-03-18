#ifndef PATTERN_H_INCLUDED
#define PATTERN_H_INCLUDED

#ifndef CRYPT_H_INCLUDED
#include "crypt.h"
#endif

typedef struct Window{
    int up_line;
    int down_line;
    int left_line;
    int right_line;
    int pxSize;

    double average; //media intensitatilor grayscale.
    double deviation; //deviatia standard a intensitatilor.
    double correlation; //valoarea corelatiei cu sablonul respectiv.

    int whichTemplate; //Cifra (sablonul ei) cu care se obtine corelatia.
}Window;

void write_grayscale(char*, char*); //Scrie o imaginea obtinuta in urma transformarii
                                //grayscale.Primeste ca parametrii calea imaginii intiale
                                //si calea unde se va scrie imaginea in grayscale.

int cmp(const void *, const void *); //functie de comparare pentru sortarea ferestrelor
                                    //descrescator in functie de corelatie.
                                    //Primeste ca parametrii 2 pointeri generici.

void sort_detections(Window*, int); //Sorteaza un vector de Ferestre in functie de corelatie.
                                    //Primeste ca parametrii adresa de inceput a vectorului
                                    //precum si dimensiunea acestuia.

int maxim(int, int); //functie care returneaza maximul dintre 2 numere intregi reprezentate de parametrii.

int minim(int, int); //functie care returneaza minimul dintre 2 numere intregi reprezentate de parametrii.

_Bool xIntersect(Window, Window); //Stabileste daca 2 ferestre s-ar intersecta pe axa OX.

_Bool yIntersect(Window, Window); //Stabileste daca 2 ferestre s-ar intersecta pe axa OY.

_Bool intersects(Window, Window); //Stabileste daca 2 ferestre se intersecteaza.

void draw_single(Bitmap *, Window, unsigned int); //O functie care deseneaza o Window intr-un Bitmap.
                                                //Din motive de performanta in realizarea proiectului s-a
                                                //renuntat la utilizarea acesteia. Functia primeste ca
                                                //parametrii Bitmapul peste care se deseneaza, Fereastra
                                                //ce trebuie desenata precum si culoarea acesteia.

void draw_rectangles(Bitmap *, Window *, int,unsigned int*); //Deseneaza intr-un Bitmap
                                                //mai multe Ferestre simultan. Din motive de performanta
                                                //s-a ales sa nu se deseneze Ferestrele una cate una.
                                                //Primeste ca parametrii Bitmapul pe care se deseneaza
                                                //adresa vectorului de ferestre de desenat
                                                //dimensiunea acestuia
                                                //vectorul de culori ce vor fi folosite pentru fiecare sablon.

Window *cross_corelation(Bitmap, Bitmap, int *, double, int); //Functie care returneaza tabloul tuturor ferestrelor
                                                //care au corelatie mai mare ca pragul cu un anume sablon.
                                                //Functia primeste ca parametrii Bitmapul imaginii pe care se cauta
                                                //matchingurile, Bitmapul sablonului curent, dimensiunea vectorului
                                                //returnat, valoarea pragului Ps, cifra (indicele sablonului) cu care
                                                //se cauta matchingurile.

Window get_intersection(Window, Window); //Intoarce fereastra rezultata prin aplicarea operatiei de intersectie
                                                //asupra celor 2 ferestre furnizate de parametrii.

Window* non_maximal_erase(Window *, int, double, int*); //Implementarea algoritmului de eliminare a non-maximelor.
                                                //Intoarce vectorul de Ferestre rezultat dupa eliminare.Primeste ca
                                                //parametrii vectorul de ferestre pe care se aplica eliminarile,
                                                //dimensiunea acestuia, pragul de considerare al suprapunerilor
                                                //si dimensiunea vectorului rezultat.

#endif // PATTERN_H_INCLUDED
