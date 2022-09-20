#include <iostream>
#include <chrono>
#include <stdlib.h>
#include <ctime>
#include "gera_registros.h"
#include "arvoreb.h"

using namespace std::chrono;
using namespace std;

#define ORDEM 2
#define QUANTIDADE_MAX_ITENS_PAGINA 2*ORDEM

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

};//estrutura registro

struct TPagina
{
    TPagina* pagina_anterior = NULL;//a pagina anterior serve para apontar para a pagina que nos levou a uma pagina atual, para futuramente voltar nela ja que nao ? utilizado recursividade

    TRegistro registros[QUANTIDADE_MAX_ITENS_PAGINA];//um vetor de registros
    TPagina* ponteiros[QUANTIDADE_MAX_ITENS_PAGINA + 1];//um vetor de ponteiros em que cada posicao vai apontar para uma pagina

    int quantidade_de_itens = 0;//inicialmente a quantidade de itens de uma pagina ? 0

};//estrutura da pagina


int arvoreb(FILE* binario, int chave, long int numero_de_transferencia_para_memoria_interna[2],
    long int numero_de_comparacoes_chave_de_pesquisa[2],double tempo_de_execucao[2], TRegistro1& registro_retornado, long int& numero_de_comparacoes_chave_de_pesquisa_real, long int& numero_de_transferencia_para_memoria_interna_real, double& tempo_de_execucao_real)
{
    TRegistro registro;
    //cria arvore
    TPagina* raiz_da_arvore = NULL;
    FILE* pesq;

    //ler registros

    if ( fopen_s(&binario,"registros.bin", "rb") != 0) //recebe arquivo de resgistros e confere se o arquivo n?o est? vazio
    {
        //cout << "\nFalha ao abrir o arquivo binario para leitura de registros" << endl;
        return 0;
    }//abre arquivo binario para leitura em registros.bin

    if (b_arquivo_vazio(binario))//se o arquivo estiver vazio posso sair do programa pois nao h? registros para se buscar
    {
        fclose(binario);
        return 0;
    }

    if (fseek(binario, 0, 0) != 0)
    {
        //cout << "\nErro ao movimentar para inicio do arquivo" << endl;
        return 0;
    }

    //tempo de criacao da arvore
    duration<double> time_span;
    duration<double> time_span2;

    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    //inicializa arvore
    raiz_da_arvore = new TPagina;
    for (int item = 0; item <= QUANTIDADE_MAX_ITENS_PAGINA; item++)
    {
        raiz_da_arvore->ponteiros[item] = NULL;
    }

    //ler elementos do arquivo e inserir na arvore

    while (fread(&registro, sizeof(TRegistro), 1, binario) == 1)//leio registro um por um e vou inserindo na arvore
    {
        numero_de_transferencia_para_memoria_interna[0]++;
        //inserir registro na arvore
        if (b_insere_na_arvore(&raiz_da_arvore, registro, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa) == 0)//confere se elemento a ser inserido ja existe na arvore
        {
            //cout << "\nFalha ao inserir o elemento ja existe" << endl;
        }

       // b_imprime_arvore(raiz_da_arvore);
    }

    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);


    cout << "\n\n__________________ " << endl << endl;
    //b_imprime_arvore(raiz_da_arvore);//imprime a arvore para conferir se est? tudo certo. Imrpime comecando dos elemento mais da esquerda

    t1 = high_resolution_clock::now();//marcar tempo da pesquisa

    cout << "\nArvore B concluida !" << endl;

    //buscar a chave na arvore   
    if (b_busca_na_arvore(raiz_da_arvore, &registro, chave, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa) == 1)//retorna 1 caso encontre
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

        b_busca_na_arvore(raiz_da_arvore, &registro, registro.chave, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa);
        t2 = high_resolution_clock::now();
        time_span2 = duration_cast<duration<double>>(t2 - t1);
        tempo_de_execucao[1] += time_span2.count();
    }

    fclose(binario);
    fclose(pesq);

    return 1;
}

