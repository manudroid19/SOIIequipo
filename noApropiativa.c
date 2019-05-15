#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <fcntl.h>

#define Tmin 0 // Tiempo mínimo de espera tras adquirir recurso
#define Tmax 2 // Tiempo máximo de espera tras adquirir recurso
#define Nmax 4 // Recursos
#define Pmax 3 // Threads

/*En el grafo los Pmax primeros elementos de la matriz son los threads y los
* Nmax elementos siguientes son los recursos.
*/
int G[Nmax + Pmax][Nmax + Pmax], visited[Nmax + Pmax];
int visitados = 0;
int filaModif; //Almaceno el último hilo que solicitó un recurso
int colModif; //Almaceno el último recurso solicitado por un hilo

void deshacerCiclo(int i, int j);
int hayCiclo();
int busquedaIguales(int *nodosVisitados);
void DFS(int i, int *nodosVisitados);
void imprimirGrafo();

sem_t *comprobador;
pthread_mutex_t recurso[Nmax], accesoGrafo;

void * trabajo(void * tid) { // Código para cada thread
  srand(time(NULL));
  int numRecursos = (int)(Nmax) * (rand() / (RAND_MAX + 1.0)) + 1;
  int thid, i, j, k, ya, mis_recursos[numRecursos];
  double x = 0;
  thid = (intptr_t) tid;
  srand(thid + time(NULL));
  comprobador = sem_open("Comprobador", O_CREAT, S_IRWXU, 0);

  printf("*********Soy %d y quiero obtener %d recursos\n", thid, numRecursos);

  for (i = 0; i < numRecursos; i++) { // Adquiero nuevos recursos
    ya = 0;

    while (ya == 0) { // Selecciono un recurso que no tengo
      j = (int)(Nmax) * (rand() / (RAND_MAX + 1.0));
      ya = 1;
      for (k = 0; k < i; k++)
        if (mis_recursos[k] == j) ya = 0;
    }

    mis_recursos[i] = j; // Incluyo recurso en mi lista
    pthread_mutex_lock(&accesoGrafo);
    printf("Soy %d y quiero el recurso %d\n", thid, j + Pmax);
    G[thid][j + Pmax] = 1; //Se añade una arista desde el hilo thid hasta el recurso j
    filaModif = thid;
    colModif = j;
    //imprimirGrafo();
    sem_post(comprobador);
    pthread_mutex_unlock(&accesoGrafo);

    pthread_mutex_lock( & recurso[j]); // Adquiero el recurso
    pthread_mutex_lock(&accesoGrafo);
    printf(" Soy %d y tengo el recurso %d\n", thid, j + Pmax);
    G[thid][j + Pmax] = 0;
    G[j + Pmax][thid] = 1; //Se añade una arista desde el  recurso j hasta el hilo thid
    //imprimirGrafo();
    pthread_mutex_unlock(&accesoGrafo);

    for (k = 0; k < 10000; k++) x += sqrt(sqrt(k + 0.1)); // Trabajo intrascendente
    k = (int) Tmin + (Tmax - Tmin + 1) * (rand() / (RAND_MAX + 1.0)) + 1;
    sleep(k); // Espero un tiempo aleatorio
  }
  for (i = 0; i < numRecursos; i++){
    pthread_mutex_unlock( & recurso[mis_recursos[i]]);
    G[i + Pmax][thid] = 0;
  }

  printf("************ACABE! Soy %d\n", thid);

  pthread_exit(NULL);
}

void *hiloComprobador(void *tid){
  comprobador = sem_open("Comprobador", O_CREAT, S_IRWXU, 0);

  while(1){
    sem_wait(comprobador);
    pthread_mutex_lock(&accesoGrafo);
    if(hayCiclo())
      deshacerCiclo(filaModif, colModif);
    pthread_mutex_unlock(&accesoGrafo);
  }
  pthread_exit(NULL);
}

int main() {
  int i;
  pthread_t th[Pmax], fio;
  int tiempo_ini, tiempo_fin; // Para medir tiempo

  for(i=0; i< Nmax + Pmax; i++){
    for (int j = 0; j < Nmax + Pmax; j++) {
      G[i][j]=0;
    }
    visited[i]=0;
  }

  sem_unlink("Comprobador");

  for (i = 0; i < Nmax; i++) pthread_mutex_destroy( & recurso[i]);
  pthread_mutex_destroy(&accesoGrafo);
  for (i = 0; i < Nmax; i++) pthread_mutex_init( & recurso[i], NULL);
  pthread_mutex_init(&accesoGrafo, NULL);

  tiempo_ini = time(NULL);
  pthread_create(&fio, NULL, hiloComprobador, (void *) (intptr_t) i);
  for (i = 0; i < Pmax; i++) pthread_create( & th[i], NULL, trabajo, (void * ) (intptr_t) i);
  for (i = 0; i < Pmax; i++) pthread_join(th[i], NULL);
  tiempo_fin = time(NULL);

  printf("Acabado en %d segundos \n", tiempo_fin - tiempo_ini);
  for (i = 0; i < Nmax; i++) pthread_mutex_destroy( & recurso[i]);
  
  return EXIT_SUCCESS;
}

void deshacerCiclo(int i, int j){
  printf("Libero el recurso %d \n", j + Pmax);
  for(int k=0; k < Pmax; k++)
    G[j + Pmax][k] = 0;
  pthread_mutex_unlock(&recurso[j]);
}

int hayCiclo(){
  int *nodosVisitados;
  nodosVisitados = (int *)malloc((Nmax + Pmax)*sizeof(int));
  for(int i=0; i < Nmax + Pmax; i++)
    nodosVisitados[i] = -1;

  nodosVisitados[visitados] = filaModif;
  visitados++;
  DFS(filaModif, nodosVisitados);
  for(int i=0; i < Nmax + Pmax; i++)
    visited[i] = 0;

  /*printf("filaModif %d, visitados %d, nodosVisitados: ", filaModif, visitados);
  for (int i = 0; i < Nmax + Pmax; i++)
    printf("%d ", nodosVisitados[i]);
  */
  printf("\n");

  if(busquedaIguales(nodosVisitados)){
    visitados = 0;
    free(nodosVisitados);
    return 1;
  }
  visitados = 0;
  free(nodosVisitados);
  return 0;
}

int busquedaIguales(int *nodosVisitados){
  for(int i=0; i < visitados; i++){
    for (int j = i+1; j < visitados; j++) {
        if(nodosVisitados[i] == nodosVisitados[j])
          return 1;
    }
  }
  return 0;
}

void DFS(int i, int *nodosVisitados){
  int j;
  visited[i]=1;
	for(j=0;j<Nmax + Pmax;j++){
    if(G[i][j]==1){
      nodosVisitados[visitados] = j; //Almaceno los nodos a los que se puede ir desde i
      visitados++; //Mantengo un registro de cuantos nodos he podido visitar
      if(!visited[j])
        DFS(j, nodosVisitados);
    }
  }
}

void imprimirGrafo(){

  for (int i = 0; i < Pmax + Nmax; i++) {
    for (int j = 0; j < Pmax + Nmax; j++) {
      printf("%d ", G[i][j]);
    }
    printf("\n");
  }

}
