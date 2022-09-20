#include <iostream>
#include <stdio.h>
#include <chrono>
#include "quick_sort.h"

using namespace std;
using namespace std::chrono;

#define TAREA  3
#define MINIMO -1
#define MAXIMO 500


#pragma warning(disable : 4996)

//estrutura dos Registros do arquivo binario e texto
struct QRegistro
{
    char inscricao[9];
    double nota;
    char estado[3];
    char cidade[51];
    char curso[31];
};

//funcao que faz os preparativos do algoritmo e chama o quicksort
int quick_inicia(int tipo_arquivo, long int numero_de_transferencias[2], long int& numero_de_comparacoes, double& tempo_de_execucao, int tamanho_do_arquivo)
{
	FILE* ArqLi;
    FILE* ArqEi;
    FILE* ArqLEs;

	//primeiro deve-se abrir o arquivo a ser ordenado

    //se o parametro tipo_arquivo for 1, deve-se ordenar o arquivo registroOrdenados.bin
    if (tipo_arquivo == 1)//ordenar normal
    {
        if ((ArqLi = fopen("registrosOrdenados.bin", "r + b")) == NULL)
        {
            //cout << "\nFalha ao abir arquivo de registros inicial" << endl;
            return 0;
        }//ponteiro de Li

        if ((ArqEi = fopen("registrosOrdenados.bin", "r+b")) == NULL)
        {
            //cout << "\nFalha ao abir arquivo de registros inicial" << endl;
            return 0;
        }//ponteiro de Ei

        if ((ArqLEs = fopen("registrosOrdenados.bin", "r+b")) == NULL)
        {
            //cout << "\nFalha ao abir arquivo de registros inicial" << endl;
            return 0;
        }//ponteiro Ls e Es juntos

    }
    //caso contrário deve ser ordenado o arquivo registros.bin, entra nesse else quando o gerador de registros precisa de gerar um arquivo ordenado para executar os testes
    else
    {
        if ((ArqLi = fopen("registros.bin", "r + b")) == NULL)
        {
            //cout << "\nFalha ao abir arquivo de registros inicial" << endl;
            return 0;
        }//direcionar o ponteiro de Li

        if ((ArqEi = fopen("registros.bin", "r+b")) == NULL)
        {
            //cout << "\nFalha ao abir arquivo de registros inicial" << endl;
            return 0;
        }//direcionar o ponteiro de Ei

        if ((ArqLEs = fopen("registros.bin", "r+b")) == NULL)
        {
            //cout << "\nFalha ao abir arquivo de registros inicial" << endl;
            return 0;
        }//direcionar os ponteiros Ls e Es juntos
    }

    //marcar o tempo de execucaoo
    duration<double> time_span;

    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    //chamar o quicksort
    if (!QuicksortExterno(&ArqLi, &ArqEi, &ArqLEs, 1, tamanho_do_arquivo, numero_de_transferencias, numero_de_comparacoes))
    {
        return 0;
    }

    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);

    tempo_de_execucao = time_span.count();

    //fechar arquivos
    fclose(ArqEi);
    fclose(ArqLi);
    fclose(ArqLEs);
    return 1;

}

//funcao que retira o menor elemento da area e a reconstitui
void RetiraMin(TQRegistro* area, TQRegistro* R, int* tamanho_da_area)
{
    *R = area[0]; // como a area e ordenada, entao o primeiro elemento e o menor

    for (int i = 0; i < *tamanho_da_area -1; i++)//reconstitui a area
    {
        area[i] = area[i + 1];
    }

    *tamanho_da_area -= 1;// como um elemento foi retirado entao o tamanho da area diminui
}

//funcao que retiro o maior elemento da area
void RetiraMax(TQRegistro* area, TQRegistro* R, int* tamanho_da_area)
{
    *R = area[*tamanho_da_area -1]; // como a area e ordenada, entao o ultimo elemento e o menor

    *tamanho_da_area -= 1;// como um elemento foi retirado entao o tamanho da area diminui
}

//funcao que escreve um registro no subarquivo inferior
int EscreveMin(FILE** ArqEi, TQRegistro R, int* Ei, long int numero_de_transferencias[2])
{
    if (fwrite(&R, sizeof(TQRegistro), 1, *ArqEi) != 1)
    {
        return 0;
    }
    numero_de_transferencias[1]++;

    *Ei += 1;

    
    return 1;
}

