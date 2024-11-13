#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>

// Buffers compartilhados
char buffer1[10];
char buffer2[10];  // Considerando espaço extra para quebras de linha

// Semáforos para sincronização
sem_t sem_fill_buffer1, sem_fill_buffer2;
sem_t sem_empty_buffer1, sem_empty_buffer2;

// Thread 1: Leitura do arquivo para buffer1
void* thread1(void* arg) {
    FILE* file = fopen("entrada.txt", "r");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    while (fgets(buffer1, 10, file)) {
        sem_wait(&sem_empty_buffer1);  // Aguarda espaço em buffer1
        sem_post(&sem_fill_buffer1);   // Sinaliza que buffer1 está cheio
    }

    fclose(file);
    pthread_exit(NULL);
}

// Thread 2: Processamento de buffer1 para buffer2 com inserção de quebras de linha
void* thread2(void* arg) {
    int n = 0, count = 0, tam_linha = 1, limite = 0;
    while (1) {
        sem_wait(&sem_fill_buffer1);   // Espera dados em buffer1
        sem_wait(&sem_empty_buffer2);  // Aguarda espaço em buffer2

        int len = strlen(buffer1);
        int j = 0;

        for (int i = 0; i < len; i++) {
            buffer2[j++] = buffer1[i];
            count++;

            // Insere '\n' de acordo com as regras
            if ((count == tam_linha) || (limite && count == 10)) {
                buffer2[j++] = '\n';
                count = 0;
                if (!limite) {
                    n++;
                    tam_linha = 2 * n + 1;
                    if (n > 10) {       // Alcança o limite
                        limite = 1;
                        tam_linha = 10;
                    }
                }
            }
        }

        buffer2[j] = '\0';  // Finaliza o conteúdo de buffer2
        sem_post(&sem_fill_buffer2);   // Sinaliza que buffer2 está cheio
        sem_post(&sem_empty_buffer1);  // Libera buffer1
    }
    pthread_exit(NULL);
}

// Thread 3: Impressão do conteúdo de buffer2
void* thread3(void* arg) {
    while (1) {
        sem_wait(&sem_fill_buffer2);  // Espera dados em buffer2
        printf("%s", buffer2);        // Imprime buffer2
        sem_post(&sem_empty_buffer2); // Libera buffer2
    }
    pthread_exit(NULL);
}

int main() {
    // Inicializa os semáforos
    sem_init(&sem_fill_buffer1, 0, 0);
    sem_init(&sem_fill_buffer2, 0, 0);
    sem_init(&sem_empty_buffer1, 0, 1);
    sem_init(&sem_empty_buffer2, 0, 1);

    pthread_t t1, t2, t3;
    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);
    pthread_create(&t3, NULL, thread3, NULL);

    // Aguarda o término das threads (thread1 finaliza automaticamente ao chegar ao fim do arquivo)
    pthread_join(t1, NULL);

    // Finaliza os semáforos e termina o programa
    sem_destroy(&sem_fill_buffer1);
    sem_destroy(&sem_fill_buffer2);
    sem_destroy(&sem_empty_buffer1);
    sem_destroy(&sem_empty_buffer2);

    return 0;
}
