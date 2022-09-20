#include <iostream>
#include <chrono>
#include <stdlib.h>
#include <ctime>
#include "gera_registros.h"
#include "acesso_seq_indexado.h"

using namespace std::chrono;
using namespace std;

#define ITENS_POR_PAGINA 4

struct Registro1
{
    int chave;
    long int dado1;
    char dado2[500];
}; // estrutura do registro a ser retornado

struct Registro
{
    int chave;
    long int dado1;
    char dado2[500];
}; // estrutura a ser inserida no arquivo binario


int acesso_sequencial_indexado(FILE* binario, int chave, long int numero_de_transferencia_para_memoria_interna[2],
    long int numero_de_comparacoes_chave_de_pesquisa[2], double tempo_de_execucao[2], TRegistro1& registro_retornado, long int& numero_de_comparacoes_chave_de_pesquisa_real,long int& numero_de_transferencia_para_memoria_interna_real, double& tempo_de_execucao_real)//inicia o programa dado que já tenha criado um arquivo binario de resgistros, onde os registros estao ordenados pela chave
{
    int itens_por_pagina;//quantidade de itens por pagina
    int tabela[MAX_TABELA];//tabela de indices usada para pesquisar no arquivo
    int tamanho_da_tabela = 0;//tamanho real da tabela(não é o tamanho maximo)
    TRegistro registro;//registro encontrado dado uma chave
    int resultado_da_pesquisa;//resultado da pesquisa pela chave
    FILE* pesq;



    if (fopen_s(&binario, "registros.bin", "rb") != 0)
    {
        //cout << "\nFalha ao criar o arquivo binario para leitura de registros" << endl;
        return 0;
    }//abre arquivo binario para leitura em registros.bin

    //tempo de criacao da tabela
    duration<double> time_span;
    duration<double> time_span2;

    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    //1° etapa: Criar tabela de índices
    if (!cria_tabela_de_indices(binario, tabela, &itens_por_pagina, &tamanho_da_tabela, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa))//chama a funcao que cria a tabela de itens que na verdade é um vetor
    {
        fclose(binario);
        return 0;
    }

    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);
    
    //imprime a tabela de indices
    /*cout << "\n\tTabela de indices" << endl;
    for (int i = 0; i < tamanho_da_tabela; i++)//imprime minha tabela de itens
    {
        cout << tabela[i] << " ";
    }
    cout << endl << endl;*/

    //2° etapa pesquisar no arquivo


    t1 = high_resolution_clock::now();//marcar tempo da pesquisa

    //a variável inteira resultado_da_pesquisa contem o resultado da busca pela chave
    resultado_da_pesquisa = pesquisa_chave(binario, tabela, itens_por_pagina, tamanho_da_tabela, &registro, chave, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa);

    t2 = high_resolution_clock::now();
    time_span2 = duration_cast<duration<double>>(t2 - t1);

    if (resultado_da_pesquisa == 0)//se o resultado for 0 a chave nem chegou a ser buscada teve um problema antes
    {
        //cout << "\nErro na pesquisa pela chave !" << endl;
        fclose(binario);

        return 0;
    }
    else if(resultado_da_pesquisa == 1)
    {
        cout << "\n\nRegistro nao encontrado\n";
    }

    //se for 1 é porque não está no arquivo binário de registros, a funcao pesquisa chave imprime uma mensagem de error

    else if (resultado_da_pesquisa == 2)//se for 2 é porque a chave foi encontrada e será impresso na tela o registro encontrado
    {
      
        cout << "\nRegistro encontrado !\nRegistro\nChave: " << registro.chave << "\nDado1: " << registro.dado1 << "\nDado2: " << registro.dado2[0] << endl;
        registro_retornado.chave = registro.chave;
        registro_retornado.dado1 = registro.dado1;
        registro_retornado.dado2[0] = registro.dado2[0];
    }
    cout << endl;

    tempo_de_execucao[0] = time_span.count();
    tempo_de_execucao[1] = time_span2.count();

    numero_de_comparacoes_chave_de_pesquisa_real = numero_de_comparacoes_chave_de_pesquisa[1];
    numero_de_transferencia_para_memoria_interna_real = numero_de_transferencia_para_memoria_interna[1];
    tempo_de_execucao_real = tempo_de_execucao[1];

    //realizar mais 9 pesquisas e fazer media

    if (fopen_s(&pesq, "registros.bin", "rb") != 0)
    {
        //cout << "\nFalha ao criar o arquivo binario para leitura de registros" << endl;
        return 0;
    }//abre arquivo binario para leitura em registros.bin


    for(int i = 1; i <= 9; i++)
    {
        fread(&registro, sizeof(TRegistro), 1, pesq);
        t1 = high_resolution_clock::now();//marcar tempo da pesquisa

        pesquisa_chave(binario, tabela, itens_por_pagina, tamanho_da_tabela, &registro, registro.chave, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa);
        t2 = high_resolution_clock::now();
        time_span2 = duration_cast<duration<double>>(t2 - t1);
        tempo_de_execucao[1] += time_span2.count();
    }

    fclose(binario);
    fclose(pesq);

    cout << "\nAcesso Sequencial Indexado concluido !" << endl;   

    return 1;//o programa é encerrado com sucesso
}

