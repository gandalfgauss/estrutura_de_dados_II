#ifndef GERA_REGISTROS_H
#define GERA_REGISTROS_H

typedef struct GRegistro TGRegistro;

int gerar(int, int, FILE*);//funcao que cria o registros.bin e chama a funcao gera_registros para gerar os registros

int gera_registros(int, int);// gera os registros de acordo com o tipo, aleatorio = 0, crescente = 1 e decrescente = 2

int g_imprime_registros();//imprime os registros antes e depois da ordenacao

int imprimir_no_txt();//coloca os registros ordenados em um arquivo txt chamado registrosordenados.txt

#endif // !GERA_REGISTROS_H