int b_arquivo_vazio(FILE* binario)//funcao que verifica se um arquivo est? vazio
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

int b_ponteiro_para_proxima_pagina(TPagina* pagina_atual, TRegistro registro, long int numero_de_transferencia_para_memoria_interna[2], long int numero_de_comparacoes_chave_de_pesquisa[2], char cmp)//funcao que recebe uma pagina e um registro e devolve o ponteiro para uma pagina em que o registro pode estar localizado
{
    int ponteiro_para_pagina_folha = 0;//inicialmente o ponteiro recebe posicao 0

    while (ponteiro_para_pagina_folha < pagina_atual->quantidade_de_itens)//enquanto for menor que o tamanho da pagina
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



        if (registro.chave < pagina_atual->registros[ponteiro_para_pagina_folha].chave)//se o registro for menor que o elemento da frente ja achei a posicao
        {
            break;//saio do loop
        }

        ponteiro_para_pagina_folha++;//acrescento um no ponteiro caso meu registro seja maior o elemento da frente
    }

    return ponteiro_para_pagina_folha;//retorno meu ponteiro
}//para conferir se o elemento j? existe tem que retornar um valor maior que 0


int b_posicao_que_deve_ser_inserido(TPagina* pagina_atual, TRegistro registro, long int numero_de_transferencia_para_memoria_interna[2], long int numero_de_comparacoes_chave_de_pesquisa[2])//funcao que recebe uma pagina e um registro e devolve a posicao em que o registro deve ser inserido
{
    int item;//inicialmente declaro um item

    for (item = 0; item < pagina_atual->quantidade_de_itens; item++)//inicializo o item em 0 e enquanto ele for menor que a quantidade de itens da pagina posso acrescentar
    {
        numero_de_comparacoes_chave_de_pesquisa[0]++;
        if (registro.chave < pagina_atual->registros[item].chave)//se a chave do registro for menor que o registro da pagina na posicao item, sai do loop
        {
            break;
        }
    }

    return item;//retorno a posicao a ser inserido
}

void b_insere_na_pagina(TPagina* pagina_atual, TRegistro registro, int posicao)//funcao que recebe uma pagina, um registro e uma posicao.Essa funcao insere na pagina recebida um registro na posicao recebida
{
    TRegistro* copia = new TRegistro[pagina_atual->quantidade_de_itens];//crio um vetor de registros copia
    TPagina** temporario = new TPagina * [pagina_atual->quantidade_de_itens + 1];//crio um vetor temporario de ponteiros

    //insere um registro na pagina
    for (int item = 0; item < pagina_atual->quantidade_de_itens; item++)//copio os registros e ponteiro para os vetores criados
    {
        copia[item] = pagina_atual->registros[item];
        temporario[item] = pagina_atual->ponteiros[item];
    }
    temporario[pagina_atual->quantidade_de_itens] = pagina_atual->ponteiros[pagina_atual->quantidade_de_itens];//aponta para o ultimo ponteiro porque o loop nao faz

    pagina_atual->registros[posicao] = registro;//insiro o registro na posicao adequada

    //mover ponteiros e registros para frente
    for (int item = posicao + 1; item <= pagina_atual->quantidade_de_itens; item++)
    {
        pagina_atual->registros[item] = copia[item - 1];
        pagina_atual->ponteiros[item + 1] = temporario[item];
    }

    //aumenta a quantidade de itens na pagina ja que foi inserido um registro
    pagina_atual->quantidade_de_itens++;
}

void b_imprime_arvore(TPagina* raiz_da_arvore)//funcao que imprime a arvore sempre indo para as paginas mais a esquerda primeiro
{
    if (raiz_da_arvore == NULL)
    {
        return;
    }

    else
    {
        for (int item = 0; item <= raiz_da_arvore->quantidade_de_itens; item++)
        {
            b_imprime_arvore(raiz_da_arvore->ponteiros[item]);

           if(item < raiz_da_arvore->quantidade_de_itens)
            {
                cout << "\nRegistro " << item << endl;
                cout << "Chave " << raiz_da_arvore->registros[item].chave << endl;
                cout << "Dado1 " << raiz_da_arvore->registros[item].dado1 << endl;
                //cout << "Dado2 " << raiz_da_arvore->registros[item].dado2 << endl;
            }
        }
        
    }
}

