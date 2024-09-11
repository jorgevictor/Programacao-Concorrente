#include<stdio.h>
#include<stdlib.h>
#include "timer.h"

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
   if(argc < 4) {
      fprintf(stderr, "Digite: %s <arquivo entrada> <arquivo entrada> <arquivo saida>\n", argv[0]);
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

   //multiplicacao da matriz pelo vetor
   GET_TIME(inicio);

    // Realiza a multiplicação de matrizes
    for (int i = 0; i < linhas1; i++) {
        for (int j = 0; j < colunas2; j++) {
            for (int k = 0; k < colunas1; k++) {
                matrizSaida[i * colunas2 + j] += matriz1[i * colunas1 + k] * matriz2[k * colunas2 + j];
            }
        }
    }

    GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo multiplicacao : %lf\n", delta);

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

