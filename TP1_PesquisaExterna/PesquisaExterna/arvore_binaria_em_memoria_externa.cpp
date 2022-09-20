#include <iostream>
#include <chrono>
#include <stdlib.h>
#include <ctime>
#include "gera_registros.h"
#include "arvore_binaria_em_memoria_externa.h"

using namespace std::chrono;
using namespace std;

struct Registro1
{
    int chave;
    long int dado1;
    char dado2[500];
}; // estrutura do registro a ser retornado

struct RegistroB1
{
    int chave = -1;
    long int dado1 = 0;
    char dado2[500] = "\0";
};

struct RegistroB
{
    RegistroB1 registro;
    int filho_esquerda = -1;
    int filho_direita = -1;
};

int arvore_binaria(FILE* binario, int chave, long int numero_de_transferencia_para_memoria_interna[2],
    long int numero_de_comparacoes_chave_de_pesquisa[2], double tempo_de_execucao[2], TRegistro1& registro_retornado, long int& numero_de_comparacoes_chave_de_pesquisa_real, long int& numero_de_transferencia_para_memoria_interna_real, double& tempo_de_execucao_real)
{
    FILE* pesq;
    FILE* arvore;
    TRegistroB registro;
    TRegistroB1 registro1;//registro para auxiliar pesquisa

    //recebe arquivo de resgistros e confere se o arquivo n?o est? vazio
    if (fopen_s(&binario, "registros.bin", "rb") != 0)
    {
        //cout << "\nFalha ao abrir o arquivo binario para leitura de registros" << endl;
        return 0;
    }//abre arquivo binario para leitura em registros.bin


    if (abeme_arquivo_vazio(binario))//se o arquivo estiver vazio posso sair do programa pois nao h? registros para se buscar
    {
        fclose(binario);
        return 0;
    }

    if (fopen_s(&arvore,"arvore.bin", "wb+") != 0)
    {
        //cout << "\nFalha ao criar arquivo arvore.bin" << endl;
        fclose(binario);
        return 0;
    }//cria arquivo binario o qual armazenar? a arvore

    //tempo de criacao da arvore
    duration<double> time_span;
    duration<double> time_span2;

    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    if (cria_arvore(binario, arvore, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa) == 0)//confere se a ?rvore bin?ria foi criada com sucesso
    {
       // cout << "\nFalha ao criar a arvore binaria no arquivo arvore.bin !" << endl;
        fclose(binario);
        fclose(arvore);

        return 0;
    }

    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    time_span = duration_cast<duration<double>>(t2 - t1);

   // imprime_registros_em_arquivo_binario(arvore);//imprime registro dentro da arvore


    t1 = high_resolution_clock::now();//marcar tempo da pesquisa

   //confere se achou a chave
    if (pesquisa_na_arvore_binaria(arvore, &registro, chave, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa) == 1)
    {
        cout << "\nChave encontrada !\nRegistro\nChave: " << registro.registro.chave << "\nDado1: " << registro.registro.dado1 << "\nDado2: " << registro.registro.dado2 << endl;
        registro_retornado.chave = registro.registro.chave;
        registro_retornado.dado1 = registro.registro.dado1;
        registro_retornado.dado2[0] = registro.registro.dado2[0];
    }
    else
    {
        cout << "A chave nao pode ser encontrada" << endl;
    }

    t2 = high_resolution_clock::now();
    time_span2 = duration_cast<duration<double>>(t2 - t1);

    

    cout << "\nArvore Binaria em Memoria Externa concluida !" << endl;

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
        fread(&registro1, sizeof(TRegistroB1), 1, pesq);
        t1 = high_resolution_clock::now();//marcar tempo da pesquisa

        registro.registro = registro1;
        pesquisa_na_arvore_binaria(arvore, &registro, registro.registro.chave, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa);
        t2 = high_resolution_clock::now();
        time_span2 = duration_cast<duration<double>>(t2 - t1);
        tempo_de_execucao[1] += time_span2.count();
    }

    //fecha os arquivos binarios e termina o programa
    fclose(binario);
    fclose(pesq);
    fclose(arvore);
    return 1;
}
void imprime_registros_em_arquivo_binario(FILE* binario)//funcao que imprime na tela os registros de um arquivo binario
{
    TRegistroB registro;
    int status = ftell(binario);
    int i;
    rewind(binario);//mover para incio do arquivo

    for (i = 0; fread(&registro, sizeof(TRegistroB), 1, binario) == 1; i++)
    {
        cout << "\nRegistro " << i + 1 << endl;
        cout << "Chave: " << registro.registro.chave << endl;
        cout << "Dado1: " << registro.registro.dado1 << endl;
        //cout << "Dado2: " << registro.registro.dado2 << endl;
        cout << "Filho da esquerda: " << registro.filho_esquerda << endl;
        cout << "Filho da direita: " << registro.filho_direita << endl;
    }

    if (i == 0)
    {
        cout << "\nArquivo Vazio" << endl;
    }

    fseek(binario, status, 0);
}
int abeme_arquivo_vazio(FILE* binario)//funcao que verifica se um arquivo est? vazio
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