int b_insere_na_arvore(TPagina** raiz_da_arvore, TRegistro registro, long int numero_de_transferencia_para_memoria_interna[2], long int numero_de_comparacoes_chave_de_pesquisa[2])//funcao que insere um registro na arvore, retorna 1 caso o registro seja inserido com sucesso, retorna 0 caso contr?rio
{
    bool nao_achei_pagina_folha = true;//utilizado para achar pagina folha
    int proxima_pagina;//recebe a posicao de um ponteiro para uma proxima pagina

    bool registro_subiu = true;//indica que um registro subiu
    int posicao;

    TPagina* pagina_atual = *raiz_da_arvore;//inicialmente a pagina atual e raiz
    TPagina* esquerda_de_baixo = NULL, * direita_de_baixo = NULL;//esquerda_de_baixo e direita_de_baixo s?o paginas dividida que estao abaixo de uma pagina onde um registro deve ser inserido

    TPagina* temporaria;//uma pagina temporaria
    TPagina* esquerda = NULL;//pagina esquerda dividida atualmente
    TPagina* direita = NULL;//pagina direita dividida atualmente

    TRegistro registro_do_meio;//um registro auxiliar que vai ajudar a trocar o registro a ser inserido

    //descer ate a pagina folha

    while (nao_achei_pagina_folha)//direciona para o no folha adequado ao registro e confere se o registro ja nao esta na arvore
    {
        //conferir se elemento nao existe
        proxima_pagina = b_ponteiro_para_proxima_pagina(pagina_atual, registro, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa, 'c');
        numero_de_comparacoes_chave_de_pesquisa[0]++;
        if (proxima_pagina > 0 && pagina_atual->registros[proxima_pagina - 1].chave == registro.chave)//conferir se elemento j? existe
        {
            return 0;//elemento j? existe retorna 0
        }

        if (pagina_atual->ponteiros[0] == NULL)//como a arvore e balanceada se tem um ponteiro inicial nulo chegamos em uma pagina folha
        {
            nao_achei_pagina_folha = false;
        }
        else//se nao chegamos ao no folha
        {
            pagina_atual = pagina_atual->ponteiros[proxima_pagina];//apontamos para a proxima pagina
        }
    }

    //apos chegar ao no folha e conferir que o registro nao existe
    //deve-se tentar inserir o registro na arvore


    while (registro_subiu)//enquanto o registro subir
    {
        //conferir se pagina tem espa?o
        if (pagina_atual == NULL)//chegamos acima da raiz, pois sempre andamos para a pagina de tras e se a pagina de tras for nula chegamos na raiz
        {
            temporaria = new TPagina;//crio uma pagina temporaria que sera a nova raiz

            for (int item = 0; item <= QUANTIDADE_MAX_ITENS_PAGINA; item++)//inicializo os ponteiros da pagina
            {
                temporaria->ponteiros[item] = NULL;
            }
            temporaria->pagina_anterior = NULL;//como e a raiz a pagina anterior a ela sera NULL
            b_insere_na_pagina(temporaria, registro, 0);//insere-se na pagina o registro

            temporaria->ponteiros[0] = esquerda_de_baixo;//o ponteiro na posicao 0 aponta para a pagina dividida a esquerda
            temporaria->ponteiros[1] = direita_de_baixo;//o ponteiro na posicao 1 aponta para a pagina dividida a direita

            //ambas as paginas esquerda de baixo e direita de baixo vao apontar para seu pai, a nova raiz(temporaria)
            esquerda_de_baixo->pagina_anterior = temporaria;
            direita_de_baixo->pagina_anterior = temporaria;


            *raiz_da_arvore = temporaria;// a raiz da arvore agora sera uma nova raiz


            break;//posso sair do loop ja que inseri no topo
        }
        if (pagina_atual->quantidade_de_itens < QUANTIDADE_MAX_ITENS_PAGINA)//caso a pagina tenha espaco
        {
            posicao = b_posicao_que_deve_ser_inserido(pagina_atual, registro, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa);//posicao recebe a posicao que o registro deve ser inserido
            b_insere_na_pagina(pagina_atual, registro, posicao);//insiro na pagina atual o registro


            //ajeitar ponteiros
            //os ponteiros ao lado do registro inserido na pagina de cima deve apontar para as paginas divididas abaixo
            pagina_atual->ponteiros[posicao] = esquerda_de_baixo;
            pagina_atual->ponteiros[posicao + 1] = direita_de_baixo;

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
            direita = new TPagina;
            direita->pagina_anterior = pagina_atual->pagina_anterior;
            for (int item = 0; item <= QUANTIDADE_MAX_ITENS_PAGINA; item++)
            {
                direita->ponteiros[item] = NULL;
            }

            //qual posicao que se encaixa nosso registro
            posicao = b_posicao_que_deve_ser_inserido(pagina_atual, registro, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa);

            numero_de_comparacoes_chave_de_pesquisa[0]++;
            if (posicao < ORDEM)//vou inserir na esquerda
            {
                //novo registro sera, pois estara no meio
                registro_do_meio = pagina_atual->registros[ORDEM - 1];

                //removo os elementos da esquerda e digo o numero de elemento que estara na direita
                direita->quantidade_de_itens = ORDEM;
                esquerda->quantidade_de_itens = ORDEM - 1;

                for (int item = ORDEM, item2 = 0; item < QUANTIDADE_MAX_ITENS_PAGINA; item++, item2++)//insiro os elemetos na direita tanto os registros quanto os ponteiros
                {
                    direita->registros[item2] = esquerda->registros[item];
                    direita->ponteiros[item2] = esquerda->ponteiros[item];
                  
                    if (direita->ponteiros[item2] != NULL)
                    {
                        if (direita->ponteiros[item2]->pagina_anterior != NULL)
                        {
                            direita->ponteiros[item2]->pagina_anterior = direita;
                        }
                    }
                }
                direita->ponteiros[ORDEM] = esquerda->ponteiros[QUANTIDADE_MAX_ITENS_PAGINA];//nao esquecer do ultimo ponteiro que o for nao faz

                if (direita->ponteiros[ORDEM] != NULL)
                {
                    if (direita->ponteiros[ORDEM]->pagina_anterior != NULL)
                    {
                        direita->ponteiros[ORDEM]->pagina_anterior = direita;
                    }
                }
                //insiro na esquerda nosso registro e ajeito seus ponteiros apontando para pagina abaixo
                b_insere_na_pagina(esquerda, registro, posicao);
                esquerda->ponteiros[posicao] = esquerda_de_baixo;
                esquerda->ponteiros[posicao + 1] = direita_de_baixo;

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

                registro_do_meio = pagina_atual->registros[ORDEM]; //novo registro sera, pois estara no meio

                //removo os elementos da esquerda e digo o numero de elemento que estara na direita
                direita->quantidade_de_itens = ORDEM - 1;
                esquerda->quantidade_de_itens = ORDEM;

                //insiro os elemetos na direita tanto os registros quanto os ponteiros
                for (int item = ORDEM + 1, item2 = 0; item < QUANTIDADE_MAX_ITENS_PAGINA; item++, item2++)
                {
                    direita->registros[item2] = esquerda->registros[item];
                    direita->ponteiros[item2] = esquerda->ponteiros[item];
                    if (direita->ponteiros[item2] != NULL)
                    {
                        if (direita->ponteiros[item2]->pagina_anterior !=  NULL)
                        {
                            direita->ponteiros[item2]->pagina_anterior = direita;
                        }
                    }
                    
                    
                }
                direita->ponteiros[ORDEM - 1] = esquerda->ponteiros[QUANTIDADE_MAX_ITENS_PAGINA];//nao esquecer do ultimo ponteiro

                if (direita->ponteiros[ORDEM-1] != NULL)
                {
                    if (direita->ponteiros[ORDEM-1]->pagina_anterior != NULL)
                    {
                        direita->ponteiros[ORDEM-1]->pagina_anterior = direita;
                    }
                }

                posicao = b_posicao_que_deve_ser_inserido(direita, registro, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa);//posicao a ser inserido na direita, na esquerda nao tem essa parte por que a funcao anterior serve para ela mas aqui na direita uma nova pagina foi criada

                //insiro na direita nosso registro e ajeito seus ponteiros apontando para pagina abaixo
                b_insere_na_pagina(direita, registro, posicao);
                direita->ponteiros[posicao] = esquerda_de_baixo;
                direita->ponteiros[posicao + 1] = direita_de_baixo;

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
                direita->quantidade_de_itens = ORDEM;
                esquerda->quantidade_de_itens = ORDEM;

                //insiro os elemetos na direita tanto os registros quanto os ponteiros
                for (int item = ORDEM, item2 = 0; item < QUANTIDADE_MAX_ITENS_PAGINA; item++, item2++)
                {
                    direita->registros[item2] = esquerda->registros[item];
                    direita->ponteiros[item2 + 1] = esquerda->ponteiros[item + 1];
                    if (direita->ponteiros[item2+1] != NULL)
                    {
                        if (direita->ponteiros[item2+1]->pagina_anterior != NULL)
                        {
                            direita->ponteiros[item2+1]->pagina_anterior = direita;
                        }
                    }
                }

                direita->ponteiros[0] = direita_de_baixo; //o primeiro ponteiro da direita aponta para pagina direita de baixo
                esquerda->ponteiros[ORDEM] = esquerda_de_baixo;//o ultimo ponteiro da esquerda aponta para pagina esquerda de baixo 

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

int b_busca_na_arvore(TPagina* raiz, TRegistro* registro, int chave, long int numero_de_transferencia_para_memoria_interna[2], long int numero_de_comparacoes_chave_de_pesquisa[2])//funcao que dado uma arvore, um registro e uma chave, busca na arvore o registro com a chave especificada. Retorna 1 e o registro caso o registro seja encontrado, caso contr?rio retorna 0
{
    TPagina* pagina_atual = raiz;//a pagina atual inicialmente e a raiz

    int proxima_pagina;//recebe a posicao de um ponteiro para uma proxima pagina

    bool nao_achei_pagina_folha = true; //utilizado para achar pagina folha

    //registro a ser buscado
    TRegistro registro_a_ser_buscado;
    registro_a_ser_buscado.chave = chave;

    while (nao_achei_pagina_folha)//direciona para o no folha adequado ao registro e confere se o registro  esta na arvore
    {
        //conferir se elemento nao existe
        proxima_pagina = b_ponteiro_para_proxima_pagina(pagina_atual, registro_a_ser_buscado, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa, 'p');

        numero_de_comparacoes_chave_de_pesquisa[1]++;

        if (proxima_pagina > 0 && pagina_atual->registros[proxima_pagina - 1].chave == registro_a_ser_buscado.chave)//conferir se elemento j? existe
        {
            *registro = pagina_atual->registros[proxima_pagina - 1];
            return 1;//elemento j? existe retorna 1
        }

        if (pagina_atual->ponteiros[0] == NULL)//como a arvore e balanceada se tem um ponteiro inicial nulo chegamos em uma pagina folha
        {
            nao_achei_pagina_folha = false;
        }
        else
        {
            pagina_atual = pagina_atual->ponteiros[proxima_pagina];
        }
    }

    return 0;//retorna 0 caso nao encontre o elemento
}