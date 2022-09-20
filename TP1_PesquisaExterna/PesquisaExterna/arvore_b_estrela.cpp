#include <iostream>
#include <chrono>
#include <stdlib.h>
#include <ctime>
#include "gera_registros.h"
#include "arvore_b_estrela.h"

using namespace std::chrono;
using namespace std;


#define ORDEM 2 // ordem da arvore
#define QUANTIDADE_MAX_ITENS_PAGINA 2*ORDEM // define a quantidade de itens de uma pagina indice
#define QUANTIDADE_MAX_ITENS_FOLHA 4 // define uma quantidade de itens numa pagina folha

struct Registro1
{
    int chave;
    long int dado1;
    char dado2[500];
}; // estrutura do registro a ser retornado


typedef long int TChave;

typedef enum
{
    Indice, Folha
} TIF;

struct Registro
{
    TChave chave;
    long int dado1;
    char dado2[500];

};//estrutura registro

struct TPaginaB//pode ter 2 tipos de paginas
{
    int indice_folha = Folha;//tipo de pagina
    TPaginaB* pagina_anterior = NULL;//a pagina anterior serve para apontar para a pagina que nos levou a uma pagina atual, para futuramente voltar nela ja que nao ? utilizado recursividade

    union
    {
        struct
        {
            int tamanho_indices;//inicialmente a quantidade de itens de uma pagina ? 0
            TChave registros_indices[QUANTIDADE_MAX_ITENS_PAGINA];
            TPaginaB* ponteiros_indices[QUANTIDADE_MAX_ITENS_PAGINA + 1];//um vetor de ponteiros em que cada posicao vai apontar para uma pagina

        }U0;

        struct
        {
            int tamanho_folha;//inicialmente a quantidade de itens de uma pagina ? 0
            TRegistro registros[QUANTIDADE_MAX_ITENS_FOLHA];//um vetor de registros
        }U1;
    }UU;

};//estrutura da pagina


int arvore_b_estrela(FILE* binario, int chave, long int numero_de_transferencia_para_memoria_interna[2],
    long int numero_de_comparacoes_chave_de_pesquisa[2], double tempo_de_execucao[2], TRegistro1& registro_retornado, long int& numero_de_comparacoes_chave_de_pesquisa_real, long int& numero_de_transferencia_para_memoria_interna_real, double& tempo_de_execucao_real)
{
    TRegistro registro;

    //cria arvore
    TPaginaB* raiz_da_arvore = NULL;
    FILE* pesq;


    //ler registros

    if (fopen_s(&binario,"registros.bin", "rb") != 0) //recebe arquivo de resgistros e confere se o arquivo n?o est? vazio
    {
        //cout << "\nFalha ao abrir o arquivo binario para leitura de registros" << endl;
        return 0;
    }//abre arquivo binario para leitura em registros.bin

    if (arquivo_vazio(binario))//se o arquivo estiver vazio posso sair do programa pois nao h? registros para se buscar
    {
        fclose(binario);
        return 0;
    }

    if (fseek(binario, 0, 0) != 0)
    {
        //cout << "\nErro ao movimentar para inicio do arquivo" << endl;
        return 0;
    }

    duration<double> time_span;
    duration<double> time_span2;

    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    //cria arvore 
    cria_arvore(&raiz_da_arvore);

    //ler elementos do arquivo e inserir na arvore

    while (fread(&registro, sizeof(TRegistro), 1, binario) == 1)//leio registro um por um e vou inserindo na arvore
    {
        numero_de_transferencia_para_memoria_interna[0]++;
        //inserir registro na arvore
        if (insere_na_arvore(&raiz_da_arvore, registro, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa) == 0)//confere se elemento a ser inserido ja existe na arvore
        {
            //cout << "\nFalha ao inserir o elemento ja existe" << endl;
        }
    }

    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);


    cout << "\n\n__________________ " << endl << endl;
    //imprime_arvore(raiz_da_arvore);//imprime a arvore para conferir se est? tudo certo. Imrpime comecando dos elemento mais da esquerda

    t1 = high_resolution_clock::now();//marcar tempo da pesquisa

    //buscar a chave na arvore   
    if (busca_na_arvore(raiz_da_arvore, &registro, chave, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa) == 1)//retorna 1 caso encontre
    {
        cout << "\nRegistro encontrado !" << endl;
        cout << "\nRegistro" << endl;
        cout << "Chave: " << registro.chave << endl;
        cout << "Dado1: " << registro.dado1 << endl;
        cout << "Dado2: " << registro.dado2[0] << endl;
        
        registro_retornado.chave = registro.chave;
        registro_retornado.dado1 = registro.dado1;
        registro_retornado.dado2[0] = registro.dado2[0];
    }

    else//nao achou
    {
        cout << "\nRegistro nao encontrado !" << endl;
    }

    t2 = high_resolution_clock::now();
    time_span2 = duration_cast<duration<double>>(t2 - t1);


    tempo_de_execucao[0] = time_span.count();
    tempo_de_execucao[1] = time_span2.count();

    numero_de_comparacoes_chave_de_pesquisa_real = numero_de_comparacoes_chave_de_pesquisa[1];
    numero_de_transferencia_para_memoria_interna_real = numero_de_transferencia_para_memoria_interna[1];
    tempo_de_execucao_real = tempo_de_execucao[1];

    if (fopen_s(&pesq, "registros.bin", "rb") != 0)
    {
        //cout << "\nFalha ao criar o arquivo binario para leitura de registros" << endl;
        return 0;
    }//abre arquivo binario para leitura em registros.bin


    for (int i = 1; i <= 9; i++)
    {
        fread(&registro, sizeof(TRegistro), 1, pesq);
        t1 = high_resolution_clock::now();//marcar tempo da pesquisa

        busca_na_arvore(raiz_da_arvore, &registro, registro.chave, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa);
        t2 = high_resolution_clock::now();
        time_span2 = duration_cast<duration<double>>(t2 - t1);
        tempo_de_execucao[1] += time_span2.count();
    }

    fclose(binario);
    fclose(pesq);

    cout << "\nArvore B Estrela concluida !" << endl;

    return 1;
}

