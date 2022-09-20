#ifndef INTERCALACAO_2FITAS_HEAP_H
#define INTERCALACAO_2FITAS_HEAP_H

#define TAMANHO_DA_MEMORIA 20
#define NUMERO_DE_FITAS 40// 20 fitas de entrada e 20 de saidas

typedef struct HRegistro THRegistro;//tipo de registro do arquivo binario
typedef struct HQFita THQFita;////tipo de registro que armazena um registro e de qual fita ele veio
typedef struct HMarcado THMarcado;////tipo de registro que armazena um registro e de qual fita ele veio

int intercacao_heap_inicia(long int numero_de_transferencias[2], long int& numero_de_comparacoes, double& tempo_de_execucao); //funcao que inicializa as fitas e chama as funcoes de gerar os registros inicialmentes ordenado, e a funcao de intercalacao
int inicializa_fitas_heap(FILE** fitas);//funcao que inicializa as fitas
int gerar_blocos_ordenados_ordena_heap(FILE* arquivo, FILE** fitas, int quantidade_de_blocos[NUMERO_DE_FITAS], int& numero_de_fitas_utilizadas, long int numero_de_transferencias[2], long int& numero_de_comparacoes);//funcao que gera os blocos ordenados inicialmente nas fitas de entrada utilizando o heap
int intercalar_ordena_heap(FILE** fitas, int quantidade_de_blocos[NUMERO_DE_FITAS], int& numero_de_fitas_utilizadas, long int numero_de_transferencias[2], long int& numero_de_comparacoes);//funcao que intercala um conjunto de fitas, dado a geracao dos blocos inicialmente ordenados
int confere_intercalacao_pronta_ordena_heap(int quantidade_de_blocos[NUMERO_DE_FITAS], long int& numero_de_comparacoes);//funcao que confere se a intercalacao resultou em um unico bloco ordena, ou seja, a intercalacao estah pronta
int apagar_heap(FILE** fitas, int e_s, long int& numero_de_comparacoes);//funcao que apaga as fitas de entradas ou as fitas de saida
int zerou_entrada_heap(int quantidade_de_blocos[NUMERO_DE_FITAS], int e_s, long int& numero_de_comparacoes);//funcao que verifica se o conjunto de fitas de entrada ou de saida estao vazia, ou seja, uma passada ja foi feita
int retirar_o_menor_heap(THQFita notas[TAMANHO_DA_MEMORIA], THQFita* menor, int& tamanho_notas, long int& numero_de_comparacoes);//funcao que recebe um vetpr de registros, retorna o menor elemento e o remove do vetor
int imprime_fitas_heap(FILE** fitas);//funcao que imprime os registros presentes nas fitas
int retornar_pai(int i);//funcao que retorna o pai de um nodo
int retornar_filho_esquerda(int i);//funcao que retorna o filho a esquerda de um nodo
int retornar_filho_direita(int i);//funcao que retorna o filho a direita de um nodo
int inserir_no_heap(THMarcado* heap, THRegistro nota, int& tamanho_heap, THRegistro saiu, long int& numero_de_comparacoes);//funcao que insere adequadamente um registro no Heap
void reconstitui_heap(THMarcado* heap, int tamanho_heap,int i, long int& numero_de_comparacoes);//funcao recusirva que reconstitui heap a partir de uma posicao, geralmente começa na inicial
void remover_do_heap(THMarcado* heap, THRegistro& nota, int& tamanho_heap, long int& numero_de_comparacoes);//funcao que remove um elemento do heap , reconstitui e retona o elemento removido

#endif
