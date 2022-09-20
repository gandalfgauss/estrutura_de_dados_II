
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>
#include <chrono>

#include "intercalacao_2Fitas_heap.h"

using namespace std::chrono;

#pragma warning(disable : 4996)
using namespace std;

//tipo de registro do arquivo binario
struct HRegistro
{
    char inscricao[9];
    double nota;
    char estado[3];
    char cidade[51];
    char curso[31];
};

////tipo de registro que armazena um registro e de qual fita ele veio
struct HQFita
{
    THRegistro nota;
    int de_qual_fita_veio;
};

struct HMarcado
{
	THRegistro registro;
	int marcado;
};

int intercacao_heap_inicia(long int numero_de_transferencias[2], long int& numero_de_comparacoes, double& tempo_de_execucao)
{
	FILE* arquivo;
	int result;
	FILE* fitas[NUMERO_DE_FITAS];
	int tamanho_notas = 0;
	int numero_de_fitas_utilizadas = 0;
	int quantidade_de_blocos[NUMERO_DE_FITAS];
	THRegistro aux;

	//abrir arquivo a ser ordenado
	if ((arquivo = fopen("registrosOrdenados.bin", "r + b")) == NULL)
	{
		//cout << "\nFalha ao abir arquivo de registros inicial" << endl;
		return 0;
	}


	duration<double> time_span;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	//inicializarfitas
	if (!inicializa_fitas_heap(fitas))
	{
		//cout << "\nFalha ao inicializar fitas !";
		return 0;
	}

	//gerar blocos inicialmentes ordenados
	if (!gerar_blocos_ordenados_ordena_heap(arquivo, fitas, quantidade_de_blocos, numero_de_fitas_utilizadas, numero_de_transferencias, numero_de_comparacoes))
	{
		//cout << "\nFalha ao gerar blocos ordenados !";
		return 0;
	}

	//imprime_fitas_heap(fitas);

	//intercalar
	result = intercalar_ordena_heap(fitas, quantidade_de_blocos, numero_de_fitas_utilizadas, numero_de_transferencias, numero_de_comparacoes);
	if (result == 0)
	{
		//cout << "Falha ao intercalar";
	}
	else if (result == 1)//os registros estao ordenados na fita de entrada
	{
		//pular para o inicio dos arquivos
		if (fseek(arquivo, 0, 0) != 0)
		{
			return 0;
		}
		if (fseek(fitas[0], 0, 0) != 0)
		{
			return 0;
		}

		numero_de_transferencias[0]++;

		//ler da fita
		if (fread(&aux, sizeof(THRegistro), 1, fitas[0]) != 1)
		{
			return 0;
		}

		//enquanto nao achar o separador
		while (aux.nota != -1)
		{
			numero_de_transferencias[1]++;
			//sobrescrever no arquivo inicial de registros
			if (fwrite(&aux, sizeof(THRegistro), 1, arquivo) != 1)
			{
				return 0;
			}

			numero_de_transferencias[0]++;

			if (fread(&aux, sizeof(THRegistro), 1, fitas[0]) != 1)
			{
				return 0;
			}
		}


	}
	else//os registros estao ordenados na fita de entrada
	{
		//pular para o inicio dos arquivos
		if (fseek(arquivo, 0, 0) != 0)
		{
			return 0;
		}
		if (fseek(fitas[NUMERO_DE_FITAS / 2], 0, 0) != 0)
		{
			return 0;
		}

		numero_de_transferencias[0]++;
		//ler da fita
		if (fread(&aux, sizeof(THRegistro), 1, fitas[NUMERO_DE_FITAS / 2]) != 1)
		{
			return 0;
		}
		//enquanto nao achar o separador
		while (aux.nota != -1)
		{
			numero_de_transferencias[1]++;
			//sobrescrever no arquivo inicial de registros
			if (fwrite(&aux, sizeof(THRegistro), 1, arquivo) != 1)
			{
				return 0;
			}

			numero_de_transferencias[0]++;
			if (fread(&aux, sizeof(THRegistro), 1, fitas[NUMERO_DE_FITAS / 2]) != 1)
			{
				return 0;
			}
		}
	}
	//jogou os reegistros ordenado no registrosOrdenados.bin

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	time_span = duration_cast<duration<double>>(t2 - t1);

	tempo_de_execucao = time_span.count();

	//fechar fitas e arquivo de registros
	for (int i = 0; i < NUMERO_DE_FITAS; i++)
	{
		fclose(fitas[i]);
	}
	fclose(arquivo);


	return 1;
}