int insere_na_arvore(FILE* arvore, TRegistroB1 registro, long int numero_de_transferencia_para_memoria_interna[2], long int numero_de_comparacoes_chave_de_pesquisa[2])//insere um registro na arvore, ponteiro do arquivo arvore esta apontando para o comeco do arquivo
{

    TRegistroB vai_inserir;
    vai_inserir.registro = registro;

    TRegistroB nodo_pai;//registro que vai percorrer a arvore
    TRegistroB generico;//insere um registro generico ou seja NULL
    long int posicao_a_se_inserir;

    //inicialmente recebe a raiz
    if (fread(&nodo_pai, sizeof(TRegistroB), 1, arvore) != 1)
    {
        //cout << "\nFalha ao inserir na arvore\nFalha ao ler a raiz da arvore" << endl;
        return 0;
    }
    numero_de_transferencia_para_memoria_interna[0]++;

    //apos ler o primeiro registro tenho que conferir se a raiz est? vazia

    numero_de_comparacoes_chave_de_pesquisa[0]++;
    if (nodo_pai.registro.chave == -1)//raiz vazia
    {
        if (fseek(arvore, 0, 2) != 0)
        {
            //cout << "\nErro ao movimentar para o final do arquivo arvore.bin" << endl;
            return 0;
        }//movimentar para final do arquivo

        nodo_pai.filho_esquerda = ftell(arvore);//posicao do filho da esquerda no arquivo
        nodo_pai.filho_direita = nodo_pai.filho_esquerda + sizeof(TRegistroB);//posicao do filho da direita no arquivo

        //o resgistro a ser inserido na arvore precisa ter o endereco de seus filhos
        vai_inserir.filho_esquerda = nodo_pai.filho_esquerda;
        vai_inserir.filho_direita = nodo_pai.filho_direita;

        if (fseek(arvore, 0, 0) != 0)//movimenta-se para o inicio do arquivo pois ? nessa posicao que a raiz ser? inserida
        {
            //cout << "\nErro ao movimentar para inicio do arquivo arvore.bin" << endl;
            return 0;
        }

        if (fwrite(&vai_inserir, sizeof(TRegistroB), 1, arvore) != 1)///escrevo a raiz na arvore
        {
            //cout << "\nFalha ao inserir(raiz) na arvore" << endl;
            return 0;
        }

        //insere filhos como default ou seja como se fosse NULL, no caso insere-se os dois poss?veis filhos da raiz
        if (fseek(arvore, vai_inserir.filho_esquerda, 0) != 0)
        {
            //cout << "\nErro ao movimentar no arquivo arvore.bin" << endl;
            return 0;
        }

        if (fwrite(&generico, sizeof(TRegistroB), 1, arvore) != 1)
        {
            //cout << "\nFalha ao inserir na arvore" << endl;
            return 0;
        }

        if (fseek(arvore, vai_inserir.filho_direita, 0) != 0)
        {
            //cout << "\nErro ao movimentar no arquivo arvore.bin" << endl;
            return 0;
        }

        if (fwrite(&generico, sizeof(TRegistroB), 1, arvore) != 1)
        {
            //cout << "\nFalha ao inserir na arvore" << endl;
            return 0;
        }
    }
    else//raiz nao vazia
    {
        do//enquanto nao achar um nodo nulo ou seja com chave -1 eu nao posso parar de buscar para inserir, pois pode-se inserir numa arvore binaria somente abaixo do no folha, ou seja em um n? nulo
        {
            numero_de_comparacoes_chave_de_pesquisa[0]++;
            if (nodo_pai.registro.chave == -1)//cheguei no nodo a ser inserido, o n? nulo
            {
                posicao_a_se_inserir = ftell(arvore) - sizeof(TRegistroB);//guardamos a posicao a se inserir

                if (fseek(arvore, 0, 2) != 0)
                {
                    //cout << "\nErro ao movimentar para o final do arquivo arvore.bin" << endl;
                    return 0;
                }//movimentar para final do arquivo

                nodo_pai.filho_esquerda = ftell(arvore);//posicao do filho da esquerda no arquivo
                nodo_pai.filho_direita = nodo_pai.filho_esquerda + sizeof(TRegistroB);//posicao do filho da direita no arquivo

                //o resgistro a ser inserido na arvore precisa ter o endereco de seus filhos
                vai_inserir.filho_esquerda = nodo_pai.filho_esquerda;
                vai_inserir.filho_direita = nodo_pai.filho_direita;

                //devo substituir o nodo vazio pelo o registro requerido
                if (fseek(arvore, posicao_a_se_inserir, 0) != 0)//pula para posicao que deve-se inserir o no
                {
                    //cout << "\nErro ao movimentar para a posicao a se inserir no arquivo arvore.bin" << endl;
                    return 0;
                }

                if (fwrite(&vai_inserir, sizeof(TRegistroB), 1, arvore) != 1)///escrevo o nodo na arvore
                {
                    //cout << "\nFalha ao inserir(raiz) na arvore" << endl;
                    return 0;
                }

                //insere filhos como default ou seja como se fosse NULL, no caso estou inserindo os dois poss?veis filhos de um no
                if (fseek(arvore, vai_inserir.filho_esquerda, 0) != 0)
                {
                    //cout << "\nErro ao movimentar no arquivo arvore.bin" << endl;
                    return 0;
                }

                if (fwrite(&generico, sizeof(TRegistroB), 1, arvore) != 1)
                {
                    //cout << "\nFalha ao inserir na arvore" << endl;
                    return 0;
                }

                if (fseek(arvore, vai_inserir.filho_direita, 0) != 0)
                {
                    //cout << "\nErro ao movimentar no arquivo arvore.bin" << endl;
                    return 0;
                }

                if (fwrite(&generico, sizeof(TRegistroB), 1, arvore) != 1)
                {
                    //cout << "\nFalha ao inserir na arvore" << endl;
                    return 0;
                }

                break;//caso tenha inserido sem nenhum problema o programa sai do la?o
            }
            numero_de_comparacoes_chave_de_pesquisa[0]++;

            if (nodo_pai.registro.chave == vai_inserir.registro.chave)//se o registro da arvore for igual ao registro da chave que dizer que tem elemento repetido
            {
                //cout << "\nChave repetida " << endl;
                return 0;
            }

            else if (vai_inserir.registro.chave > nodo_pai.registro.chave)//se o registro for maior que um nodo deve-se ir para direita da arvore
            {
                numero_de_comparacoes_chave_de_pesquisa[0]++;
                if (fseek(arvore, nodo_pai.filho_direita, 0) != 0)//a partir da posciao inicial caminha-se para o registro da direita do nodo pai
                {
                    //cout << "\nErro ao movimentar em arvore.bin" << endl;
                    return 0;
                }

                numero_de_transferencia_para_memoria_interna[0]++;

                if (fread(&nodo_pai, sizeof(TRegistroB), 1, arvore) != 1)//lei o registro da direita
                {
                    //cout << "\nFalha ao ler nodo da arvore" << endl;
                    return 0;
                }
            }

            else//se o registro for menor que um nodo deve-se ir para esquerda da arvore
            {
                numero_de_comparacoes_chave_de_pesquisa[0]++;
                if (fseek(arvore, nodo_pai.filho_esquerda, 0) != 0)//a partir da posciao inial caminha-se para o registro da direita do nodo pai
                {
                    //cout << "\nErro ao movimentar em arvore.bin" << endl;
                    return 0;
                }

                numero_de_transferencia_para_memoria_interna[0]++;

                if (fread(&nodo_pai, sizeof(TRegistroB), 1, arvore) != 1)//lei o registro da esquerda
                {
                    //cout << "\nFalha ao ler nodo da arvore" << endl;
                    return 0;
                }
            }

        } while (true);
    }
    return 1;//caso tenha ocorrido tudo bem retorna o valor 1
}

