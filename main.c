#define Tmin 0      // Tiempo mínimo de espera tras adquirir recurso #define Tmax 2
#define Tmax 2      //Tiempo mínimo de espera tras adquirir recurso
#define Nmax 4      // Recursos
#define Pmax 3      // Threads pthread_mutex_t recurso[Nmax];

void *trabajo(void *tid)  {                                  // Código para cada thread
  int thid, i, j, k, ya, mis_recursos[Nmax];
  double x=0;thid= (int)tid;srand(thid+time(NULL));
   for (i=0;i<Nmax;i++) {                                    // Adquiero nuevos recursos
     ya=0;
     while (ya==0) {                                          // Selecciono un recurso que no tengo
       j= (int) (Nmax)*(rand()/(RAND_MAX+1.0));
       ya=1;
       for (k=0;k<i;k++) if (mis_recursos[k]==j) ya=0;
     }
     mis_recursos[i]=j;         // Incluyo recurso en mi lista
     printf("Soy %d y quiero el recurso %d\n",thid,j);
     pthread_mutex_lock(&recurso[j]);             // Adquiero el recurso
     printf("           Soy %d y tengo el recurso %d\n",thid,j);
     for (k=0;k<10000;k++) x+=sqrt(sqrt(k+0.1));      // Trabajo intrascendente
     k= (int) Tmin+(Tmax-Tmin+1)*(rand()/(RAND_MAX+1.0)) + 1;
     sleep(k);                                                     // Espero un tiempo aleatorio
   }
   for (i=0;i<Nmax;i++)  pthread_mutex_unlock(&recurso[mis_recursos[i]]);
     printf("************ACABE! Soy %d\n",thid);
   }


   
