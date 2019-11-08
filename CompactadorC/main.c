#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>

typedef unsigned char byte;

typedef struct HuffNode
{
    byte letra;
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
    noLista *head;
    int tamanho;
} Lista;

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

HuffNode *remover(Lista *l)
{
    noLista *no = l->head;
    HuffNode *noArv = no->dado;

    l->head = no->prox;

    free(no);
    no = NULL;

    l->tamanho--;

    return noArv;
}

void insereLista(noLista *n, Lista *l)
{
    if (!l->head)
        l->head = n;

    else if (n->dado->freq < l->head->dado->freq)
    {
        n->prox = l->head;
        l->head = n;
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

        aux2->prox = n;
        n->prox = aux;
    }

    l->tamanho++;
}

noLista *novoNoLista(HuffNode *noArv)
{
    noLista *novo;
    if ((novo = malloc(sizeof(*novo))) == NULL) return NULL;

    novo->dado = noArv;

    novo->prox = NULL;

    return novo;
}

HuffNode *novoNo(unsigned char c, int freq, HuffNode *esq, HuffNode *dir)
{
    HuffNode *novo;
    if ((novo = malloc(sizeof(*novo))) == NULL)return NULL;

    novo->letra = c;
    novo->freq = freq;
    novo->esq = esq;
    novo->dir = dir;

    return novo;
}

HuffNode *construirArvore(unsigned *l)
{
    Lista lista = {NULL, 0};

    for (int i = 0; i < 256; i++)
        if (l[i] == NULL)
            insereLista(novoNoLista(novoNo(i, l[i], NULL, NULL)), &lista);

    while (lista.tamanho > 1)
    {
        HuffNode *esq = remover(&lista);
        HuffNode *dir = remover(&lista);

        HuffNode *soma = novoNo('#', esq->freq + dir->freq, esq, dir);

        insereLista(novoNoLista(soma), &lista);
    }

    return remover(&lista);
}

int geraBit(FILE *arqEnt, int pos, unsigned char *aux )
{
    (pos % 8 == 0) ? fread(aux, 1, 1, arqEnt) : NULL == NULL ;

    return !!((*aux) & (1 << (pos % 8)));
}

bool codigoByte(HuffNode *no, unsigned char c, char *buffer, int tamanho)
{
    if (!(no->esq || no->dir) && no->letra == c)
    {
        buffer[tamanho] = '\0';
        return true;
    }
    else
    {
        bool encontrado = false;

        if (no->esq)
        {
            buffer[tamanho] = '0';
            encontrado = codigoByte(no->esq, c, buffer, tamanho + 1);
        }

        if (!encontrado && no->dir)
        {
            buffer[tamanho] = '1';
            encontrado = codigoByte(no->dir, c, buffer, tamanho + 1);
        }

        if (!encontrado)
            buffer[tamanho] = '\0';

        return encontrado;
    }
}

void desalocarArvHuffman(HuffNode *no)
{
    if (!no)
        return;
    else
    {
        HuffNode *esq = no->esq;
        HuffNode *dir= no->dir;
        free(no);

        desalocarArvHuffman(esq);
        desalocarArvHuffman(dir);
    }
}

