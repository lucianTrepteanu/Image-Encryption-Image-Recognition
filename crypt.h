#ifndef CRYPT_H_INCLUDED
#define CRYPT_H_INCLUDED

unsigned int xor_shift(unsigned int*); //genereaza cate un numar pseudo-aleator.


unsigned int *encrypt(Bitmap, char *); //Furnizeaza forma liniarizata a
                                    //unui Bitmap in urma criptarii. Primeste
                                    //ca parametrii Bitmapul ce trebuie criptat
                                    //path-ul fisierului ce contine cheile

unsigned int *decrypt(Bitmap, char *); //Furnizeaza forma liniarizata a unui Bitmap
                                    //in urma decriptarii. Primeste ca parametrii
                                    //Bitmapul ce trebuie decriptat si path-ul
                                    //fisierului ce contine cheile.

void write_encrypted(char *, char *, char *); //Scrie imaginea obtinuta in urma criptarii
                                        //Primeste ca parametrii path-ul imaginii initiale
                                        //path-ul imaginii criptate si path-ul fisierului
                                        //ce contine cheile

void write_encrypted(char *, char *, char *); //Scrie imaginea obtinuta in urma decriptarii
                                        //Primeste ca parametrii path-ul imaginii initiale
                                        //(criptate) path-ul imaginii decriptate si path-ul
                                        //fisierului ce contine cheile.


void print_chi_square(char *); //Afiseaza valorile testului Chi-square
                                                    //pe fiecare canal de culoare (ordinea RGB)
                                                    //pentru o imagine. Primeste ca parametrii
                                                    //inaltimea,latimea precum si path-ul imaginii.


#endif // CRYPT_H_INCLUDED
