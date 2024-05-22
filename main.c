#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * Implemente um programa concorrente onde UMA thread PRODUTORA carrega
de um arquivo binário uma sequencia de N (N bastante grande) de números inteiros e
os deposita em um buffer de tamanho M (M pequeno) — um de cada vez — que será
compartilhado com threads CONSUMIDORAS. AS threads CONSUMIDORAS retiram
os números — um de cada vez — e avaliam sua primalidade
 * */

int ehPrimo(long long int n) {
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i < sqrt(n) + 1; i += 2)
        if (n % i == 0) return 0;
    return 1;

}

void criarThreads(int qtdThreads) {

}

int * leArquivoEntrada(FILE * arquivo) {

}

int main(int argc, char * argv[]) {

    //recebe os argumentos de entradachar
    if(argc < 4) {
        fprintf(stderr, "Digite ./main <nomeArquivo> <M (tamanho do buffer)> <qtd threads consumidoras> \n");
        return 1;
    }
    FILE * arquivoEntrada = argv[1];
    long M = strtol(argv[2], NULL, 10);
    long qtdThreads = strtol(argv[3],NULL, 10);

    int * numeros = leArquivoEntrada(arquivoEntrada);

    return 0;
}