//funcao que inicializa as fitas
int inicializa_fitas_heap(FILE** fitas)
{

	string nome;
	char* c;

	//cria as fitas com nomes 0.bin, 1.bin, ... , (NUMERO_DE_FITAS-1).bin
	for (int i = 0; i < NUMERO_DE_FITAS; i++)
	{
		nome = to_string(i);
		c = const_cast<char*>(nome.c_str());

		//criando uma fita
		if ((fitas[i] = fopen(c, "w + b")) == NULL)
		{
			//cout << "\nFalha ao abir arquivo de registros inicial" << endl;
			return 0;
		}
	}

	return 1;
}

//funcao que gera os blocos ordenados inicialmente nas fitas de entrada utilizando o heap
int gerar_blocos_ordenados_ordena_heap(FILE* arquivo, FILE** fitas, int quantidade_de_blocos[NUMERO_DE_FITAS], int& numero_de_fitas_utilizadas, long int numero_de_transferencias[2], long int& numero_de_comparacoes)
{
	//ler TAMANHO_DA_MEMORIA registros, inserir no heap, remover menor elemento e inserir nas fitas
	THMarcado* heap = new THMarcado[TAMANHO_DA_MEMORIA];//heap
	THRegistro nota;
	THRegistro saiu;
	saiu.nota = -1;//se saiu.nota -1 nao precisa se preocupar com marcacao, pode inserir sem se preocupar
	int tamanho_heap;
	int fita_atual = 0;
	bool aux_numero_fitas = true;//essa variavel ira ajudar a descobrir quantas fitas estao sendo utilizadas
	bool ajuda_heap = false;

	//o separador eh declado, como tendo nota = -1
	THRegistro separador;
	separador.nota = -1;

	//primeiramente de-se inicializar a quantidade de blocos de cada fita com 0
	for (int i = 0; i < NUMERO_DE_FITAS; i++)
	{
		quantidade_de_blocos[i] = 0;
	}

	//ler do arquivo

	tamanho_heap = 0;
	//ler registros do tamanho da memoria e inserir no heap
	for (int i = 0; i < TAMANHO_DA_MEMORIA; i++)
	{
		numero_de_transferencias[0]++;
		if (fread(&nota, sizeof(THRegistro), 1, arquivo) != 1)
		{
			break;
		}
		inserir_no_heap(heap, nota, tamanho_heap, saiu, numero_de_comparacoes);

	}
	//enquanto nao for final de arquivo
	while (!feof(arquivo))
	{
		//se o tamanho do heap for 0, pode retorna pois nao a nada a ser ordenado
		if (tamanho_heap == 0)
		{
			break;
		}

		do//remover um elemento e inserir em uma fita enquanto nao ficar tudo marcado
		{
			remover_do_heap(heap, nota, tamanho_heap, numero_de_comparacoes);//ao mesmo tempo reconstituir la dentro
			
			if (fwrite(&nota, sizeof(THRegistro), 1, fitas[fita_atual]) != 1)//escrever na fita corrreta
			{
				return 0;
			}
			numero_de_transferencias[1]++;
			saiu = nota;

			if (!feof(arquivo))
			{
				numero_de_transferencias[0]++;
				if (fread(&nota, sizeof(THRegistro), 1, arquivo) != 1)//ler proximo elemento
				{
					break;//se sair do looap por aqui eh porque chegou ao final do arquivo ou seja, nao a mais nada a ser inserido
				}
			}
			else
			{
				break;
			}

			ajuda_heap = inserir_no_heap(heap, nota, tamanho_heap, saiu, numero_de_comparacoes);//retorna 0 se estiver tudo marcado, a funcao desmarca automaticamente
			
		} while (ajuda_heap);//enquanto nao estiver tudo marcado
		

		if (!ajuda_heap)//caso tenha saido do loop acima por conta da marcacao de todos os elementos
		{
			if (fwrite(&separador, sizeof(THRegistro), 1, fitas[fita_atual]) != 1)//escreve o separador
			{
				return 0;
			}

			numero_de_transferencias[1]++;

			//atualizar a quantidade de blocos
			quantidade_de_blocos[fita_atual]++;//incrementa um na quatidade de blocos da fita

			//verificar quantas fitas foram utilizadas
			if (fita_atual <= NUMERO_DE_FITAS / 2 - 1 && aux_numero_fitas)
			{
				numero_de_fitas_utilizadas++;
				if (fita_atual == NUMERO_DE_FITAS / 2 - 1)
				{
					aux_numero_fitas = false;
				}
			}

			//se foi inserido na ultima fita, volta para a primeira agora
			if (!ajuda_heap)
			{
				if (fita_atual == NUMERO_DE_FITAS / 2 - 1)
				{
					fita_atual = 0;
				}
				else//se nao, sera inserido os registros nas proximas fitas
				{
					fita_atual++;
				}
			}
		}
		else//caso tenha chegado ao final do arquivo pode sair do loop
		{
			break;
		}
	}
	//conferir se heap esta vazio e inserir o resto
	if (tamanho_heap > 0)
	{
		quantidade_de_blocos[fita_atual]++;//incrementa um na quatidade de blocos da fita

		//verificar quantas fitas foram utilizadas
		if (fita_atual <= NUMERO_DE_FITAS / 2 - 1 && aux_numero_fitas)
		{
			numero_de_fitas_utilizadas++;
			if (fita_atual == NUMERO_DE_FITAS / 2 - 1)
			{
				aux_numero_fitas = false;
			}
		}

		//enquanto existir elementos no heap
		while (tamanho_heap > 0)
		{
			numero_de_comparacoes++;
			if (heap[0].marcado == 1)// se o primeiro elemento do heap estiver marcar
			{
				if (fwrite(&separador, sizeof(THRegistro), 1, fitas[fita_atual]) != 1)//deve escrever o separador de outro bloco
				{
					return 0;
				}

				numero_de_transferencias[1]++;

				//desmarcar os elemento
				for (int k = 0; k < tamanho_heap; k++)
				{
					heap[k].marcado = 0;
				}

				//e inserir na proxima fita
				if (fita_atual == NUMERO_DE_FITAS / 2 - 1)
				{
					fita_atual = 0;
				}
				else
				{
					fita_atual++;
				}

				quantidade_de_blocos[fita_atual]++;// a nova fita vai ter um novo bloco

				//sera contada como uma fita utilizada 
				if (fita_atual <= NUMERO_DE_FITAS / 2 - 1 && aux_numero_fitas)
				{
					numero_de_fitas_utilizadas++;
					if (fita_atual == NUMERO_DE_FITAS / 2 - 1)
					{
						aux_numero_fitas = false;
					}
				}
			}

			//o elemento eh removido do heap e inserido na nova fita
			remover_do_heap(heap, nota, tamanho_heap, numero_de_comparacoes);//ao mesmo tempo reconstituir la dentro
			if (fwrite(&nota, sizeof(THRegistro), 1, fitas[fita_atual]) != 1)
			{
				return 0;
			}
			numero_de_transferencias[1]++;

			if (tamanho_heap == 0)//se chegar ao ultimo elemento do heap
			{
				if (fwrite(&separador, sizeof(THRegistro), 1, fitas[fita_atual]) != 1)//deve-se colocar o separador
				{
					return 0;
				}
				numero_de_transferencias[1]++;
			}
		}
	}

	delete[] heap;
	return 1;
}

