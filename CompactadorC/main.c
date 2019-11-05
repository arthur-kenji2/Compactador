#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>

typedef unsigned char byte;

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

bool pegaCodigo(HuffNode *n, byte c, char *buffer, int tamanho)
{
    // Se o nó for folha e o seu valor for o buscado, colocar o caractere terminal no buffer e encerrar
    if (!(n->esq || n->dir) && n->letra[0] == c)
    {
        buffer[tamanho] = '\0';
        return true;
    }
    else
    {
        bool encontrado = false;

        // Se existir um nó à esquerda
        if (n->esq)
        {
            // Adicione '0' no bucket do buffer correspondente ao 'tamanho' nodeAtual
            buffer[tamanho] = '0';

            // fazer recursão no nó esquerdo
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



void criarFila( struct priorityQueue *f, int c )
{
	f->capacidade = c;
	f->noLetra = (HuffNode*) malloc (f->capacidade * sizeof(HuffNode));
	f->primeiro = 0;
	f->ultimo = -1;
	f->qtd = 0;

}

void insertionSort(struct priorityQueue *f)
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

void inserir( struct priorityQueue *f, HuffNode v)
{
	if(f->ultimo == f->capacidade-1)
		f->ultimo = -1;

	f->ultimo++;
	f->noLetra[f->ultimo] = v;
	f->qtd++;
}

void inserirNo(struct priorityQueue *f,  HuffNode no)
{

    /*
    if(f->qtd == 0)
    {
        f->qtd++;
        f->ultimo++;
        f->noLetra[f->ultimo] = no;
    }

    int i = 0;
    for(int j = 0; j < f->qtd; j++)
        if(no.freq < f->noLetra[j].freq)
        {
           i = j;
           break;
        }


    for(int j = f->ultimo + 1; j >= i; j--)
        f->noLetra[j] = f->noLetra[j - 1];



    f->primeiro = f->primeiro + 2;
    f->ultimo++;
    f->noLetra[i] = no;
    printf("%d \n", f->noLetra[--i].freq);
    */
}

HuffNode remover2( struct priorityQueue *f )
{

	HuffNode temp = f->noLetra[f->primeiro++];
	if(f->primeiro == f->capacidade)
		f->primeiro = 0;

	f->qtd--;
	return temp;

}

HuffNode remover (struct priorityQueue *f)
{
    HuffNode ret;
    ret = f->noLetra[f->primeiro];
    f->primeiro++;
    f->qtd--;
    return ret;
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



int estaVazia( struct priorityQueue *f )
{

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
    struct priorityQueue fila;

    criarFila(&fila, 256);

    for(int i = 0; i < qtd; i++)
        inserir(&fila, noLetra[i]);

    printf("\nTeste\n");
    for(int i = 0; i < getQtd(&fila); i++)
        printf("%d - %c - %d\n", i, getNoLetra(&fila, i).letra[0], getNoLetra(&fila, i).freq);

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

    FILE *saida = fopen("", "wb");
    (!saida) ? erroArquivo() : NULL == NULL ;

    byte b;

    while (fread(&b, 1, 1, file) >= 1)
    {
        // Cria um buffer vazio
        char buffer[1024] = {0};

        // Busca o código começando no nó 'raiz', utilizando o byte salvo em 'c', preenchendo 'buffer', desde o bucket deste último
        pegaCodigo(raiz, b, buffer, 0);

        // Laço que percorre o buffer
        for (char *i = buffer; *i; i++)
        {
            // Se o caractere na posição nodeAtual for '1'
            if (*i == '1')
            {
                // 2 elevado ao resto da divisão de 'tamanho' por 8
                // que é o mesmo que jogar um '1' na posição denotada por 'tamanho % 8'
                // aux = aux + pow(2, tamanho % 8);
                aux = aux | (1 << (tamanho % 8));
            }

            tamanho++;

            // Escreve byte no arquivo
            if (tamanho % 8 == 0)
            {
                fwrite(&aux, 1, 1, saida);
                // Zera a variável auxiliar
                aux = 0;
            }
        }
    }

    return 0;
}