int asi_arquivo_vazio(FILE* binario)//funcao que verifica se um arquivo está vazio
{
    int inicio_do_arquivo, final_do_arquivo;


    if (fseek(binario, 0, 0) != 0)
    {
        //cout << "\nFalha em conferir se o arquivo está vazio.\nErro ao movimentar para inicio do arquivo" << endl;
        return 1;
    }

    inicio_do_arquivo = ftell(binario);//caso não dê erro ao movimentar para o inico do arquivo a variavel inicio_do_arquivo obtem o numero de bytes até na posicao do arquivo ou seja 0

    if (fseek(binario, 0, 2) != 0)
    {
        //cout << "\nFalha em conferir se o arquivo está vazio.\nErro ao movimentar para final do arquivo" << endl;
        return 1;
    }

    final_do_arquivo = ftell(binario);//caso não dê erro ao movimentar para o final do arquivo a variável final_do_arquivo armazena o numero de bytes do arquivo

    if (inicio_do_arquivo == final_do_arquivo)//se a quantidade de bytes ate no final do arquivo for igual a do inicio o arquivo esta vazio
    {
        //cout << "\nArquivo Vazio" << endl;
        return 1;
    }


    return 0;
}

int cria_tabela_de_indices(FILE* binario, int tabela[MAX_TABELA], int* itens_por_pagina, int* tamanho_da_tabela, long int numero_de_transferencia_para_memoria_interna[2], long int numero_de_comparacoes_chave_de_pesquisa[2])//funcao que cria a tabela de indices
{
    TRegistro registro;

    if (asi_arquivo_vazio(binario))//se o arquivo estiver vazio nao podemos criar a tabela de indices
    {
        //cout << "\nFalha em criar a tabela de indices.";
        return 0;
    }

    *itens_por_pagina = ITENS_POR_PAGINA;

    if (*itens_por_pagina <= 0)//verifcando se a quantidade itens por pagina é positiva e diferente de 0
    {
        //cout << "\nDigite corretamente a quantidade de itens por pagina: ";
        return 0;
    }

    if (fseek(binario, 0, 0) != 0)//movimenta-se para o inicio do arquivo
    {
        //cout << "\nFalha em criar a tabela de indices.\nErro ao movimentar para inicio do arquivo" << endl;
        return 0;
    }

    while (fread(&registro, sizeof(TRegistro), 1, binario) == 1)//leio o primeiro registro e vou lendo de acordo com o fseek
    {
        numero_de_transferencia_para_memoria_interna[0]++;

        *tamanho_da_tabela = *tamanho_da_tabela + 1;

        if (*tamanho_da_tabela > MAX_TABELA)
        {
            //cout << "\nError ao criar a tabela ! Favor aumentar o Máximo da tabela de indices" << endl;

            return 0;
        }

        tabela[*tamanho_da_tabela - 1] = registro.chave;

        if (fseek(binario, sizeof(TRegistro) * (*itens_por_pagina - 1), 1) != 0)//move o ponteiro para quantidade de itens numa pagina -1
        {
            //cout << "\nFalha em criar a tabela de indices.\nErro ao movimentar dentro do arquivo" << endl;
            return 0;
        }
    }


    return 1;//tabela criada com sucesso
}

