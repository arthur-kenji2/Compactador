#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>

typedef unsigned char byte;

typedef struct HuffNode
{
    char letra[2];
    int freq;
    struct HuffNode *esq, *dir;
} HuffNode;

typedef struct noLista
{
    HuffNode *dado;
    struct noLista *prox;
} noLista;

typedef struct Lista
{
    noLista   *head;
    int       elementos;
}Lista;

noLista getHead(Lista *l)
{
    return *l->head;
}

void insereLista(noLista *n, Lista *l)
{
    if (!l->head)
    {
       l->head = &n;
   }
    else
        if (n->dado->freq < l->head->dado->freq)
    {
        n->prox = l->head;
        l->head = &n;
    }
    else
    {
        noLista *aux = l->head->prox;
        noLista *aux2 = l->head;

        while (aux && aux->dado->freq <= n->dado->freq)
        {
            aux2 = aux;
            aux = aux2->prox;
        }

        aux2->prox = &n;
        n->prox = &aux;
    }
    l->elementos++;
}

bool pegaCodigo(HuffNode *n, byte c, char *buffer, int tamanho)
{
    if (!(n->esq || n->dir) && n->letra[0] == c)
    {
        buffer[tamanho] = '\0';
        return true;
    }
    else
    {
        bool encontrado = false;

        if (n->esq)
        {
            buffer[tamanho] = '0';

            encontrado = pegaCodigo(n->esq, c, buffer, tamanho + 1);
        }

        if (!encontrado && n->dir)
        {
            buffer[tamanho] = '1';
            encontrado = pegaCodigo(n->dir, c, buffer, tamanho + 1);
        }
        if (!encontrado)
        {
            buffer[tamanho] = '\0';
        }

        return encontrado;
    }
}

void erroArquivo()
{
    printf("Arquivo não encontrado\n");
    exit(0);

}



void FreeHuffmanTree(HuffNode *n)
{
    if (!n)
        return;
    else
    {
        free(n);

        FreeHuffmanTree(&n->esq);
        FreeHuffmanTree(&n->dir);
    }
}

HuffNode *popMinLista(Lista *l)
{
    noLista *aux = l->head;
    HuffNode *aux2 = aux->dado;

    l->head = aux->prox;

    free(aux);
    aux = NULL;

    l->elementos--;

    return aux2;
}

int main()
{
    char c;

    HuffNode noLetra[255];
    int qtd = 0;

    setlocale(LC_ALL, "Portuguese");

    for(int i=0; i<255; i++)
    {
        noLetra[i].freq = 0;
        noLetra[i].letra[0] = '0';
        noLetra[i].letra[1] = '0';
    }

    FILE *file;
    file = fopen("texto.txt", "r");

    if (file)
    {
        while ((c = getc(file)) != EOF)
            for(int i=0; i<255; i++)
            {
                if(noLetra[i].letra[0] == '0')
                {
                    if(c == ' ')
                    {
                        noLetra[i].letra[0] = 's';
                        noLetra[i].letra[1] = 'p';
                    }
                    else
                        if(c == '\n')
                        {
                            noLetra[i].letra[0] = 'e';
                            noLetra[i].letra[1] = 't';
                        }
                        else
                            noLetra[i].letra[0] = c;
                    noLetra[i].freq  = 1;
                    qtd++;
                    break;
                }

                if(noLetra[i].letra[0] == c && noLetra[i].letra[1] == '0'
                   || noLetra[i].letra[0] == 's' && noLetra[i].letra[1] == 'p' && c == ' '
                   || noLetra[i].letra[0] == 'e' && noLetra[i].letra[1] == 't' && c == '\n')
                {
                    noLetra[i].freq += 1;
                    break;
                }

            }
        rewind(file);
    }

    for (int i = 0; i < qtd; ++i)
        for (int j = i + 1; j < qtd; ++j)
            if (noLetra[i].freq > noLetra[j].freq)
            {
                int a =  noLetra[i].freq;
                char c = noLetra[i].letra[0];
                char h = noLetra[i].letra[1];
                noLetra[i].freq = noLetra[j].freq;
                noLetra[i].letra[0] = noLetra[j].letra[0];
                noLetra[i].letra[1] = noLetra[j].letra[1];
                noLetra[j].freq = a;
                noLetra[j].letra[0] = c;
                noLetra[j].letra[1] = h;
            }

    Lista lista = {NULL, 0};

    for(int i = 0; i < qtd; i++)
    {
        noLista no;
        no.dado = &noLetra[i];
        no.prox = NULL;

        if (!lista.head)
            lista.head = &no;

        noLista *aux = lista.head->prox;
        noLista *aux2 = lista.head;

        while (aux && aux->dado->freq <= no.dado->freq)
        {
            aux2 = aux;
            aux = aux2->prox;
        }

        aux2->prox = &no;
        no.prox = aux;

        lista.elementos++;
    }

    while (lista.elementos > 1)
    {
        HuffNode *nodeEsquerdo = popMinLista(&lista);
        HuffNode *nodeDireito = popMinLista(&lista);

        HuffNode soma;
        soma.letra[0]= '#';
        soma.letra[1]= '#';
        soma.freq = nodeEsquerdo->freq + nodeDireito->freq;
        soma.esq = nodeEsquerdo;
        soma.dir = nodeDireito;

        noLista no;
        no.dado = &soma;
        no.prox = NULL;

        if (!lista.head)
        {
            lista.head = &no;
        }
        else if (no.dado->freq < lista.head->dado->freq)
        {
            no.prox = lista.head;
            lista.head = &no;
        }
        else
        {
            noLista *aux = lista.head->prox;
            noLista *aux2 = lista.head;

            while (aux && aux->dado->freq <= no.dado->freq)
            {
                aux2 = aux;
                aux = aux2->prox;
            }

            aux2->prox = &no;
            no.prox = &aux;
        }

        lista.elementos++;
    }

    return 0;
}
