#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

#define Tmin 0 // Tiempo mínimo de espera tras adquirir recurso
#define Tmax 2 // Tiempo máximo de espera tras adquirir recurso
#define Nmax 5 // Recursos
#define Pmax 3 // Threads

pthread_mutex_t recurso[Nmax];

void * trabajo(void * tid) { // Código para cada thread
  int thid, Ri;
  thid = (intptr_t) tid;
  srand(thid + time(NULL));
  Ri = (int)(Nmax) * (rand() / (RAND_MAX + 1.0)); //Numero de recursos aleatorios <=N
  int i, j, k, ya, mis_recursos[Ri];
  double x = 0;

  for (i = 0; i < Ri; i++) { // Adquiero nuevos recursos
    ya = 0;
    while (ya == 0) { // Selecciono un recurso que no tengo
      j = (int)(Nmax) * (rand() / (RAND_MAX + 1.0));
      ya = 1;
      for (k = 0; k < i; k++)
        if (mis_recursos[k] == j) ya = 0;
    }
    mis_recursos[i] = j; // Incluyo recurso en mi lista
    printf("Soy %d y quiero el recurso %d\n", thid, j);
    pthread_mutex_lock( & recurso[j]); // Adquiero el recurso
    printf(" Soy %d y tengo el recurso %d\n", thid, j);
    for (k = 0; k < 10000; k++) x += sqrt(sqrt(k + 0.1)); // Trabajo intrascendente
    k = (int) Tmin + (Tmax - Tmin + 1) * (rand() / (RAND_MAX + 1.0)) + 1;
    sleep(k); // Espero un tiempo aleatorio
  }
  for (i = 0; i < Nmax; i++) pthread_mutex_unlock( & recurso[mis_recursos[i]]);
  printf("************ACABE! Soy %d\n", thid);
  pthread_exit(NULL);
}
int main() {
  int i;
  pthread_t th[Pmax];
  int tiempo_ini, tiempo_fin; // Para medir tiempo
  for (i = 0; i < Nmax; i++) pthread_mutex_destroy( & recurso[i]);
  for (i = 0; i < Nmax; i++) pthread_mutex_init( & recurso[i], NULL);
  tiempo_ini = time(NULL);
  for (i = 0; i < Pmax; i++) pthread_create( & th[i], NULL, trabajo, (void * ) (intptr_t) i);
  for (i = 0; i < Pmax; i++) pthread_join(th[i], NULL);
  tiempo_fin = time(NULL);
  printf("Acabado en %d segundos \n", tiempo_fin - tiempo_ini);
  for (i = 0; i < Nmax; i++) pthread_mutex_destroy( & recurso[i]);
  return EXIT_SUCCESS;
}
