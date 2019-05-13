#ifndef COLA_H
#define COLA_H

//Interfaz TAD cola

typedef void *cola; /*tipo opaco*/

//Funciones de creación/destrucción
void creaCola(cola *C);
void destruyeCola(cola *C);

//Funciones de información
unsigned esVaciaCola(cola C);
int primero(cola C);

//Funciones de inserción/eliminación
void insertarCola(cola *C, int E);
void suprimirCola(cola *C);

#endif	// COLA_H
