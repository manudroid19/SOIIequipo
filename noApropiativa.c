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

//Grafo y vector auxiliar para el algoeritmo DFS
int G[Nmax + Pmax][Nmax + Pmax], visited[Nmax + Pmax];
int visitados = 0;
int filaModif; //Almaceno el último hilo que solicitó un recurso
int colModif; //Almaceno el último recurso solicitado por un hilo

//Funciones del grafo
void deshacerCiclo(int i, int j);
int hayCiclo();
int busquedaIguales(int *nodosVisitados);
void DFS(int i, int *nodosVisitados);
void imprimirGrafo();

//Semáforo usado en el hilo comprobador y mutexes de recursos y de acceso al grafo
sem_t *comprobador;
pthread_mutex_t recurso[Nmax], accesoGrafo;


void * trabajo(void * tid) { // Código para cada thread
  //Iniciación de las variables necesarias y sorteo de los recursos que solicitará
  //cada hilo
  srand(time(NULL));
  int numRecursos = (int)(Nmax) * (rand() / (RAND_MAX + 1.0)) + 1;
  int thid, i, j, k, ya, mis_recursos[numRecursos];
  double x = 0;
  thid = (intptr_t) tid;
  srand(thid + time(NULL));

  //Iniciación del semáforo del hilo comprobador
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

    //Acceso al grafo para añadir el recurso deseado
    pthread_mutex_lock(&accesoGrafo);
    printf("Soy %d y quiero el recurso %d\n", thid, j + Pmax);
    G[thid][j + Pmax] = 1; //Se añade una arista desde el hilo thid hasta el recurso j
    filaModif = thid;
    colModif = j;

    //Se permite al hilo comprobador que trabaje
    sem_post(comprobador);
    pthread_mutex_unlock(&accesoGrafo);

    pthread_mutex_lock( & recurso[j]); // Adquiero el recurso

    //Acceso al grafo para añadir el recurso obtenido
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

  //Se liberan los recursos y se actualiza el grafo
  for (i = 0; i < numRecursos; i++){
    pthread_mutex_unlock( & recurso[mis_recursos[i]]);
    pthread_mutex_lock(&accesoGrafo);
    G[i + Pmax][thid] = 0;
    pthread_mutex_unlock(&accesoGrafo);
  }

  printf("************ACABE! Soy %d\n", thid);

  pthread_exit(NULL);
}

void *hiloComprobador(void *tid){
  comprobador = sem_open("Comprobador", O_CREAT, S_IRWXU, 0);

  //Se mantiene activo hasta que acaba el hilo principal
  while(1){
    //Semáforo para controlar el trabajo (que no esté en espera activa)
    sem_wait(comprobador);
    pthread_mutex_lock(&accesoGrafo);

    //Comprueba si el grafo posee un ciclo y si lo hay lo deshace
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


//Función que deshace un ciclo del grafo liberando el recurso j y desbloqueando
//el recurso
void deshacerCiclo(int i, int j){
  printf("Libero el recurso %d \n", j + Pmax);
  for(int k=0; k < Pmax; k++)
    G[j + Pmax][k] = 0;
  pthread_mutex_unlock(&recurso[j]);
}

//Función que comprueba si hay un ciclo en el grafo
int hayCiclo(){
  int *nodosVisitados;

  //Reserva memoria para un registro de los nodos que se han visitado
  nodosVisitados = (int *)malloc((Nmax + Pmax)*sizeof(int));
  for(int i=0; i < Nmax + Pmax; i++)
    nodosVisitados[i] = -1;

  //Añade la última fila modificada como primer nodo visitado (el que se le pasa
  //al algoritmo DFS) y se llama al algoritmo DFS
  nodosVisitados[visitados] = filaModif;
  visitados++;
  DFS(filaModif, nodosVisitados);

  //Se ponen los visited a cero de nuevo
  for(int i=0; i < Nmax + Pmax; i++)
    visited[i] = 0;

  //Se busca si se visitó a dos nodos iguales
  if(busquedaIguales(nodosVisitados)){
    visitados = 0;
    free(nodosVisitados);
    return 1;
  }

  //Si no se visitaron retornamos 0
  visitados = 0;
  free(nodosVisitados);
  return 0;
}

//--------------Algoritmos de búsqueda y de exploración de grafos------------

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
