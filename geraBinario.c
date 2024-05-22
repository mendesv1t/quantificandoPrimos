#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int ehPrimo(long long int n) {
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i < sqrt(n) + 1; i += 2)
        if (n % i == 0) return 0;
    return 1;

}


void geraValoresArquivoBinario(long N, char nome[100]) {

    double min = -1000000;
    double max = 1000000;
    time_t t;

    // Initializing file pointer.
    FILE *p_file;
    unsigned i;

    if(nome == NULL) {
        printf("Erro: Atribua um nome para o arquivo\n");
    }

    p_file = fopen(nome, "wb");
    if (p_file == NULL) {
        printf("Erro ao criar o arquivo!");
    }

    srand((unsigned) time(&t));

    int qtdPrimos = 0;

    for (i = 1; i <= N; i++) {
        int random_num = rand();
        fwrite(&random_num,sizeof (int),1, p_file);
        qtdPrimos += ehPrimo(random_num);
    }

    // adicionando o tamanho na penultima posição:
    fprintf(p_file, "%ld\n", N);

    // adicionando a quantidade de primos esperada no fim do arquivo:
    fprintf(p_file, "%d", qtdPrimos);

    // fecha arquivo
    fclose(p_file);

}

int main (int argc, char * argv[]) {

    long N;
    char * nomeArquivo;


    //recebe os argumentos de entrada
    if(argc < 3) {
        fprintf(stderr, "Digite ./geraBinario <qtd de inteiros> <nome arquivo saída>: \n");
        return 1;
    }

    N = strtol(argv[1], NULL, 10);
    nomeArquivo = argv[2];

    geraValoresArquivoBinario(N, nomeArquivo);
    printf("Arquivo gerado!\nRode o comando 'gcc - o main main.c -lm' em seguida, rode './main <nomeArquivo> <M (tamanho do buffer)>' para executar\n");

    return 0;
}