int arquivo_vazio(FILE* binario)//funcao que verifica se um arquivo est? vazio
{
    int inicio_do_arquivo, final_do_arquivo;


    if (fseek(binario, 0, 0) != 0)
    {
        //cout << "\nFalha em conferir se o arquivo est? vazio.\nErro ao movimentar para inicio do arquivo" << endl;
        return 1;
    }

    inicio_do_arquivo = ftell(binario);//caso n?o d? erro ao movimentar para o inico do arquivo a variavel inicio_do_arquivo obtem o numero de bytes at? na posicao do arquivo ou seja 0

    if (fseek(binario, 0, 2) != 0)
    {
        //cout << "\nFalha em conferir se o arquivo est? vazio.\nErro ao movimentar para final do arquivo" << endl;
        return 1;
    }

    final_do_arquivo = ftell(binario);//caso n?o d? erro ao movimentar para o final do arquivo a vari?vel final_do_arquivo armazena o numero de bytes do arquivo

    if (inicio_do_arquivo == final_do_arquivo)//se a quantidade de bytes ate no final do arquivo for igual a do inicio o arquivo esta vazio
    {
        //cout << "\nArquivo Vazio" << endl;
        return 1;
    }


    return 0;
}

void cria_arvore(TPaginaB** raiz_da_arvore)//funcao que cria uma arvore b estrela
{
    *raiz_da_arvore = new TPaginaB;

    (*raiz_da_arvore)->UU.U1.tamanho_folha = 0;
    //inicialmente a raiz e uma pagina folha
}



