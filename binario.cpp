/*
1 - Definir a estrutura Registro
2 - Inicialização
3 - Inserção
4 - Inserção ordenada
5 - Remoção
6 - Pesquisa
7 - Impressão dos registros inseridos
8 - Impressão dos registros livre
9 - Impressão da estrutura
*/
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

/*definição da estrutura a ser armazenado no arquivo*/
struct dados
{
    int chave;
    char nome[30];
    // outros dados. Obs: não é recomendável usar tipo string dentro de union
};

union celula
{
    struct
    {
        int quant, first, last, free, tam; // tam armazena o tamanho total da lista
    } cabecalho;
    struct
    {
        int next, prev;
        dados reg;
    } lista;
};

void inicializar(fstream &arq, int n)
{
    /*Inicializando a estrutura*/

    cout << endl
         << endl
         << "Inicializando a estrutura de registros" << endl
         << endl;

    celula cab, l; // cab - acessar a union com dados do cabeçalho. l acessar a union com dados da lista

    /*Posicionando o ponteiro de escrita no início do arquivo*/
    arq.seekp(0, arq.beg);

    /*preenchendo o cabeçalho*/
    cab.cabecalho.quant = 0;
    cab.cabecalho.first = -1;
    cab.cabecalho.last = -1;
    cab.cabecalho.free = 1;
    cab.cabecalho.tam = n;

    /*Gravando o cabeçalho*/
    arq.write((char *)&cab, sizeof(cab));

    /*Gravando os n registros*/
    for (int i = 1; i <= cab.cabecalho.tam; i++)
    {
        if (i == n)
            l.lista.next = -1;
        else
            l.lista.next = i + 1;

        arq.write((char *)&l, sizeof(l));
    }
}

void imprimirEstrutura(fstream &arq)
{
    celula cab, l; // cab - acessar a union com dados do cabeçalho. l acessar a union com dados da lista

    cout << endl
         << endl
         << "Imprimindo a estrutura de registros" << endl;

    /*posicionando o ponteiro de leitura no início do arquivo*/
    arq.seekg(0, arq.beg);

    /*lendo o cabeçalho*/
    arq.read((char *)&cab, sizeof(cab));

    /*imprimindo o cabeçalho*/
    cout << "Cabecalho:\n";
    cout << "==========\n";
    cout << "Quant: " << cab.cabecalho.quant << endl;
    cout << "First: " << cab.cabecalho.first << endl;
    cout << "Last: " << cab.cabecalho.last << endl;
    cout << "Free: " << cab.cabecalho.free << endl;
    cout << "Tamanho: " << cab.cabecalho.tam << endl;

    /*lendo e imprimindo os n registros*/
    cout << "\n\nCelulas da lista:\n";
    cout << "=================\n\n";
    for (int i = 1; i <= cab.cabecalho.tam; i++)
    {
        cout << "Celula posicao " << i << ":\n";
        cout << "-------------------" << endl;
        arq.read((char *)&l, sizeof(l));
        cout << "Reg.chave: " << l.lista.reg.chave << endl;
        cout << "Reg.nome: " << l.lista.reg.nome << endl;
        cout << "Next: " << l.lista.next << endl;
        cout << "Prev: " << l.lista.prev << endl
             << endl;
    }
}

