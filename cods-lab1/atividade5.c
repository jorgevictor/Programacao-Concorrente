#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

float *arrayInicial;
float *arrayFinal; 
int nthreads; 
int tamanho_vetor; 

void *somaUmAoVetor(void *arg) {
    int idThread = *(int*) arg;
    for (int i = idThread; i < tamanho_vetor; i += nthreads) {
        arrayFinal[i] = arrayInicial[i] + 1;
    }
    pthread_exit(NULL);
}

void preenchedorDeArray(float* array) {
    for (int i = 0; i < tamanho_vetor; i++) {
        array[i] = i;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <nthreads> <tamanho_vetor>\n", argv[0]);
        return 1;
    }

    nthreads = atoi(argv[1]);
    tamanho_vetor = atoi(argv[2]);

    pthread_t tid_sistema[nthreads]; // Identificadores das threads no sistema
    int tid_local[nthreads]; // Array para armazenar os identificadores de thread
    
    arrayInicial = (float *) malloc(sizeof(float) * tamanho_vetor); 
    arrayFinal = (float *) malloc(sizeof(float) * tamanho_vetor); 

    //preenche o vetor
    preenchedorDeArray(arrayInicial);

    //cria as threads
    for (int thread = 0; thread < nthreads; thread++) {
        tid_local[thread] = thread;
        if (pthread_create(&tid_sistema[thread], NULL, somaUmAoVetor, (void*) &tid_local[thread])) {
            printf("ERRO NA CRIAÇÃO DA THREAD %d \n", thread);
            return 2;
        }
    }

    //espera todas as threads terminarem
    for (int thread = 0; thread < nthreads; thread++) {
        if (pthread_join(tid_sistema[thread], NULL)) { 
            printf("--ERRO: pthread_join() \n");
            return -1; 
        } 
    }

    int checadorDeArrays = 1;
    for (int i = 0; i < tamanho_vetor; i++) {
        float valorDeChecagem = arrayInicial[i] + 1;
        if (valorDeChecagem - arrayFinal[i] > 0) {
            checadorDeArrays = 0;
        }
    }

    if (checadorDeArrays) {
        printf("Os vetores são iguais!\n");
    } else {
        printf("Os vetores não são iguais!\n");    
    }

    // Liberar a memória alocada
    free(arrayInicial);
    free(arrayFinal);

    return 0;
}
