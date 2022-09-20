#ifndef ARVORE_BINARIA_EM_MEMORIA_EXTERNA_H
#define ARVORE_BINARIA_EM_MEMORIA_EXTERNA_H

typedef struct RegistroB TRegistroB;
typedef struct RegistroB1 TRegistroB1;

int arvore_binaria(FILE*, int, long int[2], long int[2], double[2], TRegistro1&, long int&, long int&, double&);
int abeme_arquivo_vazio(FILE*);//funcao que verifica se um arquivo est? vazio
int insere_na_arvore(FILE*, TRegistroB1, long int[2], long int[2]);//insere um registro na arvore
int cria_arvore(FILE*, FILE*, long int[2], long int[2]);//cria arvore binaria a partir de um arquivo de resgistros
int pesquisa_na_arvore_binaria(FILE*, TRegistroB*, int, long int[2], long int[2]);//pesquisa uma chave na arvore e retorna o registro se encontrado

void imprime_registros_em_arquivo_binario(FILE*);//funcao que imprime na tela os registros de um arquivo binario

#endif