void imprimirLista(fstream &arq)
{
    celula cab, l; // cab - acessar a union com dados do cabeçalho. l acessar a union com dados da lista

    cout << endl
         << endl
         << "Imprimindo registros validos" << endl;

    /*posicionando o ponteiro de leitura no início do arquivo*/
    arq.seekg(0, arq.beg);

    /*lendo o cabeçalho*/
    arq.read((char *)&cab, sizeof(cab));

    /*imprimindo o cabeçalho*/
    cout << "Quantidade de itens: " << cab.cabecalho.quant << endl;

    /*lendo e imprimindo os registros com dados*/
    cout << "\nRegistros:\n\n";
    if (cab.cabecalho.first == -1)
    {
        cout << "Nao existe registros\n";
        return;
    }

    /*posiciona o ponteiro de leitura no primeiro registro apontado por first*/
    arq.seekg(cab.cabecalho.first * sizeof(cab), arq.beg);

    do
    {
        /*lendo o registro*/
        arq.read((char *)&l, sizeof(l));

        /*imprimindo o registro*/
        cout << "----------------------" << endl;
        cout << "Chave: " << l.lista.reg.chave << endl;
        cout << "Nome: " << l.lista.reg.nome << endl
             << endl;

        /*localizando o próximo registro*/
        if (l.lista.next != -1)
            arq.seekg(l.lista.next * sizeof(l), arq.beg);

    } while (l.lista.next != -1);
}

void inserir(fstream &arq, dados d)
{
    celula l, cab;
    int free;

    cout << endl
         << endl
         << "Inserindo a chave " << d.chave << endl;

    /*posiciona o ponteiro de leitura no início do arquivo*/
    arq.seekg(0, arq.beg);

    /*le o cabeçalho*/
    arq.read((char *)&cab, sizeof(cab));

    /*posiciona o ponteiro de leitura na primeira posição livre no arquivo*/
    arq.seekg(sizeof(cab) * cab.cabecalho.free, arq.beg);

    /*le as informações do registro livre*/
    arq.read((char *)&l, sizeof(l));

    /*grava em free o próximo registro livre*/
    free = l.lista.next;

    /*atribuindo as informações do novo registro*/
    l.lista.reg = d;
    l.lista.next = -1;
    l.lista.prev = cab.cabecalho.last;

    /*atualizando o next do registro que vai apontar para o novo registro l*/
    if (l.lista.prev != -1)
    {
        celula bob;

        /*posiciona o ponteiro de leitura para o registro que vai apontar para o novo registro l*/
        arq.seekg(sizeof(l) * l.lista.prev, arq.beg);

        /*lendo o registro*/
        arq.read((char *)&bob, sizeof(bob));

        /*atualizando o next*/
        bob.lista.next = cab.cabecalho.free;

        /*posiciona o ponteiro de escrita para o registro que vai apontar para o novo registro l*/
        arq.seekp(sizeof(l) * l.lista.prev, arq.beg);

        /*gravando o registro*/
        arq.write((char *)&bob, sizeof(bob));
    }

    /*posiciona o ponteiro de leitura para a posição do novo registro l*/
    arq.seekp(sizeof(cab) * cab.cabecalho.free, arq.beg);

    /*gravando o registro*/
    arq.write((char *)&l, sizeof(l));

    /*Atualizando o cabeçalho*/
    if (cab.cabecalho.first == -1)
        cab.cabecalho.first = cab.cabecalho.free;

    cab.cabecalho.quant++;
    cab.cabecalho.last = cab.cabecalho.free;
    cab.cabecalho.free = free;

    /*posiciona o ponteiro de escrita no início do arquivo*/
    arq.seekp(0, arq.beg);

    /*gravado o cabeçalho*/
    arq.write((char *)&cab, sizeof(cab));
}

void imprimirFree(fstream &arq)
{
    /*imprime as celulas livres no arquivo*/
    cout << "imprimirFree nao implementado!!!" << endl;
}

void inserirOrdenado(fstream &arq, dados d)
{
    /*insere o dado d na ordem crescente da chave*/
    cout << "inserirOrdenado nao implementado!!!" << endl;
}

