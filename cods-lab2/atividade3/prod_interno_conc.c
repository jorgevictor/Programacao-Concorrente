#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Variáveis globais
long int N;              // Tamanho dos vetores
float *vet1, *vet2;      // Vetores de entrada
double produto_verificado = 0.0; // Produto interno calculado pelas threads
int nthreads;            // Número de threads

// Função executada pelas threads
void *calculaProdInterno(void *arg) {
    int idThread = *(int*) arg;
    double soma_local = 0.0;

    // Cada thread calcula uma parte do produto interno
    for (long int i = idThread; i < N; i += nthreads) {
        soma_local += vet1[i] * vet2[i];
    }
    produto_verificado += soma_local;


    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Use: %s <arquivo de entrada> <número de threads>\n", argv[0]);
        return 1;
    }

    // Abre o arquivo de entrada
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo para leitura.\n");
        return 1;
    }

    // Lê a dimensão N
    fread(&N, sizeof(long int), 1, file);

    // Aloca memória para os vetores
    vet1 = malloc(sizeof(float) * N);
    vet2 = malloc(sizeof(float) * N);
    if (vet1 == NULL || vet2 == NULL) {
        printf("Erro ao alocar memória.\n");
        fclose(file);
        return 1;
    }

    // Lê os vetores do arquivo
    fread(vet1, sizeof(float), N, file);
    fread(vet2, sizeof(float), N, file);

    // Lê o produto interno do arquivo
    double produto_interno;
    fread(&produto_interno, sizeof(double), 1, file);
    fclose(file);

    // Lê o número de threads da entrada
    nthreads = atoi(argv[2]);
    pthread_t threads[nthreads];
    int thread_ids[nthreads];

    // Cria as threads
    for (int i = 0; i < nthreads; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, calculaProdInterno, (void*)&thread_ids[i])) {
            printf("Erro ao criar a thread %d\n", i);
            return 1;
        }
    }

    // Espera todas as threads terminarem
    for (int i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Exibe os resultados
    printf("Produto interno no arquivo: %.6f\n", produto_interno);
    printf("Produto interno calculado: %.6f\n", produto_verificado);

    if (produto_verificado == produto_interno) {
        printf("Verificação correta!\n");
    } else {
        printf("Erro na verificação!\n");
    }

    // Libera a memória e destrói o mutex
    free(vet1);
    free(vet2);

    return 0;
}