int pesquisa_binaria(TRegistro* registros, int tamanho_da_pagina, TRegistro* registro, int chave, long int numero_de_transferencia_para_memoria_interna[2], long int numero_de_comparacoes_chave_de_pesquisa[2])//funcao que dado um vetor de registro busca um registro pela chave utilizando pesquisa binária
{
    int inicio = 0;
    int final = tamanho_da_pagina - 1;

    int meio;

    while (inicio <= final)
    {
        meio = (inicio + final) / 2;

        numero_de_comparacoes_chave_de_pesquisa[1]++;

        if (registros[meio].chave == chave)//registro foi encontrado
        {
            *registro = registros[meio];//salva o registro entrado
            return 1;//retorna um para indicar que encontrou
        }

        else if (registros[meio].chave < chave)
        {
            numero_de_comparacoes_chave_de_pesquisa[1]++;
            inicio = meio + 1;

        }
        else
        {
            numero_de_comparacoes_chave_de_pesquisa[1]++;
            final = meio - 1;
        }
    }

    return 0;//retorna 0 se o elemento nao esta na pagina
}

int pesquisa_chave(FILE* binario, int tabela[MAX_TABELA], int itens_por_pagina, int tamanho_da_tabela, TRegistro* registro, int chave, long int numero_de_transferencia_para_memoria_interna[2], long int numero_de_comparacoes_chave_de_pesquisa[2])//funcao que pesquisa um registro em um arquivo dado uma tabela de paginas
{
    int tamanho_da_pagina;
    TRegistro* registros_da_pagina = new TRegistro[itens_por_pagina];
    int pagina_a_se_buscar;
    int tamanho;


    //conferir se chave n?o ? menor que primeiro elemento da pagina
    numero_de_comparacoes_chave_de_pesquisa[1]++;
    if (chave < tabela[0])
    {

        //cout << "\nRegistro de chave " << chave << " n?o est? no arquivo" << endl;
        return 1;
    }

    //qual pagina est? o item
    pagina_a_se_buscar = 0;
    while (pagina_a_se_buscar < tamanho_da_tabela - 1)
    {
        numero_de_comparacoes_chave_de_pesquisa[1]++;
        if (tabela[pagina_a_se_buscar + 1] > chave)
        {

            break;
        }

        pagina_a_se_buscar++;
    }

    //se for a ultima pagina temos que descobrir qual e a quantidade de itens
    tamanho_da_pagina = ITENS_POR_PAGINA;
    if (pagina_a_se_buscar == tamanho_da_tabela - 1)
    {
        if (fseek(binario, 0, 2) != 0)//deslocar pro final
        {
            //cout << "\nFalha em buscar a chave.\nErro ao movimentar para o final do arquivo" << endl;
            return 0;
        }

        tamanho = (ftell(binario) / sizeof(TRegistro)) % ITENS_POR_PAGINA;

        if (tamanho != 0)
        {
            tamanho_da_pagina = tamanho;
        }

    }

    //deslocar para pagina a ser lida

    if (fseek(binario, 0, 0) != 0)//deslocar pro come?o
    {
        //cout << "\nFalha em buscar a chave.\nErro ao movimentar para inicio do arquivo" << endl;
        return 0;
    }

    if (fseek(binario, sizeof(TRegistro) * itens_por_pagina * pagina_a_se_buscar, 0) != 0)//desloca para pagina a ser lida
    {
        //cout << "\nFalha em buscar a chave.\nErro ao movimentar para a p?gina da chave" << endl;
        return 0;
    }

    //jogar os resgistros da pagina para a mem?ria principal e armazera o tamanho da pagina
    numero_de_transferencia_para_memoria_interna[1]++;
    if (fread(registros_da_pagina, sizeof(TRegistro), tamanho_da_pagina, binario) != tamanho_da_pagina)
    {
        //cout << "\nFalha em buscar a chave.\nErro ao jogar pagina para memoria" << endl;
    }

    if (!pesquisa_binaria(registros_da_pagina, tamanho_da_pagina, registro, chave, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa))//realizar a pesquisa binaria na pagina onde um determinado registro pode se encontrar
    {
        //cout << "\nRegistro de chave " << chave << " n?o encontrado" << endl;
        return 1;
    }

    return 2;//retorna 2 se achou o elemento
}