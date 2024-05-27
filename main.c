#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

sem_t slotCheio, slotVazio, mutexGeral; 

int * buffer;

// esse vetor armazenará a quantidade de primos contados pelas threads consumidoras:
int * vetor;

typedef struct {
  int M;  //tamanho do buffer
  int qtdThreads;  //quantidade de threads
  char * nomeArquivo; //nome do arquivo de números a ser lido.
} pArgs;

typedef struct {
  int id; //id da thread
  int M; //tamanho do buffer
} cArgs;

int ehPrimo(int n) {
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
 * Para a thread produtora, atribuí o valor da quantidade primos total
 * do arquivo. (vetor[0] = totalPrimos), a fim de verificar a corretude
 * no fim do programa.
 */
void inicializaVetorContagem(int qtdThreads) {
    for (int i = 0; i < qtdThreads+1; i++) {
        vetor[i] = 0;
    }
}

//funcao para inserir um elemento no buffer
void popula (int item, int M) {
   static int in=0;
   sem_wait(&slotVazio); //aguarda slot vazio para inserir
   sem_wait(&mutexGeral); //exclusao mutua entre produtores (aqui geral para log)
   
   buffer[in] = item;
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

   item = buffer[out];
   buffer[out] = 0;
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
  
  for (int i = 0; i < args->qtdThreads; i++) {
    sem_post(&slotCheio);
  }

  // atribuindo o ultimo valor do arquivo (total de primos) para a thread 0 (produtora):
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

void criaThreadProdutora(char * nomeArquivo,int M, pthread_t * tid, int qtdThreads) {

    pArgs * args = malloc(sizeof(pArgs));

    if (!args) {
        fprintf(stderr, "Erro ao alocar memória para o argumento da thread produtora\n");
        exit(-1);
    }

    args->M = M;
    args->nomeArquivo = nomeArquivo;
    args->qtdThreads = qtdThreads;

    if (pthread_create(&tid[0], NULL, produtor, args)) {
      printf("Erro na criacao da thread produtora\n");
      exit(-1);
    }
  }


void criaThreadsConsumidoras(int qtdThreads,int M, pthread_t * tid) {

    for(int i = 0; i < qtdThreads; i++) {

        cArgs * args = malloc(sizeof(cArgs));

        if (!args) {
            fprintf(stderr, "Erro ao alocar memória para os argumentos das threads consumidoras\n");
            exit(-1);
        }

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
    
    char * nomeArquivo = argv[1];
    int M = atoi(argv[2]);
    int qtdThreads = atoi(argv[3]);

    sem_init(&slotVazio, 0, M);
    sem_init(&slotCheio, 0, 0);
    sem_init(&mutexGeral, 0, 1);
    
    vetor = malloc(sizeof(int)*(qtdThreads+1));

    if (!vetor) {
        fprintf(stderr, "Erro ao alocar memória para vetor de resultados das threads\n");
        exit(-1);
    }

    buffer = malloc(sizeof(int)*(M));

    if (!buffer) {
        fprintf(stderr, "Erro ao alocar memória para o buffer de dados.\n");
        exit(-1);
    }


    inicializaVetorContagem(qtdThreads);

    pthread_t tid[qtdThreads+1];

    criaThreadProdutora(nomeArquivo,M,tid,qtdThreads);
    criaThreadsConsumidoras(qtdThreads,M,tid);

    for (int i = 0; i < qtdThreads+1; i++) {
    	if (pthread_join(tid[i], NULL)) {
    		fprintf(stderr, "Erro pthread_join()");
    		exit(-1);
    	}
    }

    int totalPrimos = 0;
    int vencedora;
    int qtdTemp = 0;

    for (int i = 1; i <= qtdThreads; i++) {

    	totalPrimos += vetor[i];

    	
    	if (vetor[i] > qtdTemp) {
    	    vencedora = i;
            qtdTemp = vetor[i];
    	}

    }

    /*
 * caso em que o buffer possui o valor
 * da ultima linha do arquivo (que é a quantidade de primos
 * e ele é primo:
 */
    if (ehPrimo(vetor[0])) {
        totalPrimos -= 1;
    }

    //Verificação de teste de corretude antes de imprimir o resultado:

    if (totalPrimos != vetor[0]) {
        fprintf(stderr, "Erro: o número de primos contado pelas threads é diferente da contagem original do arquivo.\n");
        exit(-1);
    }
    else {
        printf("A thread vencedora foi: %d\n", vencedora);
        printf("Foram computados %d primos.\n", totalPrimos);
        printf("O arquivo possuía %d primos.\n", vetor[0]);
    }

    sem_destroy(&slotCheio);
    sem_destroy(&slotVazio);
    sem_destroy(&mutexGeral);

    free(buffer);
    free(vetor);
    return 0;
}
