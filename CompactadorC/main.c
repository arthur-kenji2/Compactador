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
    int capacidade;
    int primeiro;
    int ultimo;
    int qtd;
    HuffNode *noLetra;
} noLista;


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

void criarFila( struct noLista *f, int c )
{
	f->capacidade = c;
	f->noLetra = (HuffNode*) malloc (f->capacidade * sizeof(HuffNode));
	f->primeiro = 0;
	f->ultimo = -1;
	f->qtd = 0;
}

void erroArquivo()
{
    printf("Arquivo não encontrado\n");
    exit(0);

}

void insertionSort(struct noLista *f)
{
    int i, j, aux;
    char c, b;

    for(i = f->primeiro + 1; i <= f->ultimo; i++)
    {
        j = i;

        while((j != 0) && (f->noLetra[j - 1].freq > f->noLetra[j].freq))
        {
            aux = f->noLetra[j].freq;
            c   = f->noLetra[j].letra[0];
            b   = f->noLetra[j].letra[1];
            f->noLetra[j].freq  = f->noLetra[j - 1].freq;
            f->noLetra[j].letra[0] = f->noLetra[j - 1].letra[0];
            f->noLetra[j].letra[1] = f->noLetra[j - 1].letra[1];
            f->noLetra[j - 1].freq  = aux;
            f->noLetra[j - 1].letra[0] = c;
            f->noLetra[j - 1].letra[1] = b;
            j--;
        }
    }
}

void inserir( struct noLista *f, HuffNode v)
{
	if(f->ultimo == f->capacidade-1)
		f->ultimo = -1;

	f->ultimo++;
	f->noLetra[f->ultimo] = v;
	f->qtd++;
}

HuffNode remover (struct noLista *f)
{
    HuffNode ret;
    ret = f->noLetra[f->primeiro];
    f->primeiro++;
    f->qtd--;
    return ret;
}

int getQtd( struct noLista *f){
    return f->qtd;
}

int getPrimeiro (struct noLista *f)
{
    return f->primeiro;
}

int getUltimo (struct noLista *f)
{
    return f->ultimo;
}

HuffNode getNoLetra (struct noLista *f, int i)
{
    return f->noLetra[i];
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

int main()
{
    char c;
    unsigned listaBytes[256] = {0};
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

    struct noLista fila;

    criarFila(&fila, 256);

    for(int i = 0; i < qtd; i++)
    {
        inserir(&fila, noLetra[i]);
        listaBytes[i] = noLetra[i].freq;
    }


    HuffNode no1;
    HuffNode no2;

    HuffNode raiz;

    for(int i = 0; getQtd(&fila) != 1; i += 2)
    {
        HuffNode noArvore;
        noArvore.freq = 0;

        no1 = remover(&fila);
        no2 = remover(&fila);

        noArvore.letra[0] = '0';
        noArvore.letra[1] = '0';

        noArvore.freq += no1.freq;
        noArvore.freq += no2.freq;

        noArvore.esq = &no1;
        noArvore.dir = &no2;

        inserir(&fila, noArvore);

        insertionSort(&fila);

        raiz = noArvore;
    }

    printf("%d", raiz.freq);

    FILE *saida = fopen("saida.txt", "wb");
    (!saida) ? erroArquivo() : NULL == NULL ;

    fwrite(listaBytes, 256, sizeof(listaBytes[0]), saida);

    return 0;
}