void compactar()
{
    char arqEntrada[40];
    char arqSaida[40];

    printf("\n\nDigite o nome do arquivo que será compactado: ");
    scanf("%s", arqEntrada);

    FILE *entrada = fopen(arqEntrada, "rb");
    if(entrada == NULL)
        erroArquivo();

    printf("\n\nDigite o nome do arquivo em que o arquivo original sera compactado: ");
    scanf("%s", arqSaida);

    byte b;
    unsigned listaFreqBytes[256] = {0};

    while(fread(&b, 1, 1, entrada))
        listaFreqBytes[(byte)b]++;

    rewind(entrada);

    HuffNode *raiz = construirArvore(listaFreqBytes);

    FILE *saida = fopen(arqSaida, "wb");
    if(saida == NULL)
        erroArquivo();

    fwrite(listaFreqBytes, 256, sizeof(listaFreqBytes[0]), saida);
    fseek(saida, sizeof(unsigned int), SEEK_CUR);

    unsigned char c;
    unsigned tamanho = 0;
    unsigned char aux = 0;

    while (fread(&c, 1, 1, entrada) >= 1)
    {
        char buffer[1024] = {0};
        codigoByte(raiz, c, buffer, 0);

        for (char *i = buffer; *i; i++)
        {
            if (*i == '1')
                aux = aux | (1 << (tamanho % 8));

            tamanho++;

            if (tamanho % 8 == 0)
            {
                fwrite(&aux, 1, 1, saida);
                aux = 0;
            }
        }
    }

    fwrite(&aux, 1, 1, saida);
    fseek(saida, 256 * sizeof(unsigned int), SEEK_SET);
    fwrite(&tamanho, 1, sizeof(unsigned), saida);

    fseek(entrada, 0L, SEEK_END);
    double tamanhoEntrada = ftell(entrada);

    fseek(saida, 0L, SEEK_END);
    double tamanhoSaida = ftell(saida);

    printf("\n\nArquivo de entrada: %s (%g bytes)\nArquivo de saida: %s (%g bytes)\n\n", arqEntrada, tamanhoEntrada / 1000, arqSaida, tamanhoSaida / 1000);

    desalocarArvHuffman(raiz);

    fclose(saida);
    fclose(entrada);

}

int descompactar()
{
    FILE *entrada;
    char arqEntrada[40];
    char arqSaida[40];
    unsigned listaFreqBytes[256] = {0};

    printf("\nDigite o nome do arquivo que esta compactado: ");
    scanf("%s", arqEntrada);

    entrada = fopen(arqEntrada, "rb");
    if (entrada == NULL)
        erroArquivo();

    printf("\nDigite o nome do arquivo em que o arquivo original sera descompactado: ");
    scanf("%s", arqSaida);

    fread(listaFreqBytes, 256, sizeof(listaFreqBytes[0]), entrada);

    HuffNode *raiz = construirArvore(listaFreqBytes);

    unsigned tamanho;
    fread(&tamanho, 1, sizeof(tamanho), entrada);

    unsigned posicao = 0;
    unsigned char aux = 0;

    FILE *saida = fopen(arqSaida, "wb");
    if(saida == NULL)
        erroArquivo();

    while (posicao < tamanho)
    {
        HuffNode *noAtual = raiz;

        while (noAtual->esq || noAtual->dir)
            noAtual = geraBit(entrada, posicao++, &aux) ? noAtual->dir : noAtual->esq;

        fwrite(&(noAtual->letra), 1, 1, saida);
    }

    fseek(entrada, 0L, SEEK_END);
    double tamanhoEntrada = ftell(entrada);

    fseek(saida, 0L, SEEK_END);
    double tamanhoSaida = ftell(saida);

    printf("\n\nArquivo de entrada: %s (%g bytes)\nArquivo de saida: %s (%g bytes)\n", entrada, tamanhoEntrada / 1000, saida, tamanhoSaida / 1000);

    desalocarArvHuffman(raiz);

    fclose(saida);
    fclose(entrada);
}


int main()
{
    setlocale(LC_ALL, "Portuguese");
    printf("\nCompactador e Descompactador\n");

    printf("Operacao: \n\n");
    printf("1 - Compactar Arquivo\n");
    printf("2 - Descompactar Arquivo\n");
    printf("3 - Sair\n\n");

    printf("Digite sua opção : ");

    char opcao;
    scanf("%c", &opcao);

    switch (opcao)
    {
        case '1':
            compactar();
            break;

        case '2':
            descompactar();
            break;

        case '3':
            printf("\nEncerrando programa . . . \n\n");
            break;

        default:
            printf("\nOpção de operacao inválida\n\n");
            break;

        return 0;
    }
}