int ponteiro_para_proxima_pagina(TPaginaB* pagina_atual, TRegistro registro, long int numero_de_transferencia_para_memoria_interna[2], long int numero_de_comparacoes_chave_de_pesquisa[2], char cmp)//funcao que recebe uma pagina e um registro e devolve o ponteiro para uma pagina em que o registro pode estar localizado
{
    int ponteiro_para_pagina_folha = 0;//inicialmente o ponteiro recebe posicao 0

    while (ponteiro_para_pagina_folha < pagina_atual->UU.U0.tamanho_indices)//enquanto for menor que o tamanho da pagina
    {

        if (cmp == 'c')
        {
            numero_de_comparacoes_chave_de_pesquisa[0]++;
            numero_de_comparacoes_chave_de_pesquisa[0]++;
        }

        else
        {
            numero_de_comparacoes_chave_de_pesquisa[1]++;
            numero_de_comparacoes_chave_de_pesquisa[1]++;
        }

        if (registro.chave < pagina_atual->UU.U0.registros_indices[ponteiro_para_pagina_folha])//se o registro for menor que o elemento da frente ja achei a posicao
        {
            break;//saio do loop
        }

        ponteiro_para_pagina_folha++;//acrescento um no ponteiro caso meu registro seja maior o elemento da frente
    }

    return ponteiro_para_pagina_folha;//retorno meu ponteiro
}


int posicao_que_deve_ser_inserido(TPaginaB* pagina_atual, TRegistro registro, long int numero_de_transferencia_para_memoria_interna[2], long int numero_de_comparacoes_chave_de_pesquisa[2])//funcao que recebe uma pagina e um registro e devolve a posicao em que o registro deve ser inserido
{
    int item;//inicialmente declaro um item

    if (pagina_atual->indice_folha == Folha)
    {
        for (item = 0; item < pagina_atual->UU.U1.tamanho_folha; item++)//inicializo o item em 0 e enquanto ele for menor que a quantidade de itens da pagina posso acrescentar
        {
            numero_de_comparacoes_chave_de_pesquisa[0]++;
            if (registro.chave < pagina_atual->UU.U1.registros[item].chave)//se a chave do registro for menor que o registro da pagina na posicao item, sai do loop
            {
                break;
            }
        }
    }

    else
    {
        for (item = 0; item < pagina_atual->UU.U0.tamanho_indices; item++)//inicializo o item em 0 e enquanto ele for menor que a quantidade de itens da pagina posso acrescentar
        {
            numero_de_comparacoes_chave_de_pesquisa[0]++;
            if (registro.chave < pagina_atual->UU.U0.registros_indices[item])//se a chave do registro for menor que o registro da pagina na posicao item, sai do loop
            {
                break;
            }
        }
    }

    return item;//retorno a posicao a ser inserido
}

void insere_no_folha(TPaginaB** pagina_atual, TRegistro* registro, long int numero_de_transferencia_para_memoria_interna[2], long int numero_de_comparacoes_chave_de_pesquisa[2])
{

    int posicao;
    TRegistro* copia = new TRegistro[(*pagina_atual)->UU.U1.tamanho_folha];//crio um vetor de registros copia

    posicao = posicao_que_deve_ser_inserido(*pagina_atual, *registro, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa);//recebe a posicao que deve ser inserido

    for (int item = 0; item < (*pagina_atual)->UU.U1.tamanho_folha; item++)//copio os registros criados
    {
        copia[item] = (*pagina_atual)->UU.U1.registros[item];

    }

    (*pagina_atual)->UU.U1.registros[posicao] = *registro;//insiro o registro na posicao adequada

    //mover registros para frente
    for (int item = posicao + 1; item <= (*pagina_atual)->UU.U1.tamanho_folha; item++)
    {
        (*pagina_atual)->UU.U1.registros[item] = copia[item - 1];
    }

    //aumenta a quantidade de itens na pagina ja que foi inserido um registro
    (*pagina_atual)->UU.U1.tamanho_folha++;

}

