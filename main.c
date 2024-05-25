#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <math.h>


sem_t slotCheio, slotVazio;
sem_t mutexGeral;

int * Buffer;
int * vetor;

typedef struct {
    int M;
    char * nomeArquivo;
} pArgs;

typedef struct {
    int id;
    int M;
} cArgs;


int ehPrimo(long long int n) {
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i < sqrt(n) + 1; i += 2)
        if (n % i == 0) return 0;
    return 1;

}

/* método para inicializar vetor que em cada posição,
 * cujo o id da thread se confere, haverá a contagem de primos
 * de cada thread.
 */
void inicializaVetorContagem(int * vetor, int qtdThreads) {
    for (int i = 0; i < qtdThreads+1; i++) {
        vetor[i] = 0;
    }
}

//funcao para inserir um elemento no buffer
void popula (int item, int M) {
    static int in=0;
    sem_wait(&slotVazio); //aguarda slot vazio para inserir
    sem_wait(&mutexGeral); //exclusao mutua entre produtores (aqui geral para log)

    Buffer[in] = item;
    in = (in + 1) % M;

    sem_post(&mutexGeral);
    sem_post(&slotCheio); //sinaliza um slot cheio
}

//funcao para retirar  um elemento no buffer
int consome (int M) {

    int item;
    static int out = 0;

    sem_wait(&slotCheio); //aguarda slot cheio para retirar
    sem_wait(&mutexGeral); //exclusao mutua entre consumidores (aqui geral para log)

    item = Buffer[out];
    Buffer[out] = 0;
    out = (out + 1) % M;

    sem_post(&mutexGeral);
    sem_post(&slotVazio); //sinaliza um slot vazio
    return item;
}


void *produtor(void * arg) {

    pArgs * args =  (pArgs *) arg;
    int M = args->M;

    FILE * arquivo = fopen(args->nomeArquivo, "rb");


    if (!arquivo) {
        fprintf(stderr, "Erro na leitura do arquivo.\n");
        exit(-1);
    }

    int item;
    while(fread(&item, sizeof(int), 1, arquivo)) {
        popula(item, M);
    }

    vetor[0] = item;

    free(arg);
    pthread_exit(NULL);
}


void *consumidor(void * arg) {

    cArgs * args = ( cArgs *) arg;

    while(1) {

        int item = consome(args->M);

        if (item == 0) {
            break;
        }

        if (ehPrimo(item)) {
            vetor[args->id]++;
        }

    }
    free(arg);
    pthread_exit(NULL);
}

void criaThreadProdutora(char * nomeArquivo,int M, pthread_t * tid) {

    pArgs * args = malloc(sizeof(pArgs));
    args->M = M;
    args->nomeArquivo = nomeArquivo;


    if (pthread_create(&tid[0], NULL, produtor, args)) {
        printf("Erro na criacao da thread produtora\n");
        exit(-1);
    }
}


void criaThreadsConsumidoras(int qtdThreads,int M, pthread_t * tid) {

    for(int i=0;i<qtdThreads;i++) {

        cArgs * args = malloc(sizeof(cArgs));
        args->id = i+1;
        args->M = M;

        if (pthread_create(&tid[i+1], NULL, consumidor, args)) {
            printf("Erro na criacao do thread consumidor\n");
            exit(-1);
        }
    }
}


int main(int argc, char * argv[]) {

    //recebe os argumentos de entrada
    if(argc < 4) {
        fprintf(stderr, "Digite ./main <nomeArquivo> <M (tamanho do buffer)> <qtd threads consumidoras> \n");
        return 1;
    }
    printf("cu");

    char * nomeArquivo = argv[1];
    int M = atoi(argv[2]);
    int qtdThreads = atoi(argv[3]);

    sem_init(&slotVazio,0,M);
    sem_init(&slotCheio, 0,0);
    sem_init(&mutexGeral,0,1);


    vetor = malloc(sizeof(int)*(qtdThreads+1));
    Buffer = malloc(sizeof(int)*(M));
    inicializaVetorContagem(vetor, qtdThreads);

    pthread_t tid[qtdThreads+1];


    criaThreadProdutora(nomeArquivo,M,tid);
    criaThreadsConsumidoras(qtdThreads,M,tid);

    for (int i = 0; i<qtdThreads+1; i++) {
        if (pthread_join(tid[i], NULL)) {
            fprintf(stderr, "Erro pthread_join()");
            exit(-1);
        }
    }

    int totalPrimos = 0;
    int vencedora;
    for (int i = 1; i <= qtdThreads; i++) {

        int qtdTemp = 0;
        vencedora = -1;
        totalPrimos += vetor[i];

        if (vetor[i] > qtdTemp) {
            vencedora = i;
        }
    }

    printf("A thread vencedora foi: %d\n", vencedora);
    printf("Foram computados %d primos.\n", totalPrimos);
    printf("O arquivo possuía %d primos.\n", vetor[0]);

    sem_destroy(&slotCheio);
    sem_destroy(&slotVazio);
    sem_destroy(&mutexGeral);
    free(Buffer);
    free(vetor);
    return 0;
}