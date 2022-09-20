#include <iostream>
#include <stdlib.h>
#include <ctime>
#include "gera_registros.h"

using namespace std;

struct Registro1
{
    int chave;
    long int dado1;
    char dado2[500];
}; // estrutura a ser inserida no arquivo binario

int gerar(int tipo_de_ordenacao, TRegistro1* registro1, int tamanho, FILE* binario, int chave)//funcao que gera registros do tipo 1
{
    srand(time(NULL));

    int* embaralha_vetor = new int[tamanho];
    int posic, temporario;
    bool inseriu = false;

    switch (tipo_de_ordenacao)
    {

        case 1://registro crescente

            for (int reg = 1; reg <= tamanho; reg++)//inserir registro de tamanho n
            {
                if (!inseriu && reg == tamanho)
                {
                    registro1->chave = chave;
                    registro1->dado1 = chave;
                    registro1->dado2[0] = (char)chave;
                    registro1->dado2[1] = '\0';
                }

                else if (reg >= chave && !inseriu)
                {
                    registro1->chave = chave;
                    registro1->dado1 = chave;
                    registro1->dado2[0] = (char)chave;
                    registro1->dado2[1] = '\0';
                    inseriu = true;
                }
                else
                {
                    registro1->chave = reg;
                    registro1->dado1 = reg;
                    registro1->dado2[0] = (char)reg;
                    registro1->dado2[1] = '\0';
                }

                if (fwrite(registro1, sizeof(TRegistro1), 1, binario) != 1)
                {
                    cout << "\nFalha ao inserir registro em registro.bin" << endl;
                    return 0;
                }
            }


        break;

        case 2://registro decrescente

            for (int reg = tamanho; reg >= 1; reg--)//inserir registro de tamanho n
            {

                if (!inseriu && reg == 1)
                {
                    registro1->chave = chave;
                    registro1->dado1 = chave;
                    registro1->dado2[0] = (char)chave;
                    registro1->dado2[1] = '\0';
                }

                else if (reg  <= chave && !inseriu)
                {
                    registro1->chave = chave;
                    registro1->dado1 = chave;
                    registro1->dado2[0] = (char)chave;
                    registro1->dado2[1] = '\0';
                    inseriu = true;
                }
                else
                {
                    registro1->chave = reg;
                    registro1->dado1 = reg;
                    registro1->dado2[0] = (char)reg;
                    registro1->dado2[1] = '\0';
                }

                if (fwrite(registro1, sizeof(TRegistro1), 1, binario) != 1)
                {
                    cout << "\nFalha ao inserir registro em registro.bin" << endl;
                    return 0;
                }

               
            }

            break;

        case 3://registro aleatorio


            //gera vetor
            for (int reg = 1; reg <= tamanho; reg++)//inserir registro de tamanho n
            {
                if (!inseriu && reg == tamanho)
                {
                    embaralha_vetor[reg - 1] = chave;
                }

                else if (reg >= chave && !inseriu)
                {
                    embaralha_vetor[reg - 1] = chave;
                    inseriu = true;
                }
                else
                {
                    embaralha_vetor[reg - 1] = reg;
                }

            }

            //embaralhar vetor
           
            for (int i = 0; i < tamanho; i++)
            {
                posic = rand() % tamanho;

                temporario = embaralha_vetor[i];
                embaralha_vetor[i] = embaralha_vetor[posic];
                embaralha_vetor[posic] = temporario;
            }

            for (int reg = 0; reg < tamanho; reg++)//inserir registro de tamanho n
            {
                registro1->chave = embaralha_vetor[reg];//registro sem repetir
                registro1->dado1 = embaralha_vetor[reg];
                registro1->dado2[0] = (char) embaralha_vetor[reg];
                registro1->dado2[1] = '\0';

                if (fwrite(registro1, sizeof(TRegistro1), 1, binario) != 1)
                {
                    cout << "\nFalha ao inserir registro em registro.bin" << endl;
                    return 0;
                }

                
            }
            break;
        

        default:
            cout << "\nDigite o tipo certo de ordenacao !" << endl;
            return 0;
            break;
    }

    if (embaralha_vetor != NULL)
        delete [] embaralha_vetor;


    return 1;

}
FILE* gera_registros(int tamanho, int tipo_de_ordenacao, int tipo_de_registro, int chave)
{
    srand(time(NULL));


    FILE* binario;
    TRegistro1 registro1;

    if (fopen_s(&binario, "registros.bin", "wb+") != 0)
    {
        cout << "\nFalha ao criar o arquivo binario para inserir registros" << endl;
        return NULL;
    }//cria arquivo binario para inserir em registros.bin

    if (gerar(tipo_de_ordenacao, &registro1, tamanho, binario, chave) == 0)
    {
        return NULL;
    }

    return binario;
}


void g_imprime_registros(FILE* binario, int tipo)
{
    TRegistro1 registro1;
    int quantidade = 1;

    if (fseek(binario, 0, 0) != 0)
    {
        cout << "\nErro ao movimentar para inicio do arquivo arvore.bin" << endl;
        return;
    }

    while (fread(&registro1, sizeof(TRegistro1), 1, binario) == 1)//leio o primeiro registro e vou lendo de acordo com o fseek
    {
        cout << "\nChave do Registro " << quantidade << " : " << registro1.chave << endl;

        quantidade++;
    }
}

FILE* gera_meu_gosto()
{

    FILE* binario;
    TRegistro1 registro1;
    int saida ;

    if (fopen_s(&binario, "registros.bin", "wb+") != 0)
    {
        cout << "\nFalha ao criar o arquivo binario para inserir registros" << endl;
        return NULL;
    }//cria arquivo binario para inserir em registros.bin

    do//inserir registro de tamanho n
    {
        cout << "\n\nDigite a chave do registro: ";
        cin >> saida;

        if (saida == -1)
        {
            break;
        }

        registro1.chave = saida;//registro crescente
        registro1.dado1 = saida;
        registro1.dado2[0] = '1';

        if (fwrite(&registro1, sizeof(TRegistro1), 1, binario) != 1)
        {
            cout << "\nFalha ao inserir registro em registro.bin" << endl;
            return 0;
        }

    } while (true);

    return binario;
}