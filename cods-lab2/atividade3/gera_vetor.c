#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Função para gerar números float aleatórios entre 0 e 1000
float random_float() {
    return ((float)rand() / RAND_MAX) * 1000.0f;
}

// Função principal
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Use: %s <arquivo de saída> <dimensão N>\n", argv[0]);
        return 1;
    }

    // Abre o arquivo de saída
    FILE *file = fopen(argv[1], "wb");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return 1;
    }

    // Converte o argumento N para inteiro
    long int N = atol(argv[2]);
    
    // Aloca memória para os vetores
    float *vet1 = malloc(sizeof(float) * N);
    float *vet2 = malloc(sizeof(float) * N);
    if (vet1 == NULL || vet2 == NULL) {
        printf("Erro ao alocar memória.\n");
        return 1;
    }

    // Gera os vetores aleatórios
    srand(time(NULL));
    for (long int i = 0; i < N; i++) {
        vet1[i] = random_float();
        vet2[i] = random_float();
    }

    // Calcula o produto interno
    double produto_interno = 0.0;
    for (long int i = 0; i < N; i++) {
        produto_interno += vet1[i] * vet2[i];
    }

    // Escreve N, os vetores e o produto interno no arquivo binário
    fwrite(&N, sizeof(long int), 1, file);
    fwrite(vet1, sizeof(float), N, file);
    fwrite(vet2, sizeof(float), N, file);
    fwrite(&produto_interno, sizeof(double), 1, file);

    // Libera a memória e fecha o arquivo
    free(vet1);
    free(vet2);
    fclose(file);

    printf("Arquivo %s gerado com sucesso.\n", argv[1]);
    return 0;
}