bool pesquisa(fstream &arq, dados &d)
{
    celula l, cab;

    // Posiciona o ponteiro e lê o arquivo
    arq.seekg(0, arq.beg);
    arq.read((char *)&cab, sizeof(cab));

    if (cab.cabecalho.first == -1)
    {
        cout << "A lista esta vazia";
        return false;
    }

    // Posiciona o ponteiro na primeira l
    arq.seekg(cab.cabecalho.first * sizeof(cab), arq.beg);

    do
    {
        arq.read((char *)&l, sizeof(l));

        if (l.lista.reg.chave == d.chave)
        {
            d = l.lista.reg;
            return true;
        }

        if (l.lista.next != -1)
        {
            arq.seekg(l.lista.next * sizeof(l), arq.beg);
        }

    } while (l.lista.next != -1);

    return false;
}

bool remover(fstream &arq, dados &d)
{
    celula l, cab;

    // Coloca o ponteiro de leitura no inicio do arquivo
    arq.seekg(0, arq.beg);

    // Lê o cabeçalho
    arq.read((char *)&cab, sizeof(cab));

    int pos_atual = cab.cabecalho.first;

    if (cab.cabecalho.first == -1)
    {
        cout << "A lista esta vazia";
        return false;
    }

    // Posiciona o ponteiro de leitura no primeiro registro apontado como first
    arq.seekg(cab.cabecalho.first * sizeof(cab), arq.beg);

    do
    {
        // Lê a celula l
        arq.read((char *)&l, sizeof(l));

        if (l.lista.reg.chave == d.chave)
        {
            // Dado encontrado

            cab.cabecalho.quant--;

            if (l.lista.prev == -1) // verifica se o item removido é o primeiro da lista
            {
                cab.cabecalho.first = l.lista.next;

                if (l.lista.next == -1) // verifica se alista possui apenas 1 item
                {
                    cab.cabecalho.last = -1;
                }
                else
                {
                    // Cria uma célula auxiliar na memoria
                    celula prox;
                    // Desloca o ponteiro uma célula
                    arq.seekg(l.lista.next * sizeof(l), arq.beg);
                    // Realiza a leitura da célula auxiliar
                    arq.read((char *)&prox, sizeof(prox));
                    // Define na memória volatil o novo prev do novo primeiro termo
                    prox.lista.prev = -1;

                    // Corrige o ponteiro
                    arq.seekp(l.lista.next * sizeof(l), arq.beg);
                    // Grava na memória secundária
                    arq.write((char *)&prox, sizeof(prox));
                }

                arq.seekp(0, arq.beg);
                arq.write((char *)&cab, sizeof(cab));
            }

            else if (l.lista.next != -1) // verifica se o item removido está no meio da lista
            {
                celula prox, ant;

                // Desloca o ponteiro uma célula
                arq.seekg(l.lista.prev * sizeof(l), arq.beg);
                // Realiza a leitura da célula anterior
                arq.read((char *)&ant, sizeof(ant));

                // Desloca o ponteiro células
                arq.seekg(l.lista.next * sizeof(l), arq.beg);
                // Realiza a leitura da célula posterior
                arq.read((char *)&prox, sizeof(prox));

                ant.lista.next = l.lista.next;
                prox.lista.prev = l.lista.prev;

                // Corrige o ponteiro e salva no arquivo
                arq.seekp(l.lista.prev * sizeof(l), arq.beg);
                arq.write((char *)&ant, sizeof(ant));

                // Corrige o ponteiro e salva no arquivo
                arq.seekp(l.lista.next * sizeof(l), arq.beg);
                arq.write((char *)&prox, sizeof(prox));
            }

            else // item removido é o ultimo da lista
            {
                celula ant;

                // Desloca o ponteiro uma célula
                arq.seekg(l.lista.prev * sizeof(l), arq.beg);
                // Realiza a leitura da célula anterior
                arq.read((char *)&ant, sizeof(ant));

                cab.cabecalho.last = l.lista.prev;
                ant.lista.next = -1;

                // Corrige o ponteiro e salva no arquivo
                arq.seekp(0, arq.beg);
                arq.write((char *)&cab, sizeof(cab));

                // Corrige o ponteiro e salva no arquivo
                arq.seekp(l.lista.prev * sizeof(l), arq.beg);
                arq.write((char *)&ant, sizeof(ant));
            }

            l.lista.next = cab.cabecalho.free;
            cab.cabecalho.free = pos_atual;

            // Atualiza o cabecalho
            arq.seekp(0, arq.beg);
            arq.write((char *)&cab, sizeof(cab));

            // Atualiza a nova célula Free
            arq.seekp(pos_atual * sizeof(l), arq.beg);
            arq.write((char *)&l, sizeof(l));

            d = l.lista.reg;

            return true;
        }

        if (l.lista.next != -1)
        {
            arq.seekg(l.lista.next * sizeof(l), arq.beg);
            pos_atual = l.lista.next;
        }

    } while (l.lista.next != -1);

    return false;
}

