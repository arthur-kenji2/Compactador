#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

typedef struct
{
    char letra[2];
    int freq;
    struct HuffNode *esq, *dir;
} HuffNode;

typedef struct priorityQueue
{
    int capacidade;
    int primeiro;
    int ultimo;
    int qtd;
    HuffNode *noLetra;
};


void criarFila( struct priorityQueue *f, int c )
{

	f->capacidade = c;
	f->noLetra = (HuffNode*) malloc (f->capacidade * sizeof(HuffNode));
	f->primeiro = 0;
	f->ultimo = -1;
	f->qtd = 0;

}

void inserir( struct priorityQueue *f, HuffNode v)
{
	if(f->ultimo == f->capacidade-1)
		f->ultimo = -1;

	f->ultimo++;
	f->noLetra[f->ultimo] = v;
	f->qtd = f->qtd + 1;
}

void inserirNo( struct priorityQueue *f,  HuffNode no)
{
    if(estaVazia(&f))
    {
        f->qtd++;
        f->ultimo++;
        f->noLetra[f->ultimo] = no;
    }

    int i;
    for(int j = 0; j < f->qtd; j++)
        if(no.freq < f->noLetra[j].freq)
        {
           i = j;
           break;
        }


    for(int j = f->ultimo + 1; j > i; j--)
        f->noLetra[j] = f->noLetra[j - 1];

    f->noLetra[++i] = NULL;

    printf("%d\n", no.freq);

    f->primeiro = f->primeiro + 2;
    f->ultimo++;
    f->noLetra[++i] = no;
}

HuffNode remover( struct priorityQueue *f ) {

	HuffNode temp = f->noLetra[f->primeiro++];
	if(f->primeiro == f->capacidade)
		f->primeiro = 0;

	f->qtd--;
	return temp;

}

int getQtd( struct priorityQueue *f){
    return f->qtd;
}

int getPrimeiro (struct priorityQueue *f)
{
    return f->primeiro;
}

int getUltimo (struct priorityQueue *f)
{
    return f->ultimo;
}

HuffNode getNoLetra (struct priorityQueue *f, int i)
{
    return f->noLetra[i];
}



int estaVazia( struct priorityQueue *f ) {

	return (f->qtd==0);

}

int estaCheia( struct priorityQueue *f ) {

	return (f->qtd == f->capacidade);
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
        fclose(file);
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

     int freqTotal = 0;
    printf("\nOrdenado:\n");
        for(int i = 0; i < qtd; i++)
        {

            if(noLetra[i].letra[1] == 'p' || noLetra[i].letra[1] == 't')
            {
                printf("%c%c - ", noLetra[i].letra[0], noLetra[i].letra[1]);
                printf("%d\n", noLetra[i].freq);
                freqTotal += noLetra[i].freq;
            }
            else
            {
                printf("%c - ", noLetra[i].letra[0]);
                printf("%d\n", noLetra[i].freq);
                freqTotal += noLetra[i].freq;
            }
        }

        printf("\nfrequencia total:%d", freqTotal);

    HuffNode noArvore;
    noArvore.freq = 0;
    noArvore.letra = NULL;

    struct priorityQueue fila;

    criarFila(&fila, 256);

    for(int i = 0; i < qtd; i++)
        inserir(&fila, noLetra[i]);

    printf("\nTeste\n");
    for(int i = 0; i < getQtd(&fila); i++)
        printf("%c - %d\n", getNoLetra(&fila, i).letra[0], getNoLetra(&fila, i).freq);

    for(int i = 0; getQtd(&fila) != 1; i += 2)
    {
        HuffNode no1 = remover(&fila);
        HuffNode no2 = remover(&fila);

        noArvore.freq += no1.freq;
        noArvore.freq += no2.freq;

        noArvore.esq = &no1;
        noArvore.dir = &no2;

        inserirNo(&fila, noArvore);
    }

    printf("\nfrequencia total: %d", noArvore.freq);

    //free();

    return 0;
}