//funcao que retorna o pai de um nodo
int retornar_pai(int i)
{
	return (i - 1) / 2;
}

//funcao que retorna o filho a esquerda de um nodo
int retornar_filho_esquerda(int i)
{
	return (2 * i) + 1;
}

//funcao que retorna o filho a direita de um nodo
int retornar_filho_direita(int i)
{
	return (2 * i) + 2;;
}

//funcao que insere adequadamente um registro no Heap
int inserir_no_heap(THMarcado* heap, THRegistro nota, int& tamanho_heap, THRegistro saiu, long int& numero_de_comparacoes)
{
	THMarcado aux;
	tamanho_heap++;//aumenta o tamanho heap, pois um elemento vai ser inserido
	int i = tamanho_heap - 1;

	if (saiu.nota == -1)//se o elemento que saiu tiver a nota -1, entao esta na primeira etapa entao nao precisa se preocupar com elementos marcados
	{
		aux.registro = nota;
		aux.marcado = 0; // inserir desmarcado inicialmente
		heap[i] = aux;//insere na ultima posicao

		//enquanto nao chegar ate o pai e o registro do pai for maior que o do filho
		while (i != 0 && heap[retornar_pai(i)].registro.nota > heap[i].registro.nota)
		{
			//troca o pai com o filho, pois filho eh menor
			numero_de_comparacoes++;
			aux = heap[retornar_pai(i)];
			heap[retornar_pai(i)] = heap[i];
			heap[i] = aux;
			i = retornar_pai(i);
		}
	}
	else//conferir se o elemento que entrou eh menor que saiu
	{
		aux.registro = nota;//aux recebe o registro completo a ser inserido
		
		numero_de_comparacoes++;
		//aux eh marcado caso o elemento que entrou seja menor que o elemento que saiu
		if (nota.nota < saiu.nota)
		{
			aux.marcado = 1;
		}
		else
		{
			aux.marcado = 0;
		}

		//a ultima posicao do heap recebe o aux
		heap[i] = aux;

		//enquanto nao chegar ate o pai e o registro do pai for maior que o do filho ou entao o pai esta marcado e o filho nao estah
		while (i != 0 && heap[retornar_pai(i)].registro.nota > heap[i].registro.nota || (heap[retornar_pai(i)].marcado == 1 && heap[i].marcado == 0))
		{
			numero_de_comparacoes++;
			//so nao troca filho com o pai, se o filho estiver marcado e pai desmarcado
			numero_de_comparacoes++;
			if (heap[retornar_pai(i)].marcado == 0 && heap[i].marcado == 1)//nao troca
			{
				break;
			}
			else //troca
			{
				aux = heap[retornar_pai(i)];
				heap[retornar_pai(i)] = heap[i];
				heap[i] = aux;
				i = retornar_pai(i);
			}
			
		}

	}

	numero_de_comparacoes++;
	if (heap[0].marcado == 1)///se o primeiro elemento do hea estiver marcado
	{
		//desmarcar toodo mundo e retornar 0, para o proximo bloco
		for (int j = 0; j < tamanho_heap; j++)
		{
			heap[j].marcado = 0;
		}

		return 0;
	}

	return 1;
	
}