//funcao que escreve um registro no subarquivo superior
int EscreveMax(FILE** ArqLEs, TQRegistro R, int* Es, long int numero_de_transferencias[2])
{
    if (fseek(*ArqLEs, (*Es - 1) * sizeof(TQRegistro), 0) != 0)
    {
        return 0;
    }

    if (fwrite(&R, sizeof(TQRegistro), 1, *ArqLEs) != 1)
    {
        return 0;
    }
    numero_de_transferencias[1]++;

    *Es -= 1;

    return 1;
}

//funcao que insere um registro na area
void InserirArea(TQRegistro* area, TQRegistro* R, int* tamanho_da_area, long int& numero_de_comparacoes)
{
    
    TQRegistro* aux = new TQRegistro[*tamanho_da_area];
    
    //gero uma copia da area para aux
    for (int i = 0; i < *tamanho_da_area; i++)
    {
        aux[i] = area[i];
    }
  
    int posic;
    //inserir ordenado
    for (posic = 0; posic < *tamanho_da_area; posic++)
    {
        numero_de_comparacoes++;
        if (area[posic].nota > R->nota) // a area e percorrida enquanto nao achar uma nota maior do que a que deve ser inserida
        {
            break;//sai do loop quando achar a posicao de insercao
        }
    }

    //apos achar a posicao deve-se passar os registros para frente da posicao
    for (int i = posic; i < *tamanho_da_area; i++)
    {
        area[i + 1] = aux[i];
    }

    area[posic] = *R; // insere o registro na area na posicao certa

    *tamanho_da_area += 1;//como foi feita uma insercao, aumenta o tamanho da area

    delete[] aux;
}

//funcao que le um registro na parte inferior do arquivo
int LeInf(FILE** ArqLi, TQRegistro* R, int* Li, bool* OndeLer, long int numero_de_transferencias[2])
{
    if (fread(R, sizeof(TQRegistro), 1, *ArqLi) != 1)
    {
        return 0;
    }
    numero_de_transferencias[0]++;

    *Li += 1;
    *OndeLer = true;


    return 1;
}

//funcao que le um registro na parte superior do arquivo
int LeSup(FILE** ArqLEs, TQRegistro* R, int* Ls, bool* OndeLer, long int numero_de_transferencias[2])
{
    if (fseek(*ArqLEs, (*Ls - 1) * sizeof(TQRegistro), 0) != 0)
    {
        return 0;
    }
    if (fread(R, sizeof(TQRegistro), 1, *ArqLEs) != 1)
    {
        return 0;
    }
    numero_de_transferencias[0]++;

    *Ls -= 1;
    *OndeLer = false;

    return 1;
}
//funcao que particiona o arquivo em dois outros subarquivos, ordena o pivô e passa os elementos menores para esquerda e maiores para direita do mesmo
int Particao(FILE** ArqLi, FILE** ArqEi, FILE** ArqLEs, TQRegistro* area, int Esq, int Dir, int* i, int* j, long int numero_de_transferencias[2], long int& numero_de_comparacoes)
{
    //inicializar as variaveis como na definicao do algoritmo
    int Ls = Dir, Es = Dir, Li = Esq, Ei = Esq;
    int tamanho_da_area = 0;
    double Linf = MINIMO, Lsup = MAXIMO;
    bool OndeLer = true;
    TQRegistro UltLido, R;

    //pular para as posicoes a serem trabalhadas no arquivo
    if (fseek(*ArqLi, (Li - 1) * sizeof(TQRegistro), 0) != 0)
    {
        return 0;
    }

    if (fseek(*ArqEi, (Ei - 1) * sizeof(TQRegistro), 0) != 0)
    {
        return 0;
    }

    //inicializar i e j
    *i = Esq - 1;
    *j = Dir + 1;

    while (Li <= Ls)//enquanto os ponteiros de leitura nao se cruzarem
    {
       
        //confere-se se falta um registro para ser inserido na area

        //se nao
        if (tamanho_da_area < TAREA -1 )
        {
            //e lido um registro do arquivo alternativamente comecando da direita
            if (OndeLer)
            {
                if (!LeSup(ArqLEs, &UltLido, &Ls, &OndeLer, numero_de_transferencias))
                {
                    return 0;
                }
            }
            else
            {
                if (!LeInf(ArqLi, &UltLido, &Li, &OndeLer, numero_de_transferencias))
                {
                    return 0;
                }
            }

            //e inserido na area o registro lido
            InserirArea(area, &UltLido, &tamanho_da_area, numero_de_comparacoes);
            continue;
        
        }

        //se nao

        //conferir se o ponteiro de escrita esta quase ultrapassando o ponteiro de leitura
        //fazer a leitura de maneira que nao permita essa ultrapassagem
        if (Ls == Es)
        {
            if (!LeSup(ArqLEs, &UltLido, &Ls, &OndeLer, numero_de_transferencias))
            {
                return 0;
            }
        }
        else if (Li == Ei)
        {
            if (!LeInf(ArqLi, &UltLido, &Li, &OndeLer, numero_de_transferencias))
            {
                return 0;
            }
        }
        else if (OndeLer)
        {
            if (!LeSup(ArqLEs, &UltLido, &Ls, &OndeLer, numero_de_transferencias))
            {
                return 0;
            }
        }
        else
        {
            if (!LeInf(ArqLi, &UltLido, &Li, &OndeLer, numero_de_transferencias))
            {
                return 0;
            }
        }
        numero_de_comparacoes++;

        //se o ultimo item lido for maior que Lsup deve-se escreve-lo no subarquivo a esquerda
        if (UltLido.nota > Lsup)
        {
            *j = Es;
            if (!EscreveMax(ArqLEs, UltLido, &Es, numero_de_transferencias))
            {
                return 0;
            }
            continue;
        }
        numero_de_comparacoes++;
        //caso contrario no subarquivo da esquerda
        if (UltLido.nota < Linf)
        {
            *i = Ei;

            if (!EscreveMin(ArqEi, UltLido, &Ei, numero_de_transferencias))
            {
                return 0;
            }
            continue;
        }

        //se o registro lido estiver entre Linf e Lsup deve ser inserido na area
        InserirArea(area, &UltLido, &tamanho_da_area, numero_de_comparacoes);

        //como a area esta cheia deve-se retirar um elemento

        numero_de_comparacoes++;
        //se o subarquivo da esquerda estiver menor
        //entao deve-se retirar o menor elemento da area
        //e escrever esse menor elemento no subarquivo da esquerda
        if (Ei - Esq < Dir - Es)
        {
            RetiraMin(area, &R, &tamanho_da_area);

            if (!EscreveMin(ArqEi,R, &Ei, numero_de_transferencias))
            {
                return 0;
            }

            Linf = R.nota;
        }
        //se o subarquivo da direita estiver menor
        //entao deve-se retirar o maior elemento da area
        //e escrever esse maior elemento no subarquivo da direita
        else
        {
            RetiraMax(area, &R, &tamanho_da_area);

            if (!EscreveMax(ArqLEs, R, &Es, numero_de_transferencias))
            {
                return 0;
            }
            Lsup = R.nota;
        }      
    }

    //ao sair do loop acima sobrara somente o pivo para ser inserido

    //esse lopp retira os elementos do pivo (area) e escreve no arquivo
    while (Ei <= Es)
    {
        RetiraMin(area, &R, &tamanho_da_area);
        if (!EscreveMin(ArqEi, R, &Ei, numero_de_transferencias))
        {
            return 0;
        }
    }

    delete[] area;
    return 1;
}