void insere_na_pagina(TPaginaB* pagina_atual, TRegistro registro, int posicao)//funcao que recebe uma pagina, um registro e uma posicao.Essa funcao insere na pagina recebida a chave do registro na posicao recebida
{
    TChave* copia = new TChave[pagina_atual->UU.U0.tamanho_indices];//crio um vetor de registros copia
    TPaginaB** temporario = new TPaginaB * [pagina_atual->UU.U0.tamanho_indices + 1];//crio um vetor temporario de ponteiros

    //insere um registro na pagina
    for (int item = 0; item < pagina_atual->UU.U0.tamanho_indices; item++)//copio os registros e ponteiro para os vetores criados
    {
        copia[item] = pagina_atual->UU.U0.registros_indices[item];
        temporario[item] = pagina_atual->UU.U0.ponteiros_indices[item];
    }
    temporario[pagina_atual->UU.U0.tamanho_indices] = pagina_atual->UU.U0.ponteiros_indices[pagina_atual->UU.U0.tamanho_indices];//aponta para o ultimo ponteiro porque o loop nao faz

    pagina_atual->UU.U0.registros_indices[posicao] = registro.chave;//insiro o registro na posicao adequada

    //mover ponteiros e registros para frente
    for (int item = posicao + 1; item <= pagina_atual->UU.U0.tamanho_indices; item++)
    {
        pagina_atual->UU.U0.registros_indices[item] = copia[item - 1];
        pagina_atual->UU.U0.ponteiros_indices[item + 1] = temporario[item];
    }

    //aumenta a quantidade de itens na pagina ja que foi inserido um registro
    pagina_atual->UU.U0.tamanho_indices++;
}

void imprime_arvore(TPaginaB* raiz_da_arvore)//funcao que imprime a arvore sempre indo para as paginas mais a esquerda primeiro
{

    if (raiz_da_arvore->indice_folha == Folha)
    {
        for (int item = 0; item < raiz_da_arvore->UU.U1.tamanho_folha; item++)
        {
            cout << "\nRegistro " << item << endl;
            cout << "Chave " << raiz_da_arvore->UU.U1.registros[item].chave << endl;
            cout << "Dado1 " << raiz_da_arvore->UU.U1.registros[item].dado1 << endl;
            cout << "Dado2 " << raiz_da_arvore->UU.U1.registros[item].dado2 << endl;
        }
    }

    else
    {
        for (int item = 0; item < raiz_da_arvore->UU.U0.tamanho_indices; item++)
        {
            cout << "\nRegistro " << item << endl;
            cout << "Chave " << raiz_da_arvore->UU.U0.registros_indices[item] << endl;
            cout << "Dado1 " << raiz_da_arvore->UU.U0.registros_indices[item] << endl;
            cout << "Dado2 " << raiz_da_arvore->UU.U0.registros_indices[item] << endl;
        }

        for (int item = 0; item <= raiz_da_arvore->UU.U0.tamanho_indices; item++)
        {
            imprime_arvore(raiz_da_arvore->UU.U0.ponteiros_indices[item]);
        }
    }

}