//funcao recusirva que reconstitui heap a partir de uma posicao, geralmente começa na inicial
void reconstitui_heap(THMarcado* heap, int tamanho_heap, int i, long int& numero_de_comparacoes)
{
	THMarcado aux;
	//posicoes dos filhos da esquerda e da direita
	int esquerda = retornar_filho_esquerda(i);
	int direita = retornar_filho_direita(i);

	//o menor elemento recebe inicialmente a posicao i (geralmente a primeira posicao, ou seja, a posicao 0 do heap)
	THMarcado menor;
	menor = heap[i];
	int posic = -1;//posicao quee deve ser trocada com i

	//trocar com o menor filho
	numero_de_comparacoes++;
	//se o elemento atual e seu filho da esquerda estiverem marcados
	if ((heap[esquerda].marcado == 1) && (menor.marcado == 1))
	{
		numero_de_comparacoes++;
		//e se a esquerda for menor que o tamanho maximo do heap e o menor for menor que o filho da esquerda
		if (esquerda < tamanho_heap && (heap[esquerda].registro.nota < menor.registro.nota))
		{
			//entao menor recebe filho da esquerda
			menor = heap[esquerda];
			posic = esquerda;
		}
	}
	else if ((heap[esquerda].marcado == 0) && (menor.marcado == 1))//se o elemento atual estiver marcado e seu filho da esquerda nao
	{
		numero_de_comparacoes++;
		numero_de_comparacoes++;

		//e se a esquerda for menor que o tamanho maximo do heap
		if (esquerda < tamanho_heap )
		{
			//entao menor recebe filho da esquerda
			menor = heap[esquerda];
			posic = esquerda;
		}
	}
	else if ((heap[esquerda].marcado == 0) && (menor.marcado == 0))//se o elemento atual e seu filho da esquerda estiverem desmarcados
	{
		numero_de_comparacoes++;
		numero_de_comparacoes++;
		//e se a esquerda for menor que o tamanho maximo do heap e se o menor for menor que o filho da esquerda
		if (esquerda < tamanho_heap && (heap[esquerda].registro.nota <menor.registro.nota))
		{
			//entao menor recebe filho da esquerda
			menor = heap[esquerda];
			posic = esquerda;
		}
	}

	numero_de_comparacoes++;
	//se o elemento atual e seu filho da direita estiverem marcados
	if ((heap[direita].marcado == 1) && (menor.marcado == 1))
	{
		numero_de_comparacoes++;
		//e se a direita for menor que o tamanho maximo do heap e o se o menor for menor que o filho da direita
		if (direita < tamanho_heap && (heap[direita].registro.nota < menor.registro.nota))
		{
			//entao menor recebe filho da esquerda
			menor = heap[direita];
			posic = direita;
		}
	}
	else if ((heap[direita].marcado == 0) && (menor.marcado == 1))//se o elemento atual estiver marcado e seu filho da direita nao
	{
		numero_de_comparacoes++;
		numero_de_comparacoes++;
		//e se a direita for menor que o tamanho maximo do heap
		if (direita < tamanho_heap )
		{
			//entao menor recebe filho da direita
			menor = heap[direita];
			posic = direita;
		}
	}
	else if ((heap[direita].marcado == 0) && (menor.marcado == 0))//se o elemento atual e seu filho da direita estiverem desmarcados
	{
		numero_de_comparacoes++;
		numero_de_comparacoes++;
		//e se a direita for menor que o tamanho maximo do heap e se o menor for menor que o filho da direita
		if (direita < tamanho_heap && (heap[direita].registro.nota < menor.registro.nota))
		{
			//entao menor recebe filho da direita
			posic = direita;
			menor = heap[direita];
		}
	}

	numero_de_comparacoes++;
	if (posic != -1)//se posic valer -1 quer dizer que nao houve troca entao a chamada recursiva eh encerrada
	{
		//caso posic seja diferente de -1, entao a uma troca da posicao i (atual) e o menor elemento
		aux = heap[i];
		heap[i] = menor;
		heap[posic] = aux;
		reconstitui_heap(heap, tamanho_heap,posic, numero_de_comparacoes);//o processo continua recurssivamente a partir da posicao retornada(posic)

	}
}

