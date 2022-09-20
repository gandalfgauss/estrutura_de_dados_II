#pragma once
#ifndef ARVORE_B_ESTRELA_H
#define ARVORE_B_ESTRELA_H

typedef struct Registro TRegistro;
typedef struct TPaginaB TPaginaB;

int arvore_b_estrela(FILE*, int, long int[2], long int[2], double[2], TRegistro1&, long int&, long int&, double&);

int arquivo_vazio(FILE*);//funcao que verifica se um arquivo est? vazio

void cria_arvore(TPaginaB**);//funcao que cria uma arvore b estrela

int ponteiro_para_proxima_pagina(TPaginaB*, TRegistro, long int[2], long int[2], char);//funcao que recebe uma pagina e um registro e devolve o ponteiro para uma pagina em que o registro pode estar localizado

int posicao_que_deve_ser_inserido(TPaginaB*, TRegistro, long int[2], long int[2]);//funcao que recebe uma pagina e um registro e devolve a posicao em que o registro deve ser inserido

void insere_no_folha(TPaginaB**, TRegistro*, long int[2], long int[2]);//funcao que insere um registro em um no folha

void insere_na_pagina(TPaginaB*, TRegistro, int);//funcao que recebe uma pagina, um registro e uma posicao.Essa funcao insere na pagina recebida a chave do registro na posicao recebida

int insere_na_arvore(TPaginaB**, TRegistro, long int[2], long int[2]);//funcao que insere um registro na arvore, retorna 1 caso o registro seja inserido com sucesso, retorna 0 caso contr?rio

void imprime_arvore(TPaginaB*);//funcao que imprime a arvore sempre indo para as paginas mais a esquerda primeiro

int busca_na_arvore(TPaginaB* raiz, TRegistro* registro, int chave, long int[2], long int[2]);//funcao que dada uma arvore, um registro e uma chave, busca na arvore o registro com a chave especificada. Retorna 1 e o registro caso o registro seja encontrado, caso contr?rio retorna 0



#endif // !ARVORE_B_ESTRELA_H
