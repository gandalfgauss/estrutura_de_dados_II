#ifndef QUICK_SORT
#define QUICK_SORT

using namespace std;

//todas as funcoes com o tipo de retorna inteiro, retornam 1 se a operacao foi realizada com sucesso e 0 caso contrario

typedef struct QRegistro TQRegistro;//estrutura de registros do arquivo
int quick_inicia(int tipo_arquivo, long int numero_de_transferencias[2], long int& numero_de_comparacoes, double& tempo_de_execucao, int tamanho_do_arquivo);//funcao que faz os preparativos do algoritmo e chama o quicksort
void RetiraMin(TQRegistro* area, TQRegistro* R, int* tamanho_da_area);//funcao que retira o menor elemento da area e a reconstitui
void RetiraMax(TQRegistro* area, TQRegistro* R, int* tamanho_da_area);//funcao que retiro o maior elemento da area
int EscreveMin(FILE** ArqEi, TQRegistro R, int* Ei, long int numero_de_transferencias[2]);//funcao que escreve um registro no subarquivo inferior
int EscreveMax(FILE** ArqLEs, TQRegistro R, int* Es, long int numero_de_transferencias[2]);//funcao que escreve um registro no subarquivo superior
void InserirArea(TQRegistro* area, TQRegistro* R, int* tamanho_da_area, long int& numero_de_comparacoes);//funcao que insere um registro na area
int LeInf(FILE** ArqLi, TQRegistro* R, int* Li, bool* OndeLer, long int numero_de_transferencias[2]);//funcao que le um registro na parte inferior do arquivo
int LeSup(FILE** ArqLEs, TQRegistro* R, int* Ls, bool* OndeLer, long int numero_de_transferencias[2]);//funcao que le um registro na parte superior do arquivo
int Particao(FILE** ArqLi, FILE** ArqEi, FILE** ArqLEs, TQRegistro* area, int Esq, int Dir, int* i, int* j, long int numero_de_transferencias[2], long int& numero_de_comparacoes);//funcao que particiona o arquivo em dois outros subarquivos, ordena o pivô e passa os elementos menores para esquerda e maiores para direita do mesmo
int QuicksortExterno(FILE** ArqLi, FILE** ArqEi, FILE** ArqLEs, int Esq, int Dir, long int numero_de_transferencias[2], long int& numero_de_comparacoes);//funcao que ordena um arquivo



#endif // !QUICK_SORT
