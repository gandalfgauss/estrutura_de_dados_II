#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>
#include <chrono>
#include "intercalacao_2Fitas_ordenacao.h"

using namespace std::chrono;

#pragma warning(disable : 4996)
using namespace std;

//tipo de registro do arquivo binario
struct ORegistro
{
    char inscricao[9];
    double nota;
    char estado[3];
    char cidade[51];
    char curso[31];
};

////tipo de registro que armazena um registro e de qual fita ele veio
struct QFita
{
	TORegistro nota;
	int de_qual_fita_veio;
};

//funcao que inicializa as fitas e chama as funcoes de gerar os registros inicialmentes ordenado, e a funcao de intercalacao
int intercacao_ordenacao_inicia(long int numero_de_transferencias[2], long int& numero_de_comparacoes, double& tempo_de_execucao)
{
    FILE* arquivo;
	int result;
    FILE* fitas[NUMERO_DE_FITAS];
    int tamanho_notas = 0;
    int numero_de_fitas_utilizadas = 0;
    int quantidade_de_blocos[NUMERO_DE_FITAS];
	TORegistro aux;

    //abrir arquivo a ser ordenado
    if ((arquivo = fopen("registrosOrdenados.bin", "r + b")) == NULL)
    {
        //cout << "\nFalha ao abir arquivo de registros inicial" << endl;
        return 0;
    }

	
	duration<double> time_span;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	//inicializarfitas
	if(!inicializa_fitas(fitas))
	{ 
		//cout << "\nFalha ao inicializar fitas !";
		return 0;
	}

    //gerar blocos inicialmentes ordenados
	if (!gerar_blocos_ordenados_ordena(arquivo, fitas, quantidade_de_blocos, numero_de_fitas_utilizadas, numero_de_transferencias, numero_de_comparacoes))
	{
		//cout << "\nFalha ao gerar blocos ordenados !";
		return 0;
	}

	//intercalar
	result = intercalar_ordena(fitas, quantidade_de_blocos, numero_de_fitas_utilizadas, numero_de_transferencias, numero_de_comparacoes);
	if (result == 0)
	{
		//cout << "Falha ao intercalar";
	}
	else if (result == 1)//os registros estao ordenados na fita de entrada
	{
		//pular para o inicio dos arquivos
		if (fseek(arquivo,0, 0) != 0)
		{
			return 0;
		}
		if (fseek(fitas[0], 0, 0) != 0)
		{
			return 0;
		}

		numero_de_transferencias[0]++;

		//ler da fita
		if (fread(&aux, sizeof(TORegistro), 1, fitas[0]) != 1)
		{
			return 0;
		}

		//enquanto nao achar o separador
		while (aux.nota != -1)
		{
			numero_de_transferencias[1]++;
			//sobrescrever no arquivo inicial de registros
			if (fwrite(&aux, sizeof(TORegistro), 1, arquivo) != 1)
			{
				return 0;
			}

			numero_de_transferencias[0]++;

			if (fread(&aux, sizeof(TORegistro), 1, fitas[0]) != 1)
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
		if (fseek(fitas[NUMERO_DE_FITAS/2], 0, 0) != 0)
		{
			return 0;
		}

		numero_de_transferencias[0]++;
		//ler da fita
		if (fread(&aux, sizeof(TORegistro), 1, fitas[NUMERO_DE_FITAS/2]) != 1)
		{
			return 0;
		}
		//enquanto nao achar o separador
		while (aux.nota != -1)
		{
			numero_de_transferencias[1]++;
			//sobrescrever no arquivo inicial de registros
			if (fwrite(&aux, sizeof(TORegistro), 1, arquivo) != 1)
			{
				return 0;
			}

			numero_de_transferencias[0]++;
			if (fread(&aux, sizeof(TORegistro), 1, fitas[NUMERO_DE_FITAS/2]) != 1)
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


//funcao que imprime os registros presentes nas fitas
int imprime_fitas(FILE** fitas)
{
	TORegistro aux;
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
		while (fread(&aux, sizeof(TORegistro), 1, fitas[i]) == 1)
		{
			cout << "\n\nAluno " << j << ": " << endl;
			cout << "\nInscricao : " << aux.inscricao << endl;
			cout << "\nNota : " << aux.nota << endl;

			j++;
		}
	}

	return 1;
}

//funcao que inicializa as fitas
int inicializa_fitas(FILE** fitas)
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

//funcao que ordena um vetor de registros
void ordena(TORegistro* registros, int inicio, int final, long int& numero_de_comparacoes)//paramentos 0 e n
{
	int i, j;
	double pivo;
	TORegistro aux;
	i = inicio;
	j = final - 1;
	//o pivo recebe o elemento do meio
	pivo = registros[(inicio + final) / 2].nota;

	//enquanto os indices nao se cruzarem
	while (i <= j)
	{
		numero_de_comparacoes++;
		while (registros[i].nota < pivo && i < final)
		{
			numero_de_comparacoes++;
			i++;
		}
		while (registros[j].nota > pivo && j > inicio)
		{
			numero_de_comparacoes++;
			j--;
		}
		//ha uma troca de elementos caso um elemento menor que o pivô esteja a direita do mesmo,
		//ou um elemento maior que o pivo esteja na esquerda do mesmo
		if (i <= j)
		{
			numero_de_comparacoes++;
			aux = registros[i];
			registros[i] = registros[j];
			registros[j] = aux;
			i++;
			j--;
		}
	}
	//chama recursivamente o quick sort para as duas particoes
	if (j > inicio)
	{
		numero_de_comparacoes++;
		ordena(registros, inicio, j + 1, numero_de_comparacoes);
	}
		
	if (i < final)
	{
		numero_de_comparacoes++;
		ordena(registros, i, final, numero_de_comparacoes);
	}
		
}

//funcao que gera os blocos ordenados inicialmente nas fitas de entrada
int gerar_blocos_ordenados_ordena(FILE* arquivo, FILE** fitas, int quantidade_de_blocos[NUMERO_DE_FITAS], int &numero_de_fitas_utilizadas, long int numero_de_transferencias[2], long int& numero_de_comparacoes)
{
    //ler TAMANHO_DA_MEMORIA registros, ordenar e inserir nas fitas
	TORegistro* notas = new TORegistro[TAMANHO_DA_MEMORIA];
	int tamanho_notas;
	int fita_atual = 0;
	bool aux_numero_fitas = true;//essa variavel ira ajudar a descobrir quantas fitas estao sendo utilizadas

	//o separador eh declado, como tendo nota = -1
	TORegistro separador;
	separador.nota = -1;

	//primeiramente de-se inicializar a quantidade de blocos de cada fita com 0
	for (int i = 0; i < NUMERO_DE_FITAS; i++)
	{
		quantidade_de_blocos[i] = 0;
	}

	//ler do arquivo
	//enquanto
	do
	{
		tamanho_notas = 0;
		//ler registros do tamanho da memoria
		for (int i = 0; i < TAMANHO_DA_MEMORIA; i++)
		{
			numero_de_transferencias[0]++;
			if (fread(&notas[i], sizeof(TORegistro), 1, arquivo) != 1)
			{
				break;
			}

			tamanho_notas++;
		}

		if (tamanho_notas == 0)
		{
			break;
		}

		//ordenar os registros em memoria principals
		ordena(notas, 0, tamanho_notas,numero_de_comparacoes);

		
		//inserir nas fitas
		if (fwrite(notas, sizeof(TORegistro), tamanho_notas,fitas[fita_atual]) != tamanho_notas)
		{
			return 0;
		}
		numero_de_transferencias[1]++;

		//inserir separador de blocos
		if (fwrite(&separador, sizeof(TORegistro), 1, fitas[fita_atual]) != 1)
		{
			return 0;
		}
		numero_de_transferencias[1]++;

		//atualizar a quantidade de blocos
		quantidade_de_blocos[fita_atual]++;


		//verificar quantas fitas foram utilizadas
		if (fita_atual <= NUMERO_DE_FITAS/2-1 && aux_numero_fitas)
		{
			numero_de_fitas_utilizadas++;
			if (fita_atual == NUMERO_DE_FITAS/2 - 1)
			{
				aux_numero_fitas = false;
			}
		}

		//se foi inserido na ultima fita, volta para a primeira agora
		if (fita_atual == NUMERO_DE_FITAS/2 -1)
		{
			fita_atual = 0;
		}
		else//se nao, sera inserido os registros na proxima fitas
		{
			fita_atual++;
		}
	} while (!feof(arquivo));

	delete[] notas;
	return 1;
}

//funcao que confere se a intercalacao resultou em um unico bloco ordena, ou seja, a intercalacao estah pronta
int confere_intercalacao_pronta_ordena(int quantidade_de_blocos[NUMERO_DE_FITAS], long int& numero_de_comparacoes)
{
	int pronto =  0;

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

		for (int i = NUMERO_DE_FITAS/2 +1; i < NUMERO_DE_FITAS; i++)
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

//funcao que recebe um vetpr de registros, retorna o menor elemento e o remove do vetor
int retirar_o_menor(TQFita notas[TAMANHO_DA_MEMORIA], TQFita* menor, int& tamanho_notas, long int& numero_de_comparacoes)
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
	for (int i = posic; i < tamanho_notas-1; i++)
	{
		notas[i] = notas[i + 1];
	}
	tamanho_notas--;

	return 1;
}

//funcao que verifica se o conjunto de fitas de entrada ou de saida estao vazia, ou seja, uma passada ja foi feita
int zerou_entrada(int quantidade_de_blocos[NUMERO_DE_FITAS], int e_s, long int& numero_de_comparacoes)
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

//funcao que intercala um conjunto de fitas, dado a geracao dos blocos inicialmente ordenados
int intercalar_ordena(FILE** fitas, int quantidade_de_blocos[NUMERO_DE_FITAS], int& numero_de_fitas_utilizadas, long int numero_de_transferencias[2], long int& numero_de_comparacoes)
{
	bool aux_numero_de_fitas = true;//essa variavel ira ajudar a descobrir quantas fitas estao sendo utilizadas
	TQFita* notas = new TQFita[TAMANHO_DA_MEMORIA];//vetor de registros do tamanho da memoria
	int fita_insercao;//fita que sera inserido um bloco
	int confere_pronto = 0;//variavel que ajuda a fazer o controle se a intercalacao resultou em um unico bloco, e aonde esta esse bloco
	confere_pronto = confere_intercalacao_pronta_ordena(quantidade_de_blocos, numero_de_comparacoes);
	bool entrada_saida = true; // ajuda a controlar a variacao entre fitas de entrada e fitas de saida, true e entrada, false e saida
	int tamanho_notas;
	TQFita menor;
	int posic;
	int numero_de_fitas_utilizadas_aux;
	//inicializacao de um separador de blocos, onde nota do separador tem valor -1
	TORegistro separador;
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
					if (fread(&notas[i].nota, sizeof(TORegistro), 1, fitas[i]) != 1)
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
					posic = retirar_o_menor(notas, &menor, tamanho_notas, numero_de_comparacoes);

					numero_de_comparacoes++;
					if (posic != -1)
					{
						numero_de_transferencias[1]++;
						if (fwrite(&menor.nota, sizeof(TORegistro), 1, fitas[fita_insercao]) != 1)
						{
							return 0;
						}

						numero_de_transferencias[0]++;
						//ler o proximo da onde foi retirado
						if (fread(&notas[tamanho_notas].nota, sizeof(TORegistro), 1, fitas[menor.de_qual_fita_veio]) != 1)
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
				if (fwrite(&separador, sizeof(TORegistro), 1, fitas[fita_insercao]) != 1)//escrever separador de blocos
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
					if (fita_insercao == NUMERO_DE_FITAS-1)
					{
						aux_numero_de_fitas = false;
					}
				}
				//se chegar na ultima fita, a proxima fita a ser inserida eh a de posicao NUMERO_DE_FITAS/2
				if (fita_insercao == NUMERO_DE_FITAS-1)
				{
					fita_insercao = NUMERO_DE_FITAS/2;
				}
				else//se nao, eh so avancar uma posicao
				{
					fita_insercao++;
				}
			} while (zerou_entrada(quantidade_de_blocos, 0, numero_de_comparacoes));//enquanto nao acabar os blocos das fitas de entrada

			// apos inserir nas fitas de saida deve-se apagar as fitas de entrada
			apagar(fitas, 0, numero_de_comparacoes);
		
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
					if (fread(&notas[i - NUMERO_DE_FITAS / 2].nota, sizeof(TORegistro), 1, fitas[i]) != 1)
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
					posic = retirar_o_menor(notas, &menor, tamanho_notas, numero_de_comparacoes);

					if (posic != -1)
					{

						if (fwrite(&menor.nota, sizeof(TORegistro), 1, fitas[fita_insercao]) != 1)
						{
							return 0;
						}
						numero_de_transferencias[1]++;
						//ler o proximo da onde foi retirado

						if (fread(&notas[tamanho_notas].nota, sizeof(TORegistro), 1, fitas[menor.de_qual_fita_veio]) != 1)
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
				if (fwrite(&separador, sizeof(TORegistro), 1, fitas[fita_insercao]) != 1)//escrever separador de blocos
				{
					return 0;
				}
				numero_de_transferencias[1]++;

				quantidade_de_blocos[fita_insercao]++;

				//conferir a quantidade de fitas que serao utilizadas na proxima iteracao
				if (fita_insercao <= NUMERO_DE_FITAS/2 -1 && aux_numero_de_fitas)
				{
					numero_de_fitas_utilizadas_aux++;
					if (fita_insercao == NUMERO_DE_FITAS/2 -1)
					{
						aux_numero_de_fitas = false;
					}
				}
				//se chegar na ultima fita, a proxima fita a ser inserida eh a de posicao 0
				if (fita_insercao == NUMERO_DE_FITAS/2 - 1)
				{
					fita_insercao = 0;
				}
				else//se nao, eh so avancar uma posicao
				{
					fita_insercao++;
				}

			} while (zerou_entrada(quantidade_de_blocos, 1, numero_de_comparacoes));//enquanto nao acabar os blocos das fitas de saida

			//o numero de fitas utilizada eh exatamento o numero_de_fita_auxiliares
			//e a proxima intercalacao ocorrera numa fita de saida
			numero_de_fitas_utilizadas = numero_de_fitas_utilizadas_aux;
			entrada_saida = true;

			// apos inserir nas fitas de saida deve-se apagar as fitas de entrada
			apagar(fitas, 1, numero_de_comparacoes);
		}
		//apos terminar um processo de intercalacao deve-se conferir se a intercalacao esta completa
		confere_pronto = confere_intercalacao_pronta_ordena(quantidade_de_blocos, numero_de_comparacoes);
		
	}

	delete[] notas;
	return confere_pronto;

}

//funcao que apaga as fitas de entradas ou as fitas de saida
int apagar(FILE** fitas, int e_s, long int& numero_de_comparacoes)
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
		for (int i = NUMERO_DE_FITAS/2; i < NUMERO_DE_FITAS; i++)
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