int cria_arvore(FILE* binario, FILE* arvore, long int numero_de_transferencia_para_memoria_interna[2], long int numero_de_comparacoes_chave_de_pesquisa[2])//cria arvore binaria a partir de um arquivo de resgistros
{
    TRegistroB1 registro;

    //apontar para o come?o dos arquivos
    if (fseek(binario, 0, 0) != 0)
    {
        //cout << "\nErro ao movimentar para inicio do arquivo registros.bin" << endl;
        return 0;
    }

    if (fseek(arvore, 0, 0) != 0)
    {
        //cout << "\nErro ao movimentar para inicio do arquivo arvore.bin" << endl;
        return 0;
    }

    //iserir raiz na arvore com chave -1 o arquivo
    fwrite(&registro, sizeof(TRegistroB), 1, arvore);

    while ((fread(&registro, sizeof(TRegistroB1), 1, binario) == 1))//ler registro um por um de registro.bin e inserir na arvore
    {
        numero_de_transferencia_para_memoria_interna[0]++;
        if (fseek(arvore, 0, 0) != 0)//apontar sempre para come?o quando for inserir
        {
           // cout << "\nErro ao movimentar para inicio do arquivo arvore.bin" << endl;
            return 0;
        }

        if (insere_na_arvore(arvore, registro, numero_de_transferencia_para_memoria_interna, numero_de_comparacoes_chave_de_pesquisa) == 0)//caso algo d? errado na inser??o sai do loop retornando o valor 0
        {
            return 0;
        }
    }
    return 1;//caso d? tudo certo na cria??o da arvore ? retornado 1
}