//funcao que remove um elemento do heap , reconstitui e retona o elemento removido
void remover_do_heap(THMarcado* heap, THRegistro& nota, int& tamanho_heap, long int& numero_de_comparacoes)
{
	//se o tamanho do heap eh 0, nao tem nada a ser removido
	if (tamanho_heap == 0)
	{
		return;
	}
	else if (tamanho_heap == 1)//se for 1 nao precisa reconstituir
	{
		numero_de_comparacoes++;
		tamanho_heap--;//o tamanho do heap eh decrementado
		nota = heap[0].registro;//o elemento a ser removido eh sempre o primeiro
	}
	else//caso contrario
	{
		numero_de_comparacoes++;
		nota = heap[0].registro;//o primeiro registro eh removido 
		heap[0] = heap[tamanho_heap - 1];//e troca com o ultimo elemento
		tamanho_heap--;//o tamanho do heap eh decrementado
		reconstitui_heap(heap, tamanho_heap, 0, numero_de_comparacoes);//e o heap eh reconstituido
	}
		
		
}

//funcao que intercala um conjunto de fitas, dado a geracao dos blocos inicialmente ordenados
int intercalar_ordena_heap(FILE** fitas, int quantidade_de_blocos[NUMERO_DE_FITAS], int& numero_de_fitas_utilizadas, long int numero_de_transferencias[2], long int& numero_de_comparacoes)
{
	bool aux_numero_de_fitas = true;//essa variavel ira ajudar a descobrir quantas fitas estao sendo utilizadas
	THQFita* notas = new THQFita[TAMANHO_DA_MEMORIA];//vetor de registros do tamanho da memoria
	int fita_insercao;//fita que sera inserido um bloco
	int confere_pronto = 0;//variavel que ajuda a fazer o controle se a intercalacao resultou em um unico bloco, e aonde esta esse bloco
	confere_pronto = confere_intercalacao_pronta_ordena_heap(quantidade_de_blocos, numero_de_comparacoes);
	bool entrada_saida = true; // ajuda a controlar a variacao entre fitas de entrada e fitas de saida, true e entrada, false e saida
	int tamanho_notas;
	THQFita menor;
	int posic;
	int numero_de_fitas_utilizadas_aux;
	//inicializacao de um separador de blocos, onde nota do separador tem valor -1
	THRegistro separador;
	separador.nota = -1;

	int quantidade = 0;

	if (confere_pronto == 1)//esta na fita de entrada os dados ordenados
	{
		return 1;
	}
	else if (confere_pronto == 2)//esta na fita de saida os dados ordenados
	{

		return 2;
	}

	//enquanto nao terminar a intercalacao com apenas um bloco em alguma fita
	while (confere_pronto == 0)
	{
		tamanho_notas = 0;
		numero_de_fitas_utilizadas_aux = 0;
		aux_numero_de_fitas = true;

		for (int i = 0; i < NUMERO_DE_FITAS; i++)//colocar o ponteiro das fitas de entrada e saida apontando pro inicio
		{
			if (fseek(fitas[i], 0, 0) != 0)
			{
				return 0;
			}
		}

		if (entrada_saida)//se os blocos estao na fita de entrada tem que passa-los para as fitas de saida
		{
			fita_insercao = NUMERO_DE_FITAS / 2;//a primeira fita de saida tem a posicao NUMERO_DE_FITAS/2

			//faca
			do
			{
				//ler primeiros registros das fitas de entrada
				for (int i = 0; i < numero_de_fitas_utilizadas; i++)
				{
					numero_de_transferencias[0]++;
					if (fread(&notas[i].nota, sizeof(THRegistro), 1, fitas[i]) != 1)
					{
						break;
					}

					//salvar de qual fita veio
					notas[i].de_qual_fita_veio = i;
					tamanho_notas++;
				}

				do
				{
					//retirar o menor, inserir numa fita de saida, ate encontrar um separador
					posic = retirar_o_menor_heap(notas, &menor, tamanho_notas, numero_de_comparacoes);

					numero_de_comparacoes++;
					if (posic != -1)
					{
						numero_de_transferencias[1]++;
						if (fwrite(&menor.nota, sizeof(THRegistro), 1, fitas[fita_insercao]) != 1)
						{
							return 0;
						}

						numero_de_transferencias[0]++;
						//ler o proximo da onde foi retirado
						if (fread(&notas[tamanho_notas].nota, sizeof(THRegistro), 1, fitas[menor.de_qual_fita_veio]) != 1)
						{
							continue;
						}

						numero_de_comparacoes++;
						if (notas[tamanho_notas].nota.nota != -1)//nao achei o separador
						{
							notas[tamanho_notas].de_qual_fita_veio = menor.de_qual_fita_veio;
							tamanho_notas++;

						}
					}
				} while (posic != -1);

				//remover blocos das fitas de entrada
				for (int i = 0; i < numero_de_fitas_utilizadas; i++)
				{
					quantidade_de_blocos[i]--;
					if (quantidade_de_blocos[i] < 0)
					{
						quantidade_de_blocos[i] = 0;
					}
				}

				//inserir separador
				if (fwrite(&separador, sizeof(THRegistro), 1, fitas[fita_insercao]) != 1)//escrever separador de blocos
				{
					return 0;
				}
				numero_de_transferencias[1]++;

				//atualizar a quantidade de blocos na fita que foi inserida
				quantidade_de_blocos[fita_insercao]++;

				//conferir a quantidade de fitas que serao utilizadas na proxima iteracao
				if (fita_insercao <= NUMERO_DE_FITAS && aux_numero_de_fitas)
				{
					numero_de_fitas_utilizadas_aux++;
					if (fita_insercao == NUMERO_DE_FITAS - 1)
					{
						aux_numero_de_fitas = false;
					}
				}
				//se chegar na ultima fita, a proxima fita a ser inserida eh a de posicao NUMERO_DE_FITAS/2
				if (fita_insercao == NUMERO_DE_FITAS - 1)
				{
					fita_insercao = NUMERO_DE_FITAS / 2;
				}
				else//se nao, eh so avancar uma posicao
				{
					fita_insercao++;
				}

			} while (zerou_entrada_heap(quantidade_de_blocos, 0, numero_de_comparacoes));//enquanto nao acabar os blocos das fitas de entrada

			// apos inserir nas fitas de saida deve-se apagar as fitas de entrada
			apagar_heap(fitas, 0, numero_de_comparacoes);

			//o numero de fitas utilizada eh exatamento o numero_de_fita_auxiliares
			//e a proxima intercalacao ocorrera numa fita de entrada
			numero_de_fitas_utilizadas = numero_de_fitas_utilizadas_aux;
			entrada_saida = false;
		}
		else//se os blocos estao na fita de saida tem que passa-los para as fitas de entrada
		{
			fita_insercao = 0;//a primeira fita de entrada tem a posicao 0

			do
			{
				///ler primeiros registros das fitas de saida
				for (int i = NUMERO_DE_FITAS / 2; i < NUMERO_DE_FITAS / 2 + numero_de_fitas_utilizadas; i++)
				{
					numero_de_transferencias[0]++;
					if (fread(&notas[i - NUMERO_DE_FITAS / 2].nota, sizeof(THRegistro), 1, fitas[i]) != 1)
					{
						break;
					}
					//salvar de qual fita veio
					notas[i - NUMERO_DE_FITAS / 2].de_qual_fita_veio = i;
					tamanho_notas++;
				}

				do
				{
					//retirar o menor, inserir numa fita de saida, ate encontrar um separador
					posic = retirar_o_menor_heap(notas, &menor, tamanho_notas, numero_de_comparacoes);

					numero_de_comparacoes++;
					if (posic != -1)
					{

						if (fwrite(&menor.nota, sizeof(THRegistro), 1, fitas[fita_insercao]) != 1)
						{
							return 0;
						}
						numero_de_transferencias[1]++;
						//ler o proximo da onde foi retirado

						if (fread(&notas[tamanho_notas].nota, sizeof(THRegistro), 1, fitas[menor.de_qual_fita_veio]) != 1)
						{
							continue;
						}

						numero_de_transferencias[0]++;

						numero_de_comparacoes++;
						if (notas[tamanho_notas].nota.nota != -1)//nao achei o separador
						{
							notas[tamanho_notas].de_qual_fita_veio = menor.de_qual_fita_veio;
							tamanho_notas++;
						}
					}
				} while (posic != -1);

				//remover blocos das fitas de saida
				for (int i = NUMERO_DE_FITAS / 2; i < NUMERO_DE_FITAS / 2 + numero_de_fitas_utilizadas; i++)
				{
					quantidade_de_blocos[i]--;
					if (quantidade_de_blocos[i] < 0)
					{
						quantidade_de_blocos[i] = 0;
					}
				}

				//inserir separador
				if (fwrite(&separador, sizeof(THRegistro), 1, fitas[fita_insercao]) != 1)//escrever separador de blocos
				{
					return 0;
				}
				numero_de_transferencias[1]++;

				quantidade_de_blocos[fita_insercao]++;
				//conferir a quantidade de fitas que serao utilizadas na proxima iteracao
				if (fita_insercao <= NUMERO_DE_FITAS / 2 - 1 && aux_numero_de_fitas)
				{
					numero_de_fitas_utilizadas_aux++;
					if (fita_insercao == NUMERO_DE_FITAS / 2 - 1)
					{
						aux_numero_de_fitas = false;
					}
				}
				//se chegar na ultima fita, a proxima fita a ser inserida eh a de posicao 0
				if (fita_insercao == NUMERO_DE_FITAS / 2 - 1)
				{
					fita_insercao = 0;
				}
				else//se nao, eh so avancar uma posicao
				{
					fita_insercao++;
				}

			} while (zerou_entrada_heap(quantidade_de_blocos, 1, numero_de_comparacoes));//enquanto nao acabar os blocos das fitas de saida

			//o numero de fitas utilizada eh exatamento o numero_de_fita_auxiliares
			//e a proxima intercalacao ocorrera numa fita de saida
			numero_de_fitas_utilizadas = numero_de_fitas_utilizadas_aux;
			entrada_saida = true;

			// apos inserir nas fitas de saida deve-se apagar as fitas de entrada
			apagar_heap(fitas, 1, numero_de_comparacoes);
		}
		//apos terminar um processo de intercalacao deve-se conferir se a intercalacao esta completa
		confere_pronto = confere_intercalacao_pronta_ordena_heap(quantidade_de_blocos, numero_de_comparacoes);

	}

	delete[] notas;
	return confere_pronto;

}

