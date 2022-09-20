#include <iostream>
#include <stdio.h>
#include "gera_registros.h"
#include "quick_sort.h"
#pragma warning(disable : 4996)

struct GRegistro
{
    char inscricao[9];
    double nota;
    char estado[3];
    char cidade[51];
    char curso[31];
};

using namespace std;

//recebe a quantidade de registro e o tipo de ornacao a ser gerado
int gera_registros(int tamanho, int tipo_de_ordenacao) 
{
    FILE* binario;


    if ((binario = fopen( "registros.bin", "w+b")) == NULL)
    {
        //cout << "\nFalha ao criar o arquivo binario registros.bin para inserir registros" << endl;
        return 0;
    }//cria arquivo binario o qual tera os dados a serem ordenados

    if (gerar(tipo_de_ordenacao, tamanho, binario) == 0)//chama a funcao que gera os registros de acordo com o tipo
    {
        return 0;
    }

    fclose(binario);//fecha registros.bin

    return 1;
}


int gerar(int tipo_de_ordenacao, int tamanho, FILE* binario)//funcao que gera registros
{
    TGRegistro aux;
    FILE* provao;
    char enter;
    FILE* registros_ordenados;
    long int numero_de_transferencias[2], numero_de_comparacoes;
    double tempo_de_execucao;
    FILE* auxiliar;
    int posic;

    //ler do Provao e inserir  arquivo registros.bin
    if ((provao = fopen("PROVAO.txt", "r")) == NULL)
    {
        //cout << "\nFalha ao abrir o PROVAO.txt" << endl;
        return 0;
    }

    for (int i = 0; i < tamanho; i++)
    {
        if (fscanf(provao, "%8[0-9] %lf %2[A-Z a-z] %50[A-Z a-z ' -] %30[A-Z a-z] %[\n] ", aux.inscricao, &aux.nota, aux.estado, aux.cidade, aux.curso, &enter) == 0)
        {
            //cout << "Error ao ler dados do arquivo PROVAO.TXT" << endl;
            return 0;
        }

        if (fwrite(&aux, sizeof(TGRegistro), 1, binario) != 1)
        {
            //cout << "\nFalha ao escrever no arquivo registros.bin" << endl;
            return 0;
        }
    }

    fclose(provao);


    switch (tipo_de_ordenacao)
    {

    case 1://registro crescente

        //chamar quick sort externo
        fclose(binario);
        //ordenar registros.bin 
        if (!quick_inicia(2, numero_de_transferencias, numero_de_comparacoes, tempo_de_execucao, tamanho))
        {
            return 0;
        }


        if ((binario = fopen("registros.bin", "r+b")) == NULL)
        {
            //cout << "\nFalha ao abrir registros.bin" << endl;
            return 0;
        }//abre novamente registros.bin   

        break;

    case 2://registro decrescente

        //chamar quick sort externo e inverter arquivo
        fclose(binario);

        //ordenar registros.bin
        if (!quick_inicia(2, numero_de_transferencias, numero_de_comparacoes, tempo_de_execucao, tamanho))
        {
            return 0;
        }

        if ((binario = fopen("registros.bin", "r+b")) == NULL)
        {
            //cout << "\nFalha ao abrir arquivo registros.bin" << endl;
            return 0;
        }//cria arquivo binario o qual tera os dados a serem ordenados

        if ((auxiliar = fopen("registrosauxiliar.bin", "w+b")) == NULL)
        {
            //cout << "\nFalha ao criar o arquivo binario registrosauxiliar.bin" << endl;
            return 0;
        }//cria arquivo binario o qual tera os dados a serem ordenados

        posic = tamanho - 1;//posicao que vou ler do arquivo registro.bin ou seja, de tras para frente

        do
        {
            if (fseek(binario, sizeof(TGRegistro)*posic, 0) != 0)//ir para a posicao do registro.bin
            {
                return 0;
            }
            
            if (fread(&aux, sizeof(TGRegistro), 1, binario) != 1)//ler do registro.bin
            {
                return 0;
            }

            if (fwrite(&aux, sizeof(TGRegistro), 1, auxiliar) != 1)//escrever no registrosauxiliar.bin
            {
                return 0;
            }

            posic--;
        } while (posic >= 0);

        if (fseek(auxiliar, 0, 0) != 0)//do comeco do arquivo registrosauxiliar.bin
        {
            return 0;
        }

        if (fseek(binario, 0, 0) != 0)//do comeco do arquivo registros.bin
        {
            return 0;
        }

        for (int i = 0; i < tamanho; i++)
        {
            if (fread(&aux, sizeof(TGRegistro), 1, auxiliar) != 1)//ler do registrosauxiliar.bin
            {
                return 0;
            }

            if (fwrite(&aux, sizeof(TGRegistro), 1, binario) != 1)//escrever no registros.bin
            {
                return 0;
            }

        }
           
        fclose(auxiliar);

        break;

    case 3://registro aleatorio

        //faz nada ja esta proto
        break;
    }

    //passar para o arquivo binario onde vai ocorrer insercao
    //o arquivo onde deve ser ordenado e registrosOrdenados.bin
    if ((registros_ordenados = fopen("registrosOrdenados.bin", "wb")) == NULL)
    {
        //cout << "\nFalha ao gerar registrosOrdenado.bin" << endl;
        return 0;
    }
    
    if (fseek(binario, 0, 0) != 0)//do comeco do arquivo registros.bin
    {
        return 0;
    }

    for (int i = 0; i < tamanho; i++)
    {
        if (fread(&aux, sizeof(TGRegistro), 1, binario) != 1)// le do registros.bin
        {
            //cout << "\nFalha ao escrever no arquivo binario" << endl;
            return 0;
        }

        if (fwrite(&aux, sizeof(TGRegistro), 1, registros_ordenados) != 1)//escrever em registrosOrdenados.bin
        {
            //cout << "\nFalha ao escrever no arquivo binario" << endl;
            return 0;
        }

    }

    fclose(registros_ordenados);//fecha registrosOrdenado.bin
    return 1;//retorna 1 se deu tudo certo e 0 se ocorreu algo de errado

}