int insere_na_arvore(TPaginaB** raiz_da_arvore, TRegistro registro, long int numero_de_transferencia_para_memoria_interna[2], long int numero_de_comparacoes_chave_de_pesquisa[2])//funcao que insere um registro na arvore, retorna 1 caso o registro seja inserido com sucesso, retorna 0 caso contr?rio
{
    bool nao_achei_pagina_folha = true;//utilizado para achar pagina folha
    int proxima_pagina;//recebe a posicao de um ponteiro para uma proxima pagina

    bool registro_subiu = true;//indica que um registro subiu
    int posicao;

    TPaginaB* pagina_atual = *raiz_da_arvore;//inicialmente a pagina atual e raiz
    TPaginaB* esquerda_de_baixo = NULL, * direita_de_baixo = NULL;//esquerda_de_baixo e direita_de_baixo s?o paginas dividida que estao abaixo de uma pagina onde um registro deve ser inserido

    TPaginaB* temporaria;//uma pagina temporaria
    TPaginaB* esquerda = NULL;//pagina esquerda dividida atualmente
    TPaginaB* direita = NULL;//pagina direita dividida atualmente

    TRegistro registro_do_meio;//um registro auxiliar que vai ajudar a trocar o registro a ser inserido

    //conferir se a raiz nao ? nula ou seja arvore vazia

    //descer ate a pagina folha

    while (nao_achei_pagina_folha)//direciona para o no folha adequado ao registro e confere se o registro ja nao esta na arvore
    {
        proxima_pagina = ponteiro_para_proxima_pagina(pagina_atual, registro, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa, 'c');

        if (pagina_atual->indice_folha == Folha)//se a pagina for uma pagina folha cheguei na pagina que queria
        {
            nao_achei_pagina_folha = false;
        }
        else//se nao chegamos ao no folha
        {
            pagina_atual = pagina_atual->UU.U0.ponteiros_indices[proxima_pagina];//apontamos para a proxima pagina
        }
    }

    //apos chegar a pagina folha conferir se o elemento ja existe

    //busca sequencial para encontrar elemento
    for (int item = 0; item < pagina_atual->UU.U1.tamanho_folha; item++)
    {
        numero_de_comparacoes_chave_de_pesquisa[0]++;
        if (pagina_atual->UU.U1.registros[item].chave == registro.chave)//se achar o elemento saio da funcao retornando 0 pois nao pode inserir um elemento repetido
        {
            return 0;
        }

        if (item < pagina_atual->UU.U1.tamanho_folha - 1)
        {
            numero_de_comparacoes_chave_de_pesquisa[0]++;
            if (pagina_atual->UU.U1.registros[item + 1].chave > registro.chave)//item da frente ? maior logo nao sera encontrado
            {
                break;
            }
        }
    }

    //conferir se no folha esta cheio

    if (pagina_atual->UU.U1.tamanho_folha < QUANTIDADE_MAX_ITENS_FOLHA)//tem espa?o
    {
        insere_no_folha(&pagina_atual, &registro, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa);//? s? inserir no no folha ja que tem espa?o
        return 1;
    }

    else//dividir pagina e criar uma nova obs: pagina cheia
    {
        //dividir paginas
        esquerda = pagina_atual;

        //crio a pagina da direita e inicializo
        direita = new TPaginaB;
        direita->pagina_anterior = pagina_atual->pagina_anterior;
        direita->UU.U1.tamanho_folha = 0;

        for (posicao = 0; posicao < pagina_atual->UU.U1.tamanho_folha; posicao++)
        {
            if (pagina_atual->UU.U1.registros[posicao].chave > registro.chave)
            {
                break;
            }
        }


        if (posicao <= (pagina_atual->UU.U1.tamanho_folha / 2) - 1)//inserir na esquerda
        {
            for (int item = (pagina_atual->UU.U1.tamanho_folha / 2) - 1, item2 = 0; item < QUANTIDADE_MAX_ITENS_FOLHA; item++, item2++)
            {
                direita->UU.U1.registros[item2] = esquerda->UU.U1.registros[item];
                direita->UU.U1.tamanho_folha++;
                esquerda->UU.U1.tamanho_folha--;
            }

            insere_no_folha(&esquerda, &registro, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa);
        }
        else //insere na direita
        {
            for (int item = (pagina_atual->UU.U1.tamanho_folha / 2), item2 = 0; item < QUANTIDADE_MAX_ITENS_FOLHA; item++, item2++)
            {
                direita->UU.U1.registros[item2] = esquerda->UU.U1.registros[item];
                direita->UU.U1.tamanho_folha++;
                esquerda->UU.U1.tamanho_folha--;
            }
            insere_no_folha(&direita, &registro, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa);
        }

        registro = direita->UU.U1.registros[0];//o novo registro a ser inserido sera primeiro da direita
        esquerda_de_baixo = esquerda;//a nova esquerda de baixo sera minha esquerda atual
        direita_de_baixo = direita;//a nova direita de baixo sera a direita atual
        esquerda = NULL;//a esquerda atual e nula
        direita = NULL;//a direita atual e nula
        pagina_atual = pagina_atual->pagina_anterior;//minha nova pagina atual vai ser a pagina anterior]

    }

    //deve-se tentar inserir o registro na arvore
    while (registro_subiu)//enquanto o registro subir
    {
        //conferir se pagina tem espa?o
        if (pagina_atual == NULL)//chegamos acima da raiz, pois sempre andamos para a pagina de tras e se a pagina de tras for nula chegamos na raiz
        {
            temporaria = new TPaginaB;//crio uma pagina temporaria que sera a nova raiz
            temporaria->indice_folha = Indice;
            temporaria->UU.U0.tamanho_indices = 0;

            for (int item = 0; item <= QUANTIDADE_MAX_ITENS_PAGINA; item++)//inicializo os ponteiros da pagina
            {
                temporaria->UU.U0.ponteiros_indices[item] = NULL;
            }
            temporaria->pagina_anterior = NULL;//como e a raiz a pagina anterior a ela sera NULL
            insere_na_pagina(temporaria, registro, 0);//insere-se na pagina o registro

            temporaria->UU.U0.ponteiros_indices[0] = esquerda_de_baixo;//o ponteiro na posicao 0 aponta para a pagina dividida a esquerda
            temporaria->UU.U0.ponteiros_indices[1] = direita_de_baixo;//o ponteiro na posicao 1 aponta para a pagina dividida a direita

            //ambas as paginas esquerda de baixo e direita de baixo vao apontar para seu pai, a nova raiz(temporaria)
            esquerda_de_baixo->pagina_anterior = temporaria;
            direita_de_baixo->pagina_anterior = temporaria;


            *raiz_da_arvore = temporaria;// a raiz da arvore agora sera uma nova raiz


            break;//posso sair do loop ja que inseri no topo
        }
        if (pagina_atual->UU.U0.tamanho_indices < QUANTIDADE_MAX_ITENS_PAGINA)//caso a pagina tenha espaco
        {
            posicao = posicao_que_deve_ser_inserido(pagina_atual, registro, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa);//posicao recebe a posicao que o registro deve ser inserido
            insere_na_pagina(pagina_atual, registro, posicao);//insiro na pagina atual o registro


            //ajeitar ponteiros
            //os ponteiros ao lado do registro inserido na pagina de cima deve apontar para as paginas divididas abaixo
            pagina_atual->UU.U0.ponteiros_indices[posicao] = esquerda_de_baixo;
            pagina_atual->UU.U0.ponteiros_indices[posicao + 1] = direita_de_baixo;

            //esquerda e direita e de baixo tem que saber de qual pagina ela veio
            if (esquerda_de_baixo != NULL)
            {
                esquerda_de_baixo->pagina_anterior = pagina_atual;
            }
            if (direita_de_baixo != NULL)
            {
                direita_de_baixo->pagina_anterior = pagina_atual;
            }

            registro_subiu = false;//registro nao vai subir mais posso sair do loop
        }

        else//dividir pagina e criar uma nova obs: pagina cheia
        {


            //dividir paginas
            esquerda = pagina_atual;

            //crio a pagina da direita e inicializo
            direita = new TPaginaB;
            direita->pagina_anterior = pagina_atual->pagina_anterior;
            direita->indice_folha = Indice;
            direita->UU.U0.tamanho_indices = 0;

            for (int item = 0; item <= QUANTIDADE_MAX_ITENS_PAGINA; item++)
            {
                direita->UU.U0.ponteiros_indices[item] = NULL;
            }

            //qual posicao que se encaixa nosso registro
            posicao = posicao_que_deve_ser_inserido(pagina_atual, registro, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa);

            numero_de_comparacoes_chave_de_pesquisa[0]++;
            if (posicao < ORDEM)//vou inserir na esquerda
            {
                //novo registro sera, pois estara no meio
                registro_do_meio.chave = pagina_atual->UU.U0.registros_indices[ORDEM - 1];

                //removo os elementos da esquerda e digo o numero de elemento que estara na direita
                direita->UU.U0.tamanho_indices = ORDEM;
                esquerda->UU.U0.tamanho_indices = ORDEM - 1;

                for (int item = ORDEM, item2 = 0; item < QUANTIDADE_MAX_ITENS_PAGINA; item++, item2++)//insiro os elemetos na direita tanto os registros quanto os ponteiros
                {
                    direita->UU.U0.registros_indices[item2] = esquerda->UU.U0.registros_indices[item];
                    direita->UU.U0.ponteiros_indices[item2] = esquerda->UU.U0.ponteiros_indices[item];

                    if (direita->UU.U0.ponteiros_indices[item2] != NULL)
                    {
                        if (direita->UU.U0.ponteiros_indices[item2]->pagina_anterior != NULL)
                        {
                            direita->UU.U0.ponteiros_indices[item2]->pagina_anterior = direita;
                        }
                    }
                }

                direita->UU.U0.ponteiros_indices[ORDEM] = esquerda->UU.U0.ponteiros_indices[QUANTIDADE_MAX_ITENS_PAGINA];//nao esquecer do ultimo ponteiro que o for nao faz

                if (direita->UU.U0.ponteiros_indices[ORDEM] != NULL)
                {
                    if (direita->UU.U0.ponteiros_indices[ORDEM]->pagina_anterior != NULL)
                    {
                        direita->UU.U0.ponteiros_indices[ORDEM]->pagina_anterior = direita;
                    }
                }
                //insiro na esquerda nosso registro e ajeito seus ponteiros apontando para pagina abaixo
                insere_na_pagina(esquerda, registro, posicao);
                esquerda->UU.U0.ponteiros_indices[posicao] = esquerda_de_baixo;
                esquerda->UU.U0.ponteiros_indices[posicao + 1] = direita_de_baixo;

                //mudar apontadores  anteriores
                if (esquerda_de_baixo != NULL)
                {
                    esquerda_de_baixo->pagina_anterior = esquerda;
                }
                if (direita_de_baixo != NULL)
                {
                    direita_de_baixo->pagina_anterior = esquerda;
                }

                registro = registro_do_meio;//nosso novo elemento a ser inserido sera o registro do meio
            }

            else if (posicao > ORDEM)//vou inserir na direita
            {
                numero_de_comparacoes_chave_de_pesquisa[0]++;

                registro_do_meio.chave = pagina_atual->UU.U0.registros_indices[ORDEM]; //novo registro sera, pois estara no meio

                //removo os elementos da esquerda e digo o numero de elemento que estara na direita
                direita->UU.U0.tamanho_indices = ORDEM - 1;
                esquerda->UU.U0.tamanho_indices = ORDEM;

                //insiro os elemetos na direita tanto os registros quanto os ponteiros
                for (int item = ORDEM + 1, item2 = 0; item < QUANTIDADE_MAX_ITENS_PAGINA; item++, item2++)
                {
                    direita->UU.U0.registros_indices[item2] = esquerda->UU.U0.registros_indices[item];
                    direita->UU.U0.ponteiros_indices[item2] = esquerda->UU.U0.ponteiros_indices[item];

                    if (direita->UU.U0.ponteiros_indices[item2] != NULL)
                    {
                        if (direita->UU.U0.ponteiros_indices[item2]->pagina_anterior != NULL)
                        {
                            direita->UU.U0.ponteiros_indices[item2]->pagina_anterior = direita;
                        }
                    }
                }
                direita->UU.U0.ponteiros_indices[ORDEM - 1] = esquerda->UU.U0.ponteiros_indices[QUANTIDADE_MAX_ITENS_PAGINA];//nao esquecer do ultimo ponteiro

                if (direita->UU.U0.ponteiros_indices[ORDEM-1] != NULL)
                {
                    if (direita->UU.U0.ponteiros_indices[ORDEM-1]->pagina_anterior != NULL)
                    {
                        direita->UU.U0.ponteiros_indices[ORDEM-1]->pagina_anterior = direita;
                    }
                }
                posicao = posicao_que_deve_ser_inserido(direita, registro, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa);//posicao a ser inserido na direita, na esquerda nao tem essa parte por que a funcao anterior serve para ela mas aqui na direita uma nova pagina foi criada

                //insiro na direita nosso registro e ajeito seus ponteiros apontando para pagina abaixo
                insere_na_pagina(direita, registro, posicao);
                direita->UU.U0.ponteiros_indices[posicao] = esquerda_de_baixo;
                direita->UU.U0.ponteiros_indices[posicao + 1] = direita_de_baixo;

                //mudar apontadores  anteriores
                if (esquerda_de_baixo != NULL)
                {
                    esquerda_de_baixo->pagina_anterior = direita;
                }

                if (direita_de_baixo != NULL)
                {
                    direita_de_baixo->pagina_anterior = direita;
                }

                registro = registro_do_meio;//nosso novo elemento a ser inserido sera o registro do meio
            }

            else//o registro a inserir ? o do meio
            {
                numero_de_comparacoes_chave_de_pesquisa[0]++;
                //divido igualmente os registros
                direita->UU.U0.tamanho_indices = ORDEM;
                esquerda->UU.U0.tamanho_indices = ORDEM;

                //insiro os elemetos na direita tanto os registros quanto os ponteiros
                for (int item = ORDEM, item2 = 0; item < QUANTIDADE_MAX_ITENS_PAGINA; item++, item2++)
                {
                    direita->UU.U0.registros_indices[item2] = esquerda->UU.U0.registros_indices[item];
                    direita->UU.U0.ponteiros_indices[item2 + 1] = esquerda->UU.U0.ponteiros_indices[item + 1];
                    if (direita->UU.U0.ponteiros_indices[item2+1] != NULL)
                    {
                        if (direita->UU.U0.ponteiros_indices[item2+1]->pagina_anterior != NULL)
                        {
                            direita->UU.U0.ponteiros_indices[item2+1]->pagina_anterior = direita;
                        }
                    }
                }

                direita->UU.U0.ponteiros_indices[0] = direita_de_baixo; //o primeiro ponteiro da direita aponta para pagina direita de baixo
                esquerda->UU.U0.ponteiros_indices[ORDEM] = esquerda_de_baixo;//o ultimo ponteiro da esquerda aponta para pagina esquerda de baixo 

                //mudar apontadores  anteriores
                if (esquerda_de_baixo != NULL)
                {
                    esquerda_de_baixo->pagina_anterior = esquerda;
                }

                if (direita_de_baixo != NULL)
                {
                    direita_de_baixo->pagina_anterior = direita;
                }

            }
        }

        esquerda_de_baixo = esquerda;//a nova esquerda de baixo sera minha esquerda atual
        direita_de_baixo = direita;//a nova direita de baixo sera a direita atual
        esquerda = NULL;//a esquerda atual e nula
        direita = NULL;//a direita atual e nula
        pagina_atual = pagina_atual->pagina_anterior;//minha nova pagina atual vai ser a pagina anterior
    }

    return 1;//retorna 1 caso d? tudo certo com a insercao
}