//funcao que confere se a intercalacao resultou em um unico bloco ordena, ou seja, a intercalacao estah pronta
int confere_intercalacao_pronta_ordena_heap(int quantidade_de_blocos[NUMERO_DE_FITAS], long int& numero_de_comparacoes)
{
	int pronto = 0;

	//se a primeira fita de entrada estiver com 1 bloco
	if (quantidade_de_blocos[0] == 1)
	{
		pronto = 1;

		for (int i = 1; i < NUMERO_DE_FITAS / 2; i++)
		{
			//e todas as outras fitas de entrada estiverem sem nenhum outro bloco
			if (quantidade_de_blocos[i] != 0)
			{
				pronto = 0;
				break;
			}
		}
		//entao sera retornado pronto valendo 1, ou seja, o bloco ordenado esta na primeira fita de entrada
	}
	else if (quantidade_de_blocos[NUMERO_DE_FITAS / 2] == 1)//se a primeira fita de saida estiver com 1 bloco
	{

		pronto = 2;

		for (int i = NUMERO_DE_FITAS / 2 + 1; i < NUMERO_DE_FITAS; i++)
		{
			//e todas as outras fitas de saida estiverem sem nenhum outro bloco
			if (quantidade_de_blocos[i] != 0)
			{
				pronto = 0;
			}
			break;
		}
		//entao sera retornado pronto valendo 2, ou seja, o bloco ordenado esta na primeira fita de saida
	}

	return pronto;

}

