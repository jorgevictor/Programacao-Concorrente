#include <stdio.h>
#include <stdlib.h>
#include "list_int.h"
#include <pthread.h>
#include "timer.h"

#define QTDE_OPS 1000000
#define QTDE_INI 100
#define MAX_VALUE 100

struct list_node_s* head_p = NULL;
int nthreads;

pthread_mutex_t mutex ;
pthread_cond_t readers_cond ;
pthread_cond_t writers_cond ;

// Controle de prioridade
int writing = 0;        // Escrita em andamento
int write_requests = 0; // Pedidos de escrita pendentes
int readers = 0;        // Leitores ativos

// Tarefa das threads
void* tarefa(void* arg) {
   long int id = (long int) arg;
   int op, in = 0, out = 0, read = 0;

   for (long int i = id; i < QTDE_OPS; i += nthreads) {
      op = rand() % 100;

      if (op < 98) {  // Operação de leitura
         pthread_mutex_lock(&mutex);
         while (writing || write_requests > 0) {
            pthread_cond_wait(&readers_cond, &mutex);
         }
         readers++;
         pthread_mutex_unlock(&mutex);

         // Realiza a leitura
        //  printf("Thread %ld iniciando leitura\n", id);
         Member(i % MAX_VALUE, head_p);
        //  printf("Thread %ld finalizando leitura\n", id);

         pthread_mutex_lock(&mutex);
         readers--;
         if (readers == 0) pthread_cond_signal(&writers_cond);
         pthread_mutex_unlock(&mutex);
         read++;
      } 
      else if (op < 99) {  // Operação de inserção
         pthread_mutex_lock(&mutex);
         write_requests++;
         while (writing || readers > 0) {
            pthread_cond_wait(&writers_cond, &mutex);
         }
         write_requests--;
         writing = 1;
         pthread_mutex_unlock(&mutex);

         // Realiza a escrita
        //  printf("Thread %ld iniciando escrita (inserção)\n", id);
         Insert(i % MAX_VALUE, &head_p);
        //  printf("Thread %ld finalizando escrita (inserção)\n", id);

         pthread_mutex_lock(&mutex);
         writing = 0;
         pthread_cond_broadcast(&writers_cond);
         pthread_cond_broadcast(&readers_cond);
         pthread_mutex_unlock(&mutex);
         in++;
      } 
      else {  // Operação de remoção
         pthread_mutex_lock(&mutex);
         write_requests++;
         while (writing || readers > 0) {
            pthread_cond_wait(&writers_cond, &mutex);
         }
         write_requests--;
         writing = 1;
         pthread_mutex_unlock(&mutex);

         // Realiza a escrita
        //  printf("Thread %ld iniciando escrita (remoção)\n", id);
         Delete(i % MAX_VALUE, &head_p);
        //  printf("Thread %ld finalizando escrita (remoção)\n", id);

         pthread_mutex_lock(&mutex);
         writing = 0;
         pthread_cond_broadcast(&writers_cond);
         pthread_cond_broadcast(&readers_cond);
         pthread_mutex_unlock(&mutex);
         out++;
      }
   }

   printf("Thread %ld: in=%d out=%d read=%d\n", id, in, out, read);
   pthread_exit(NULL);
}

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   pthread_t* tid;
   double ini, fim, delta;

   if (argc < 2) {
      printf("Digite: %s <numero de threads>\n", argv[0]);
      return 1;
   }
   nthreads = atoi(argv[1]);

   for (int i = 0; i < QTDE_INI; i++) {
      Insert(i % MAX_VALUE, &head_p);
   }

   tid = malloc(sizeof(pthread_t) * nthreads);
   if (tid == NULL) {
      printf("--ERRO: malloc()\n");
      return 2;
   }

   GET_TIME(ini);

   for (long int i = 0; i < nthreads; i++) {
      if (pthread_create(tid + i, NULL, tarefa, (void*)i)) {
         printf("--ERRO: pthread_create()\n");
         return 3;
      }
   }

   for (int i = 0; i < nthreads; i++) {
      if (pthread_join(*(tid + i), NULL)) {
         printf("--ERRO: pthread_join()\n");
         return 4;
      }
   }

   GET_TIME(fim);
   delta = fim - ini;
   printf("Tempo: %lf\n", delta);

   free(tid);
   Free_list(&head_p);

   return 0;
}