int busca_na_arvore(TPaginaB* raiz, TRegistro* registro, int chave, long int numero_de_transferencia_para_memoria_interna[2], long int numero_de_comparacoes_chave_de_pesquisa[2])//funcao que dado uma arvore, um registro e uma chave, busca na arvore o registro com a chave especificada. Retorna 1 e o registro caso o registro seja encontrado, caso contr?rio retorna 0
{
    bool nao_achei_pagina_folha = true;//utilizado para achar pagina folha
    int proxima_pagina;//recebe a posicao de um ponteiro para uma proxima pagina
    TRegistro copia;
    copia.chave = chave;
    TPaginaB* pagina_atual = raiz;//inicialmente a pagina atual e raiz

    //conferir se a raiz nao ? nula ou seja arvore vazia

    //descer ate a pagina folha

    while (nao_achei_pagina_folha)//direciona para o no folha adequado ao registro e confere se o registro ja nao esta na arvore
    {
        proxima_pagina = ponteiro_para_proxima_pagina(pagina_atual, copia, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa, 'p');

        if (pagina_atual->indice_folha == Folha)//se a pagina for uma pagina folha cheguei na pagina que queria
        {
            nao_achei_pagina_folha = false;
        }
        else//se nao chegamos ao no folha
        {
            pagina_atual = pagina_atual->UU.U0.ponteiros_indices[proxima_pagina];//apontamos para a proxima pagina
        }
    }

    //apos chegar a pagina folha tenho que procurar o registro com a chave especificada

    for (int item = 0; item < pagina_atual->UU.U1.tamanho_folha; item++)
    {
        numero_de_comparacoes_chave_de_pesquisa[1]++;
        if (pagina_atual->UU.U1.registros[item].chave == chave)
        {
            *registro = pagina_atual->UU.U1.registros[item];
            return 1;
        }
    }

    return 0;//retorna 0 caso nao encontre o elemento
}