int main()
{
    dados d;
    fstream arq;
    int n, op = -1;

    /*Abrindo o arquivo*/
    cout << "Abrindo arquivo pagina.dat ..." << endl;
    arq.open("pagina.dat", ios::binary | fstream::in | fstream::out);
    if (!arq.is_open())
    {
        cout << "Arquivo pagina.dat nao existe!" << endl;
        arq.open("pagina.dat", ios::binary | fstream::in | fstream::out | fstream::trunc);
        if (!arq.is_open())
        {
            cout << "Erro ao abrir o arquivo!!";
            return 0;
        }
        cout << "Criando arquivo pagina.dat ..." << endl;
        cout << "Digite o numero maximo de registros a serem armazenados no arquivo: ";
        cin >> n;
        inicializar(arq, n);
    }

    cout << "Arquivo pagina.dat aberto." << endl;

    /*Menu de opções*/
    while (op != 0)
    {
        cout << "\n\nMenu\n=================\n";
        cout << "1 - Insere" << endl;
        cout << "2 - Insere ordenado" << endl;
        cout << "3 - Remove" << endl;
        cout << "4 - Pesquisa" << endl;
        cout << "5 - Imprime Lista" << endl;
        cout << "6 - Imprime a Estrutura da lista" << endl;
        cout << "7 - Imprime a Lista Free" << endl;
        cout << "0 - Sair" << endl;
        cout << "Digite uma opcao: ";
        cin >> op;
        cout << endl;

        switch (op)
        {
        case 1: /*insere*/
            cout << "Digite a chave do registro a ser inserido: ";
            cin >> d.chave;
            cout << "Digite o nome: ";
            cin >> d.nome;
            inserir(arq, d);
            break;

        case 2: /*insere ordenado*/
            cout << "Digite a chave do registro a ser inserido: ";
            cin >> d.chave;
            cout << "Digite o nome: ";
            cin >> d.nome;
            inserirOrdenado(arq, d);
            break;

        case 3: /*remover*/
            cout << "Digite a chave a ser removida: ";
            cin >> d.chave;
            if (remover(arq, d))
            {
                cout << "\nRegistro removido:\n";
                cout << "Chave: " << d.chave << endl;
                cout << "Nome: " << d.nome << endl;
            }
            else
                cout << "\nRegistro com a chave " << d.chave << " nao encontrado.";
            break;

        case 4:
            cout << "Digite a chave a ser pesquisada: ";
            cin >> d.chave;
            if (pesquisa(arq, d))
            {
                cout << "\nRegistro encontrado:\n";
                cout << "Chave: " << d.chave << endl;
                cout << "Nome: " << d.nome << endl;
            }
            else
                cout << "\nRegistro com a chave " << d.chave << " nao encontrado.";
            break;

        case 5:
            imprimirLista(arq);
            break;

        case 6:
            imprimirEstrutura(arq);
            break;

        case 7:
            imprimirFree(arq);
            break;

        case 0:
            cout << "\n\nFinalizando o programa\n";
            break;

        default:
            cout << "Opcao Invalida!!!" << endl;
        }
    }

    return 0;
}
