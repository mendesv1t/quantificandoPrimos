##Quantificando primos com threads e semáforos

### Contexto:
Este programa concorrente consiste na contagem de primos presente em um arquivo binário fornecido como entrada.

Ele aborda o padrão produtor/consumidor, onde há uma única thread produtora, que irá preenchendo o buffer 
conforme ele for ficando vazio, e threads consumidoras, sendo a quantidade definida pelo usuário, que possuem
como tarefa verificar se um número e primo ou não.

Ao fim da execução, o programa diz qual foi a thread que contou mais primos, sendo ela a vencedora.

### Execução:

- Para gerar valores aleatórios numéricos inteiros em um arquivo binário, compile o arquivo ```geraBinario.c```
com o comando ```gcc -o gera geraBinario.c -lm -lpthread -Wall```.
- Execute ```./gera <qtdNumeros> <nomeArquivoSaida>``` para gerar o arquivo.
- Para compilar o programa concorrente principal rode ```gcc -o main main.c -lm -lpthread -Wall```
- Em seguida, execute ```./main <nomeArquivo> <qtd itens no buffer> <qtd de threads consumidoras>```, sendo o
arquivo gerado anteriormente o primeiro argumento, e os demais os
valores para o tamanho do buffer e a quantidade de threads consumidoras.
