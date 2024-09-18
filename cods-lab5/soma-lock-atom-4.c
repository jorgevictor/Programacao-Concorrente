#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

long int soma = 0; // Variável compartilhada entre as threads
pthread_mutex_t mutex; // Variável de lock para exclusão mútua
pthread_cond_t cond; // Variável de condição para sincronização
int imprimindo = 0; // Variável de controle para sincronização
int multiplos_impressos = 0; // Contador de múltiplos de 10 impressos

// Função executada pelas threads
void *ExecutaTarefa(void *arg) {
  long int id = (long int)arg;
  printf("Thread : %ld esta executando...\n", id);

  for (int i = 0; i < 10000; i++) {
    pthread_mutex_lock(&mutex);

    while (imprimindo) { // Espera se a thread 'extra' está imprimindo
        pthread_cond_wait(&cond, &mutex);
    }

    soma++; // Incrementa a variável compartilhada 

    if (soma % 10 == 0 && multiplos_impressos < 20) { // Se 'soma' for múltiplo de 10 e menos de 20 foram impressos
        imprimindo = 1; // Indica que a thread extra deve imprimir
        pthread_cond_signal(&cond); // Sinaliza a thread extra
    }

    pthread_mutex_unlock(&mutex);
  }

  printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

// Função executada pela thread de log
void *extra(void *args) {
  printf("Extra : esta executando...\n");

  while (multiplos_impressos < 20) { // Continua até imprimir 20 múltiplos de 10
    pthread_mutex_lock(&mutex);

    while (!imprimindo) { // Espera até que haja algo para imprimir
        pthread_cond_wait(&cond, &mutex);
    }

    // Imprime o valor se 'soma' for múltiplo de 10
    if (soma % 10 == 0) {
        printf("soma = %ld \n", soma);
        multiplos_impressos++; // Incrementa o contador de múltiplos impressos
    }

    imprimindo = 0; // Indica que a impressão foi feita
    pthread_cond_broadcast(&cond); // Sinaliza todas as threads para continuar

    pthread_mutex_unlock(&mutex);
  }

  printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

// Fluxo principal
int main(int argc, char *argv[]) {
   pthread_t *tid; // Identificadores das threads no sistema
   int nthreads; // Quantidade de threads (passada pela linha de comando)

   // Lê e avalia os parâmetros de entrada
   if(argc < 2) {
      printf("Digite: %s <numero de threads>\n", argv[0]);
      return 1;
   }
   nthreads = atoi(argv[1]);

   // Aloca as estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*(nthreads + 1));
   if(tid == NULL) {puts("ERRO--malloc"); return 2;}

   // Inicializa o mutex e a variável de condição
   pthread_mutex_init(&mutex, NULL);
   pthread_cond_init(&cond, NULL);

   // Cria as threads
   for(long int t = 0; t < nthreads; t++) {
     if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
     }
   }

   // Cria a thread de log
   if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
   }

   // Espera todas as threads terminarem
   for (int t = 0; t < nthreads + 1; t++) {
     if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
     } 
   } 

   // Finaliza o mutex e a variável de condição
   pthread_mutex_destroy(&mutex);
   pthread_cond_destroy(&cond);
   
   printf("Valor de 'soma' = %ld\n", soma);

   return 0;
}