//funcao que ordena um arquivo
int QuicksortExterno(FILE** ArqLi, FILE** ArqEi, FILE** ArqLEs, int Esq, int Dir, long int numero_de_transferencias[2], long int& numero_de_comparacoes)
{
    int i, j;
    TQRegistro* area = new TQRegistro[TAREA];

    //se uma particao tem o tamanho < 1 entao deve ser ignorada
    if (Dir - Esq < 1)
    {
        return 1;
    }
    
    //caso contrario devo fazer as particoes do meu arquivo
    //de maneira que os elementos maiores que o pivo fique a direita
    //os elementos menores que o pivo fique a esquerda
    //e o pivo fique ordenado
    if (!Particao(ArqLi, ArqEi, ArqLEs, area, Esq, Dir, &i, &j,numero_de_transferencias, numero_de_comparacoes))
    {
        return 0;
    }

    //deve-se realizar a ordenacao da menor particao primeiro

    //se a menor particao for a da esquerda entao ela e ordenada primeiro
    if (i - Esq < Dir - j)
    {
        if (!QuicksortExterno(ArqLi, ArqEi, ArqLEs, Esq, i, numero_de_transferencias, numero_de_comparacoes))
        {
            return 0;
        }

        if (!QuicksortExterno(ArqLi, ArqEi, ArqLEs, j, Dir, numero_de_transferencias, numero_de_comparacoes))
        {
            return 0;
        }

    }
    else//caso contrario a particao da direita sera ordenada primeiro
    {
        if (!QuicksortExterno(ArqLi, ArqEi, ArqLEs, j, Dir, numero_de_transferencias, numero_de_comparacoes))
        {
            return 0;
        }
        if (!QuicksortExterno(ArqLi, ArqEi, ArqLEs, Esq, i, numero_de_transferencias, numero_de_comparacoes))
        {
            return 0;
        }

    }
    return 1;
}