//funcao que apaga as fitas de entradas ou as fitas de saida
int apagar_heap(FILE** fitas, int e_s, long int& numero_de_comparacoes)
{
	string nome;
	char* c;

	if (e_s == 0)//apagar as fitas de entrada
	{
		for (int i = 0; i < NUMERO_DE_FITAS / 2; i++)
		{
			nome = to_string(i);
			c = const_cast<char*>(nome.c_str());

			fclose(fitas[i]);
			if ((fitas[i] = fopen(c, "w + b")) == NULL)
			{
				//cout << "\nFalha ao abir arquivo de registros inicial" << endl;
				return 0;
			}
		}

		return 1;
	}
	else//apagar as fitas de saida
	{
		for (int i = NUMERO_DE_FITAS / 2; i < NUMERO_DE_FITAS; i++)
		{
			nome = to_string(i);
			c = const_cast<char*>(nome.c_str());

			fclose(fitas[i]);
			if ((fitas[i] = fopen(c, "w + b")) == NULL)
			{
				//cout << "\nFalha ao abir arquivo de registros inicial" << endl;
				return 0;
			}
		}

		return 1;
	}
}

//funcao que verifica se o conjunto de fitas de entrada ou de saida estao vazia, ou seja, uma passada ja foi feita
int zerou_entrada_heap(int quantidade_de_blocos[NUMERO_DE_FITAS], int e_s, long int& numero_de_comparacoes)
{
	int achou = 0;

	if (e_s == 0)// verifica se as fitas de entrada estao vazias
	{
		for (int i = 0; i < NUMERO_DE_FITAS / 2; i++)
		{
			if (quantidade_de_blocos[i] != 0)
			{
				achou = 1;
				break;
			}
		}
	}
	else// verifica se as fitas de saida estao vazias
	{
		for (int i = NUMERO_DE_FITAS / 2; i < NUMERO_DE_FITAS; i++)
		{
			if (quantidade_de_blocos[i] != 0)
			{
				achou = 1;
				break;
			}
		}
	}

	return achou;
}

