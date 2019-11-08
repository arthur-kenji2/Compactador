#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>

typedef unsigned char byte;

typedef struct HuffNode
{
    int freq;
    byte letra;
    struct HuffNode *esq, *dir;
} HuffNode;

typedef struct NoLista
{
    HuffNode *dado;
    struct NoLista *prox;
} NoLista;

typedef struct Lista
{
    NoLista *head;
    int tamanho;
} Lista;

NoLista *NovoNoLista(HuffNode *no)
{
    NoLista *novo;
    if ((novo = malloc(sizeof(*novo))) == NULL) return NULL;

    novo->dado = no;
    novo->prox = NULL;

    return novo;
}

HuffNode *NovoNo(byte b, int freq, HuffNode *esq, HuffNode *dir)
{
    HuffNode *novo;
    if ((novo = malloc(sizeof(*novo))) == NULL)return NULL;

    novo->letra = b;
    novo->freq = freq;
    novo->esq = esq;
    novo->dir = dir;

    return novo;
}

void insereLista(NoLista *n, Lista *l)
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
        NoLista *aux = l->head->prox;
        NoLista *aux2 = l->head;

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

HuffNode *remover(Lista *l)
{
    NoLista *no = l->head;
    HuffNode *noArv = no->dado;

    l->head = no->prox;

    free(no);
    no = NULL;

    l->tamanho--;

    return noArv;
}

HuffNode *construirArvore(unsigned *lista)
{
    Lista l = {NULL, 0};

    for (int i = 0; i < 256; i++)
        if (lista[i])
            insereLista(NovoNoLista(NovoNo(i, lista[i], NULL, NULL)), &l);

    while (l.tamanho > 1)
    {
        HuffNode *esq = remover(&l);
        HuffNode *dir = remover(&l);

        HuffNode *soma = NovoNo('#', esq->freq + dir->freq, esq, dir);

        insereLista(NovoNoLista(soma), &l);
    }

    return remover(&l);
}

bool getCodigo(HuffNode *no, byte c, char *buffer, int tamanho)
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
            encontrado = getCodigo(no->esq, c, buffer, tamanho + 1);
        }

        if (!encontrado && no->dir)
        {
            buffer[tamanho] = '1';
            encontrado = getCodigo(no->dir, c, buffer, tamanho + 1);
        }

        if (!encontrado)
            buffer[tamanho] = '\0';

        return encontrado;
    }
}

void freeArvore(HuffNode *no)
{
    if (!no)
        return;
    else
    {
        HuffNode *esq = no->esq;
        HuffNode *dir= no->dir;
        free(no);

        freeArvore(esq);
        freeArvore(dir);
    }
}

int geraBit(FILE *entrada, int pos, byte *aux )
{
    (pos % 8 == 0) ? fread(aux, 1, 1, entrada) : NULL == NULL ;

    return !!((*aux) & (1 << (pos % 8)));
}

int compactar()
{
    char arqEntrada[50];
    char arqSaida[50];
    unsigned listaFreqBytes[256] = {0};

    printf("\nDigite o nome do arquivo que sera compactado: ");
    scanf("%s", arqEntrada);

    FILE *entrada = fopen(arqEntrada, "rb");
    if (entrada == NULL)
        erroArquivo();

    printf("\nDigite o nome do arquivo em que o arquivo original sera compactado: ");
    scanf("%s", arqSaida);

    byte b;
    while(fread(&b, 1, 1, entrada))
    {
        listaFreqBytes[(unsigned char)b]++;
    }

    rewind(entrada);

    HuffNode *raiz = construirArvore(listaFreqBytes);

    FILE *saida = fopen(arqSaida, "wb");
    if (saida == NULL)
        erroArquivo();

    fwrite(listaFreqBytes, 256, sizeof(listaFreqBytes[0]), saida);
    fseek(saida, sizeof(unsigned int), SEEK_CUR);

    unsigned char c;
    unsigned tamanho = 0;
    unsigned char aux = 0;

    while (fread(&c, 1, 1, entrada) >= 1)
    {
        char buffer[1024] = {0};
        getCodigo(raiz, c, buffer, 0);

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

    printf("\n\nArquivo de entrada: %s (%g bytes)\nArquivo de saida: %s (%g bytes)", arqEntrada, tamanhoEntrada / 1000, arqSaida, tamanhoSaida / 1000);

    freeArvore(raiz);

    fclose(entrada);
    fclose(saida);
}

int descompactar()
{
    char arqEntrada[50];
    char arqSaida[50];
    unsigned listaFreqBytes[256] = {0};

    printf("\nDigite o nome do arquivo que esta compactado : ");
    scanf("%s", arqEntrada);

    FILE *entrada = fopen(arqEntrada, "rb");
    if (entrada == NULL)
        erroArquivo();

    printf("\nDigite o nome do arquivo em que o arquivo original sera descompactado : ");
    scanf("%s", arqSaida);

    FILE *saida = fopen(arqSaida, "wb");
    if (saida == NULL)
        erroArquivo();

    fread(listaFreqBytes, 256, sizeof(listaFreqBytes[0]), entrada);

    HuffNode *raiz = construirArvore(listaFreqBytes);

    unsigned tamanho;
    fread(&tamanho, 1, sizeof(tamanho), entrada);

    unsigned posicao = 0;
    unsigned char aux = 0;

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

    printf("\n\nArquivo de entrada: %s (%g bytes)\nArquivo de saida: %s (%g bytes)\n\n", arqEntrada, tamanhoEntrada / 1000, arqSaida, tamanhoSaida / 1000);

    freeArvore(raiz);

    fclose(entrada);
    fclose(saida);
}

void erroArquivo()
{
    printf("\nProblemas com a abertura do arquivo\n");
    exit(0);
}

int main()
{
    setlocale(LC_ALL, "Portuguese");
    printf("\nCompactador e Descompactador\n");

    printf("Operacões: \n\n");
    printf("1 - Compactar Arquivo\n");
    printf("2 - Descompactar Arquivo\n");
    printf("9 - Sair\n\n");

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

        case '9':
            printf("\nEncerrando programa . . . \n\n");
            break;

        default:
            printf("\nOpção de operacao inválida\n\n");
            break;
    }

    return 0;
}
