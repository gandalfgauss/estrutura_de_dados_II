#include <iostream>
#include <stdlib.h> 

#include "gera_registros.h"
#include "acesso_seq_indexado.h"
#include "arvore_binaria_em_memoria_externa.h"
#include "arvoreb.h"
#include "arvore_b_estrela.h"

struct Registro1
{
	int chave;
	long int dado1;
	char dado2[500];
}; // estrutura do registro a ser retornado

using namespace std;

int main(int argc, char* argv[])
{
	TRegistro1 registro_retornado;
	long int numero_de_transferencia_para_memoria_interna[2] = { 0,0 };//posicao 0 criacao do metodo, posicao 1 da pesquisa
	long int numero_de_comparacoes_chave_de_pesquisa[2] = { 0,0 };//posicao 0 criacao do metodo, posicao 1 da pesquisa
	long int numero_de_comparacoes_chave_de_pesquisa_real;
	long int numero_de_transferencia_para_memoria_interna_real;
	double tempo_de_execucao_real;
	double tempo_de_execucao[2] = { 0,0 };//posicao 0 criacao do metodo, posicao 1 da pesquisa

	//pesquisa, metodo, quantidade, situacao, chave, imprimir

	//1 - acesso sequencial indexado
	//2- arvore binaria em memoria externa
	//3 - arvore b
	//4 - arvore b estrela

	long int p1, p2, p3, p4;

	FILE* registros;
	//conferir se os parametros foram digitados corretamente
	if (argc < 5 || argc > 6 || atoi(argv[1]) < 1 || atoi(argv[1]) > 4 || (atoi(argv[1]) ==1 && atoi(argv[3]) != 1) ||  atoi(argv[2]) <= 0
		|| atoi(argv[3]) < 1 || atoi(argv[3]) > 3 || atoi(argv[4]) <= 0 )
	{
		cout << "\nDigite corretamente os parametros ! " << endl;
		return 0;
	}

	p1 = atoi(argv[1]);//metodo
	p2 = atoi(argv[2]);//quantidade
	p3 = atoi(argv[3]);//situacao
	p4 = atoi(argv[4]);//chave*/


	/*// EX:
	p1 = 4;//metodo
	p2 = 650000;//quantidade
	p3 = 1;//situacao
	p4 = 400000;//chave
	//argc = 6;*/

	//primeiro gerar os registros
	registros = gera_registros(p2, p3, 1, p4);

	if (argc >= 6)
	{
		g_imprime_registros(registros, 1);
	}

	
	//gerador do meu gosto para teste
	/*registros = gera_meu_gosto();
	cout << "\nDigite a chave a ser buscada: ";
	cin >> p4;*/

	fclose(registros);


	switch (p1)
	{

	case 1:

		if (acesso_sequencial_indexado(registros, p4, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa, tempo_de_execucao, registro_retornado, numero_de_comparacoes_chave_de_pesquisa_real, numero_de_transferencia_para_memoria_interna_real, tempo_de_execucao_real) == 1)
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

		if (arvore_binaria(registros, p4, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa, tempo_de_execucao, registro_retornado, numero_de_comparacoes_chave_de_pesquisa_real, numero_de_transferencia_para_memoria_interna_real, tempo_de_execucao_real) == 1)
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

		if (arvoreb(registros, p4, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa, tempo_de_execucao, registro_retornado, numero_de_comparacoes_chave_de_pesquisa_real, numero_de_transferencia_para_memoria_interna_real, tempo_de_execucao_real) == 1)
		{
			cout << "\nSucesso";
		}

		else
		{
			cout << "\nError";
			return 0;
		}

		break;


	case 4:

		if (arvore_b_estrela(registros, p4, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa, tempo_de_execucao, registro_retornado, numero_de_comparacoes_chave_de_pesquisa_real, numero_de_transferencia_para_memoria_interna_real, tempo_de_execucao_real) == 1)
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

	cout << "\n\t\tCriacao do Metodo\nDados: " << endl;
	cout << "Tempo gasto: " << tempo_de_execucao[0] << " s" << endl;
	cout << "Numero de comparacoes entre chaves de pesquisa: " << numero_de_comparacoes_chave_de_pesquisa[0] << endl;
	cout << "Numero de transferencias de registros da memoria externa para a interna: " << numero_de_transferencia_para_memoria_interna[0];

	cout << "\n\n\t\tPesquisa\nDados: " << endl;
	cout << "Tempo gasto: " << tempo_de_execucao_real << " s" << endl;
	cout << "Numero de comparacoes entre chaves de pesquisa: " << numero_de_comparacoes_chave_de_pesquisa_real << endl;
	cout << "\nNumero de transferencias de registros da memoria externa para a interna: " << numero_de_transferencia_para_memoria_interna_real;

	cout << "\n\n\t\tPesquisa Media\nDados: " << endl;
	cout << "Tempo gasto: " << tempo_de_execucao[1]/10 << " s" << endl;
	cout << "Numero de comparacoes entre chaves de pesquisa: " << numero_de_comparacoes_chave_de_pesquisa[1]/10 << endl;
	cout << "\nNumero de transferencias de registros da memoria externa para a interna: " << numero_de_transferencia_para_memoria_interna[1]/10;
	cout << "\n\n\t\tTotal\nDados: " << endl;
	cout << "\nTempo total: " << tempo_de_execucao[0] + tempo_de_execucao[1]/10 << " s" << endl;
	cout << "Numero de comparacoes entre chaves de pesquisa: " << numero_de_comparacoes_chave_de_pesquisa[1]/10 + numero_de_comparacoes_chave_de_pesquisa[0] << endl;
	cout << "Numero de transferencias de registros da memoria externa para a interna: " << numero_de_transferencia_para_memoria_interna[1]/10 + numero_de_transferencia_para_memoria_interna[0] << endl;

	return 0;
}