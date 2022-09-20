#ifndef ARVOREB_H
#define ARVOREB_H

typedef struct Registro TRegistro;
typedef struct TPagina TPagina;

int arvoreb(FILE*, int, long int[2], long int[2], double[2], TRegistro1&, long int&, long int&, double&);

int b_arquivo_vazio(FILE*);//funcao que verifica se um arquivo est? vazio

int b_ponteiro_para_proxima_pagina(TPagina*, TRegistro, long int[2], long int[2], char);//funcao que recebe uma pagina e um registro e devolve o ponteiro para uma pagina em que o registro pode estar localizado

int b_posicao_que_deve_ser_inserido(TPagina*, TRegistro, long int[2], long int[2]);//funcao que recebe uma pagina e um registro e devolve a posicao em que o registro deve ser inserido

void b_insere_na_pagina(TPagina*, TRegistro, int);//funcao que recebe uma pagina, um registro e uma posicao.Essa funcao insere na pagina recebida um registro na posicao recebida

int b_insere_na_arvore(TPagina**, TRegistro, long int[2], long int[2]);//funcao que insere um registro na arvore, retorna 1 caso o registro seja inserido com sucesso, retorna 0 caso contr?rio

void b_imprime_arvore(TPagina*);//funcao que imprime a arvore sempre indo para as paginas mais a esquerda primeiro

int b_busca_na_arvore(TPagina*, TRegistro*, int, long int[2], long int[2]);//funcao que dado uma arvore, um registro e uma chave, busca na arvore o registro com a chave especificada. Retorna 1 e o registro caso o registro seja encontrado, caso contr?rio retorna 0

#endif // !ARVOREB_H