int pesquisa_na_arvore_binaria(FILE* arvore, TRegistroB* registro, int chave, long int numero_de_transferencia_para_memoria_interna[2], long int numero_de_comparacoes_chave_de_pesquisa[2])//pesquisa uma chave na arvore e retorna o registro se encontrado
{
    //registros para pagina??o
    TRegistroB no_pai;
    TRegistroB filho_esquerda;
    TRegistroB filho_direita;

    if (abeme_arquivo_vazio(arvore))//se o arquivo estiver vazio posso sair do programa pois nao h? registros para se buscar
    {
        //cout << "\nN?o foi possivel buscar na arv?re pois ela esta vazia" << endl;
        return 0;
    }

    if (fseek(arvore, 0, 0) != 0)
    {
        //cout << "\nErro ao movimentar para o inicio do arquivo arvore.bin" << endl;
        return 0;
    }//movimentar para inicio do arquivo

    //ler a pagina

    if (fread(&no_pai, sizeof(TRegistroB), 1, arvore) != 1)//leio o no pai(ou seja a raiz)
    {
       //cout << "\nFalha ao buscar\nFalha ao ler a raiz";
        return 0;
    }
    numero_de_transferencia_para_memoria_interna[1]++;

    do
    {
        numero_de_comparacoes_chave_de_pesquisa[1]++;
        if (no_pai.registro.chave == -1)//chave nao encontrada
        {
            return 0;
        }

        else
        {
            numero_de_comparacoes_chave_de_pesquisa[1]++;
            if (chave == no_pai.registro.chave)//encontrei o registro
            {
                *registro = no_pai;
                break;
            }
            else if (chave > no_pai.registro.chave)// se a chave ? maior que o no pai irei procurar no filho da direita
            {
                numero_de_comparacoes_chave_de_pesquisa[1]++;

                if (fseek(arvore, no_pai.filho_direita, 0) != 0)
                {
                    //cout << "\nErro ao movimentar no arquivo arvore.bin" << endl;
                    return 0;
                }//movimentar para o filho a direita

                //ler direita


                if (fread(&no_pai, sizeof(TRegistroB), 1, arvore) != 1)//leio o filho da direita
                {
                    //cout << "\nFalha ao buscar";
                    return 0;
                }
                numero_de_transferencia_para_memoria_interna[1]++;
                
            }

            else//chave menor que a chave do n? pai, deve-se ir para esquerda
            {
                numero_de_comparacoes_chave_de_pesquisa[1]++;

                if (fseek(arvore, no_pai.filho_esquerda, 0) != 0)
                {
                    //cout << "\nErro ao movimentar no arquivo arvore.bin" << endl;
                    return 0;
                }//movimentar para o filho a esquerda

                //ler direita


                if (fread(&no_pai, sizeof(TRegistroB), 1, arvore) != 1)//leio o filho da esquerda
                {
                    //cout << "\nFalha ao buscar";
                    return 0;
                }

                numero_de_transferencia_para_memoria_interna[1]++;
            }
        }
    } while (true);
    return 1;
}