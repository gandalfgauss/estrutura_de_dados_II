#ifndef INTERCALACAO_2FITAS_ORDENACAO_H
#define INTERCALACAO_2FITAS_ORDENACAO_H

#define TAMANHO_DA_MEMORIA 20
#define NUMERO_DE_FITAS 40// 20 fitas de entrada e 20 de saidas

typedef struct ORegistro TORegistro;//tipo de registro do arquivo binario
typedef struct QFita TQFita;////tipo de registro que armazena um registro e de qual fita ele veio

int inicializa_fitas(FILE** fitas);//funcao que inicializa as fitas
int intercacao_ordenacao_inicia(long int numero_de_transferencias[2], long int& numero_de_comparacoes, double& tempo_de_execucao); //funcao que inicializa as fitas e chama as funcoes de gerar os registros inicialmentes ordenado, e a funcao de intercalacao
void ordena(TORegistro* registros, int inicio, int final, long int& numero_de_comparacoes);//funcao que ordena um vetor de registros
int gerar_blocos_ordenados_ordena(FILE* arquivo, FILE** fitas, int quantidade_de_blocos[NUMERO_DE_FITAS], int& numero_de_fitas_utilizadas, long int numero_de_transferencias[2], long int& numero_de_comparacoes);//funcao que gera os blocos ordenados inicialmente nas fitas de entrada
int confere_intercalacao_pronta_ordena(int quantidade_de_blocos[NUMERO_DE_FITAS], long int& numero_de_comparacoes);//funcao que confere se a intercalacao resultou em um unico bloco ordena, ou seja, a intercalacao estah pronta
int intercalar_ordena(FILE** fitas, int quantidade_de_blocos[NUMERO_DE_FITAS], int& numero_de_fitas_utilizadas, long int numero_de_transferencias[2], long int& numero_de_comparacoes);//funcao que intercala um conjunto de fitas, dado a geracao dos blocos inicialmente ordenados
int retirar_o_menor(TQFita notas[TAMANHO_DA_MEMORIA], TQFita* menor, int& tamanho_notas,long int& numero_de_comparacoes);//funcao que recebe um vetpr de registros, retorna o menor elemento e o remove do vetor
int zerou_entrada(int quantidade_de_blocos[NUMERO_DE_FITAS], int e_s,  long int& numero_de_comparacoes);//funcao que verifica se o conjunto de fitas de entrada ou de saida estao vazia, ou seja, uma passada ja foi feita
int apagar(FILE** fitas, int e_s, long int& numero_de_comparacoes);//funcao que apaga as fitas de entradas ou as fitas de saida
int imprime_fitas(FILE** fitas);//funcao que imprime os registros presentes nas fitas

#endif // !INTERCALACAO_2FITAS_ORDENACAO_H



