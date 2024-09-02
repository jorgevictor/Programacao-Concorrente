#include <stdio.h>
#include <stdlib.h>

// Função para ler e verificar o arquivo binário
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Use: %s <arquivo de entrada>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo para leitura.\n");
        return 1;
    }

    long int N;
    fread(&N, sizeof(long int), 1, file);

    float *vet1 = malloc(sizeof(float) * N);
    float *vet2 = malloc(sizeof(float) * N);
    if (vet1 == NULL || vet2 == NULL) {
        printf("Erro ao alocar memória.\n");
        return 1;
    }

    fread(vet1, sizeof(float), N, file);
    fread(vet2, sizeof(float), N, file);

    double produto_interno;
    fread(&produto_interno, sizeof(double), 1, file);

    fclose(file);

    // Exibe os resultados para verificação
    printf("Dimensão N: %ld\n", N);
    printf("Produto interno no arquivo: %.6f\n", produto_interno);

    // Calcula o produto interno manualmente para verificação
    double produto_verificado = 0.0;
    for (long int i = 0; i < N; i++) {
        produto_verificado += vet1[i] * vet2[i];
    }

    printf("Produto interno calculado: %.6f\n", produto_verificado);

    if (produto_verificado == produto_interno) {
        printf("Verificação correta!\n");
    } else {
        printf("Erro na verificação!\n");
    }

    free(vet1);
    free(vet2);

    return 0;
}