//funcao que recebe um vetpr de registros, retorna o menor elemento e o remove do vetor
int retirar_o_menor_heap(THQFita notas[TAMANHO_DA_MEMORIA], THQFita* menor, int& tamanho_notas, long int& numero_de_comparacoes)
{
	int posic;
	numero_de_comparacoes++;
	if (tamanho_notas == 0)
	{
		return -1;
	}

	//inicialmente o menor recebe a primeira posicao do vetor
	*menor = notas[0];
	posic = 0;

	for (int i = 1; i < tamanho_notas; i++)
	{
		numero_de_comparacoes++;
		//se algum registro tiver a nota menor, o menor recebe esse registro
		if (notas[i].nota.nota < menor->nota.nota)
		{
			*menor = notas[i];
			posic = i;//salva a posicao onde se encontra o menor elemento
		}
	}

	//remover o menor
	for (int i = posic; i < tamanho_notas - 1; i++)
	{
		notas[i] = notas[i + 1];
	}
	tamanho_notas--;

	return 1;
}

//funcao que imprime os registros presentes nas fitas
int imprime_fitas_heap(FILE** fitas)
{
	THRegistro aux;
	for (int i = 0; i < NUMERO_DE_FITAS; i++)//colocar o ponteiro das fitas  apontando pro inicio
	{
		if (fseek(fitas[i], 0, 0) != 0)
		{
			return 0;
		}
	}

	//imprimir conteudo das fitas
	int j;
	for (int i = 0; i < NUMERO_DE_FITAS; i++)
	{
		j = 0;
		cout << "\n\nFita: " << i << endl;
		while (fread(&aux, sizeof(THRegistro), 1, fitas[i]) == 1)
		{
			cout << "\n\nAluno " << j << ": " << endl;
			cout << "\nInscricao : " << aux.inscricao << endl;
			cout << "\nNota : " << aux.nota << endl;

			j++;
		}
	}

	return 1;
}

