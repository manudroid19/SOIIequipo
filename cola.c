#include <stdlib.h>
#include <stdio.h>

struct celdaCola{
    int elemento;
    struct celdaCola * sig;
};

typedef struct celdaCola * puntero;

typedef struct{
	puntero frente;
	puntero final;
}tipocola;

typedef tipocola * cola;

void creaCola(cola *C)
{
    *C=(cola)malloc(sizeof(tipocola));
    if (C == NULL)
      printf("Error: Memoria insuficiente\n");
    else{
        (*C)->frente=(puntero)malloc(sizeof(struct celdaCola));
        if ((*C)->frente == NULL)
             printf("Error: Memoria insuficiente\n");
        else{
            (*C)->final=(*C)->frente;
            (*C)->frente->sig=NULL;
        }
    }
}

void destruyeCola(cola *C)
{
   puntero p,r;
	p=(*C)->frente;
	while(p!=NULL)
	{
		r=p;
		p=p->sig;
		free(r);
	}
	free(*C);
}

unsigned esVaciaCola(cola C)
{
    return (C->frente==C->final);
}

int primero(cola C)
{
    if (!esVaciaCola(C))
	return (C->frente)->sig->elemento;
    else
        printf("Error: cola vacía\n");
    return -1;
}

void insertarCola(cola *C, int E)
{
    ((*C)->final)->sig=(puntero)malloc(sizeof(struct celdaCola));
    if ((*C)->final->sig == NULL)
        printf("Error: Memoria insuficiente\n");
    else{
        (*C)->final=((*C)->final)->sig;
        ((*C)->final)->elemento=E;
   // printf("Insertado %d\n",E);
        ((*C)->final)->sig=NULL;
    }
}


void suprimirCola(cola *C)
{
    puntero p;
    if (esVaciaCola(*C))
        printf("Error: Cola vacia\n");
    else{
        p=(*C)->frente;
	(*C)->frente=(*C)->frente->sig;
	free(p);
    }
}
