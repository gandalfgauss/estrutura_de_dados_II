#ifndef GERA_REGISTROS_H
#define GERA_REGISTROS_H

typedef struct Registro1 TRegistro1;

int gerar(int, TRegistro1*, int, FILE*, int);//funcao que gera registros de tipo 1

FILE* gera_registros(int, int, int, int);//tipo 1 ou tipo 2, aleatorio = 0, crescente = 1 e decrescente = 2

void g_imprime_registros(FILE*, int);

FILE* gera_meu_gosto();

#endif // !GERA_REGISTROS_H
