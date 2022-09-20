#include <iostream>
#include <stdlib.h> 

#include "gera_registros.h"
#include "quick_sort.h"
#include "intercalacao_2Fitas_ordenacao.h"
#include "intercalacao_2Fitas_heap.h"

using namespace std;

int main(int argc, char* argv[])
{

	long int numero_de_transferencias[2]=  { 0,0 };//posicao 0 leitura, 1 escrita
	long int numero_de_comparacoes = 0;// numero de comparacoes
	double tempo_de_execucao = 0.0;//

	//pesquisa, metodo, quantidade, situacao, imprimir

	//1 - Intercalação balanceada de vários caminhos (2f fitas) utilizando um método de ordenação interna na criação dos blocos ordenados
	//2- Intercalação balanceada de vários caminhos (2f fitas) utilizando a técnica de seleção por substituição na geração dos blocos ordenados, 
	//3 - QuickSort Externo

	long int p1, p2, p3;

	//conferir se os parametros foram digitados corretamente
	if (argc < 4 || argc > 5 || atoi(argv[1]) < 1 || atoi(argv[1]) > 3 || atoi(argv[2]) <= 0
		|| atoi(argv[3]) < 1 || atoi(argv[3]) > 3 )
	{
		cout << "\nDigite corretamente os parametros ! " << endl;
		return 0;
	}

	p1 = atoi(argv[1]);//metodo
	p2 = atoi(argv[2]);//quantidade
	p3 = atoi(argv[3]);//situacao


	// EX:
	/*p1 = 2;//metodo
	p2 = 471705;//quantidade
	p3 = 1;//situacao
	//argc = 6;*/

	//primeiro gerar os registros
	if (gera_registros(p2, p3) == 0)
	{
		cout <<"\nNao foi possivel gerar os registros !";
		return 0;
	}

	switch (p1)
	{

	case 1:

		
		if (intercacao_ordenacao_inicia(numero_de_transferencias,numero_de_comparacoes, tempo_de_execucao))
		{
			cout << "\nSucesso";
		}

		else
		{
			cout << "\nError";
			return 0;
		}


		break;

		
	case 2:

		if (intercacao_heap_inicia(numero_de_transferencias, numero_de_comparacoes, tempo_de_execucao))
		{
			cout << "\nSucesso";
		}

		else
		{
			cout << "\nError";
			return 0;
		}

		break;

	case 3:

		if (quick_inicia(1, numero_de_transferencias, numero_de_comparacoes, tempo_de_execucao, p2))
		{
			cout << "\nSucesso";
		}

		else
		{
			cout << "\nError";
			return 0;
		}

		break;
	}

	imprimir_no_txt();//colocar os registros ordenado em registrosordenados.txt

	if (argc >= 5)//imprime registros antes de serem ordenados e depois
	{
		g_imprime_registros();
	}

	cout << "\nDados: " << endl;
	cout << "Tempo gasto: " << tempo_de_execucao << " s" << endl;
	cout << "Total de comparacoes: " << numero_de_comparacoes << endl;
	cout << "Total de transferencias de registros da memoria externa para a interna (leitura): " << numero_de_transferencias[0]<< endl;
	cout << "Numero de transferencias de registros da memoria interna para a externa (escrita): " << numero_de_transferencias[1] << endl;
	
	return 0;
}