#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include "timer.h"

typedef struct {
    int id;                  // ID da thread
    int num_threads;         // Número de threads
    int linhas1, colunas1;   // Dimensões da matriz1
    int linhas2, colunas2;   // Dimensões da matriz2
    float *matriz1;          // Ponteiro para matriz1
    float *matriz2;          // Ponteiro para matriz2
    float *matrizSaida;      // Ponteiro para matriz de saída
} ThreadData;


void* multiplicacao_matriz(void* arg) {
    ThreadData *data = (ThreadData*) arg;
    int id = data->id;
    int num_threads = data->num_threads;
    int linhas1 = data->linhas1;
    int colunas1 = data->colunas1;
    int colunas2 = data->colunas2;
    float *matriz1 = data->matriz1;
    float *matriz2 = data->matriz2;
    float *matrizSaida = data->matrizSaida;

    // Calcula o tamanho do bloco de linhas que cada thread deve processar
    int linhas_por_thread = linhas1 / num_threads;
    int inicio = id * linhas_por_thread; // Linha inicial para esta thread
    int fim = (id == num_threads - 1) ? linhas1 : inicio + linhas_por_thread; // Linha final para esta thread

    // Cada thread calcula um bloco contínuo de linhas da matriz de saída
    for (int i = inicio; i < fim; i++) {
        for (int j = 0; j < colunas2; j++) {
            for (int k = 0; k < colunas1; k++) {
                matrizSaida[i * colunas2 + j] += matriz1[i * colunas1 + k] * matriz2[k * colunas2 + j];
            }
        }
    }

    pthread_exit(NULL);
}


int main(int argc, char*argv[]) {
   float *matriz1; //matriz que será carregada do arquivo
   float *matriz2;
   float *matrizSaida;
   int linhas1, colunas1; //dimensoes da matriz
   long long int tam1; //qtde de elementos na matriz
   int linhas2, colunas2; //dimensoes da matriz
   long long int tam2; //qtde de elementos na matriz
   long long int tam3; //qtde de elementos na matriz
   FILE * descritorArquivo1; //descritor do arquivo de entrada
   FILE * descritorArquivo2; //descritor do arquivo de entrada
   FILE * descritorArquivoSaida; //descritor do arquivo de entrada
   size_t ret; //retorno da funcao de leitura no arquivo de entrada
   double inicio, fim, delta;
   
   GET_TIME(inicio);
   
   //recebe os argumentos de entrada
   if(argc < 5) {
      fprintf(stderr, "Digite: %s <arquivo entrada> <arquivo entrada> <arquivo saida> <número de threads>\n", argv[0]);
      return 1;
   }
   
   //abre o primeiro arquivo para leitura binaria
   descritorArquivo1 = fopen(argv[1], "rb");
   if(!descritorArquivo1) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 2;
   }

   //abre o segundo arquivo para leitura binaria
   descritorArquivo2 = fopen(argv[2], "rb");
   if(!descritorArquivo2) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 2;
   }

   //abre o arquivo para escrita binaria
   descritorArquivoSaida = fopen(argv[3], "wb");
   if(!descritorArquivoSaida) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 3;
   }

   //le as dimensoes da primeira matriz
   ret = fread(&linhas1, sizeof(int), 1, descritorArquivo1);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }
   ret = fread(&colunas1, sizeof(int), 1, descritorArquivo1);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }
   tam1 = linhas1 * colunas1; //calcula a qtde de elementos da primeira matriz

   //le as dimensoes da segunda matriz
   ret = fread(&linhas2, sizeof(int), 1, descritorArquivo2);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }
   ret = fread(&colunas2, sizeof(int), 1, descritorArquivo2);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }
   tam2 = linhas2 * colunas2; //calcula a qtde de elementos da segunda matriz

   tam3 = linhas1 * colunas2; // Corrige a quantidade de elementos da matriz de saída

   int num_threads = atoi(argv[4]); // Quantidade de threads recebida na linha de comando
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

   printf("Dimensões da matriz 1: %d x %d\n", linhas1, colunas1);
    printf("Dimensões da matriz 2: %d x %d\n", linhas2, colunas2);


   //aloca memoria para a primeira matriz
   matriz1 = (float*) malloc(sizeof(float) * tam1);
   if(!matriz1) {
      fprintf(stderr, "Erro de alocao da memoria da matriz\n");
      return 3;
   }

   //aloca memoria para a segunda matriz
   matriz2 = (float*) malloc(sizeof(float) * tam2);
   if(!matriz2) {
      fprintf(stderr, "Erro de alocao da memoria da matriz\n");
      return 3;
   }

   //aloca memoria para a matriz de saida
   matrizSaida = (float*) malloc(sizeof(float) * tam3);
   if(!matrizSaida) {
      fprintf(stderr, "Erro de alocao da memoria da matriz\n");
      return 3;
   }

   //carrega a primeira matriz de elementos do tipo float do arquivo
   ret = fread(matriz1, sizeof(float), tam1, descritorArquivo1);
   if(ret < tam1) {
      fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
      return 4;
   }

   //carrega a segunda matriz de elementos do tipo float do arquivo
   ret = fread(matriz2, sizeof(float), tam2, descritorArquivo2);
   if(ret < tam2) {
      fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
      return 4;
   }

    // Inicializa a matriz resultante com zero
    for (int i = 0; i < linhas1; i++) {
        for (int j = 0; j < colunas2; j++) {
            matrizSaida[i * colunas2 + j] = 0.0;
        }
    }

    GET_TIME(fim);
   delta = fim - inicio;
   printf("Tempo inicializacao:%lf\n", delta);

    GET_TIME(inicio);

    // Cria as threads
    for (int t = 0; t < num_threads; t++) {
        thread_data[t].id = t;
        thread_data[t].num_threads = num_threads;
        thread_data[t].linhas1 = linhas1;
        thread_data[t].colunas1 = colunas1;
        thread_data[t].linhas2 = linhas2;
        thread_data[t].colunas2 = colunas2;
        thread_data[t].matriz1 = matriz1;
        thread_data[t].matriz2 = matriz2;
        thread_data[t].matrizSaida = matrizSaida;

        pthread_create(&threads[t], NULL, multiplicacao_matriz, (void*) &thread_data[t]);
    }

    // Aguarda todas as threads terminarem
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo multiplicacao concorrente: %lf\n", delta);

   //liberacao da memoria
   GET_TIME(inicio);


   //escreve numero de linhas e de colunas
   ret = fwrite(&linhas1, sizeof(int), 1, descritorArquivoSaida);
   ret = fwrite(&colunas1, sizeof(int), 1, descritorArquivoSaida);
    //escreve os elementos da matriz
   ret = fwrite(matrizSaida, sizeof(float), tam3, descritorArquivoSaida);
   if(ret < tam3) {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      return 4;
   }

   //finaliza o uso das variaveis
   fclose(descritorArquivo1);
   fclose(descritorArquivo2);
   fclose(descritorArquivoSaida);
   free(matriz1);
   free(matriz2);
   free(matrizSaida);

   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo finalizacao:%lf\n", delta);
   
   return 0;
}

