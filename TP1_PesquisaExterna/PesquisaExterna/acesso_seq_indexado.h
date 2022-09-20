#ifndef ACESSO_SEQ_INDEXADO_H
#define ACESSO_SEQ_INDEXADO_H

#define MAX_TABELA 251000

typedef struct Registro TRegistro;

int acesso_sequencial_indexado(FILE*, int, long int[2], long int[2], double[2], TRegistro1&, long int&, long int&, double&);
int asi_arquivo_vazio(FILE*);//funcao que verifica se um arquivo está vazio
int cria_tabela_de_indices(FILE*, int[MAX_TABELA], int*, int*, long int[2], long int[2]);//funcao que cria a tabela de indices
int pesquisa_binaria(TRegistro*, int, TRegistro*, int, long int[2], long int[2]);//funcao que dado um vetor de registro busca um registro pela chave utilizando pesquisa binária
int pesquisa_chave(FILE*, int[MAX_TABELA], int, int, TRegistro*, int, long int[2], long int[2]);//funcao que pesquisa um registro em um arquivo dado uma tabela de paginas

#endif 