int g_imprime_registros()
{
    TGRegistro aux;
    int quantidade = 1;
    FILE* binario;

    if ((binario = fopen("registros.bin", "rb")) == NULL)
    {
        //cout << "\nFalha ao ler registros.bin em IMPRIME" << endl;
        return 0;
    }//abrir registros.bin

    //imprimir dados antes de ordenar
    cout << "\n\nAntes de ordenar :";
    while (fread(&aux, sizeof(TGRegistro), 1, binario) == 1)//le o primeiro registro de registros.bin ate o ultimo e sai imprimindo
    {
        cout << "\n\nAluno " << quantidade << ": " << endl;
        cout << "\nInscricao : " << aux.inscricao << endl;
        cout << "\nNota : " << aux.nota << endl;
        cout << "\nEstado: " << aux.estado << endl;
        cout << "\nCidade : " << aux.cidade << endl;
        cout << "\nCurso : " << aux.curso << endl;

        quantidade++;
    }
    fclose(binario);// fechar registros.bin

    if ((binario = fopen("registrosOrdenados.bin", "rb")) == NULL)
    {
        //cout << "\nFalha ao ler registros em IMPRIME" << endl;
        return 0;
    }//abrir arquivo registrosOrdenados.bin para leitura

    quantidade = 1;
    //imprimir dados antes de ordenar
    cout << "\n\nDepois de ordenar :";
    while (fread(&aux, sizeof(TGRegistro), 1, binario) == 1)////le o primeiro registro de registrosOrdenados.bin ate o ultimo e sai imprimindo
    {
        cout << "\n\nAluno " << quantidade << ": " << endl;
        cout << "\nInscricao : " << aux.inscricao << endl;
        cout << "\nNota : " << aux.nota << endl;
        cout << "\nEstado: " << aux.estado << endl;
        cout << "\nCidade : " << aux.cidade << endl;
        cout << "\nCurso : " << aux.curso << endl;

        quantidade++;
    }
    fclose(binario);//fechar registrosOrdenados.bin
    return 1;
}

int imprimir_no_txt()//funcao que coloca os registros ordenados em um arquivo txt chamado registrosordenados.txt
{
    TGRegistro aux;
    FILE* binario;
    FILE* ordenado;
    char enter = '\n';

    if ((binario = fopen("registrosOrdenados.bin", "rb")) == NULL)
    {
        //cout << "\nFalha ao ler registros em IMPRIME NO TXT" << endl;
        return 0;
    }//abre arquivo binario ordenado (registrosOrdenados.bin)

    if ((ordenado = fopen("registrosordenados.txt", "w")) == NULL)
    {
        //cout << "\nFalha ao ler registros em IMPRIME" << endl;
        return 0;
    }//criar um arquivo texto 

    while (fread(&aux, sizeof(TGRegistro), 1, binario) == 1)//ler todos os registros de registro.bin
    {
        //escrever em registroordenados.txt
        fprintf(ordenado, "%s %.1lf %s %50s %30s\n", aux.inscricao, aux.nota, aux.estado, aux.cidade, aux.curso);
    }

    return 1;
}

