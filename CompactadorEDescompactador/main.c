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

void Inserir(NoLista *n, Lista *l)
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

HuffNode *Remover(Lista *l)
{
    NoLista *no = l->head;
    HuffNode *noArv = no->dado;

    l->head = no->prox;

    free(no);
    no = NULL;

    l->tamanho--;

    return noArv;
}

HuffNode *CriarArvore(unsigned *lista)
{
    Lista l = {NULL, 0};

    for (int i = 0; i < 256; i++)
        if (lista[i])
            Inserir(NovoNoLista(NovoNo(i, lista[i], NULL, NULL)), &l);

    while (l.tamanho > 1)
    {
        HuffNode *esq = Remover(&l);
        HuffNode *dir = Remover(&l);

        HuffNode *soma = NovoNo('#', esq->freq + dir->freq, esq, dir);

        Inserir(NovoNoLista(soma), &l);
    }

    return Remover(&l);
}

bool PegarCodigo(HuffNode *no, byte c, char *buffer, int tamanho)
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
            encontrado = PegarCodigo(no->esq, c, buffer, tamanho + 1);
        }

        if (!encontrado && no->dir)
        {
            buffer[tamanho] = '1';
            encontrado = PegarCodigo(no->dir, c, buffer, tamanho + 1);
        }

        if (!encontrado)
            buffer[tamanho] = '\0';

        return encontrado;
    }
}

void Liberar(HuffNode *no)
{
    if (!no)
        return;
    else
    {
        HuffNode *esq = no->esq;
        HuffNode *dir= no->dir;
        free(no);

        Liberar(esq);
        Liberar(dir);
    }
}

int PrepararBit(FILE *entrada, int pos, byte *aux )
{
    (pos % 8 == 0) ? fread(aux, 1, 1, entrada) : NULL == NULL ;

    return !!((*aux) & (1 << (pos % 8)));
}

int CompactarArq()
{
    char arqEntrada[50];
    char arqSaida[50];
    unsigned listaFreqBytes[256] = {0};

    printf("\nPor favor, escreva o arquivo para ser compactado: ");
    scanf("%s", arqEntrada);

    FILE *entrada = fopen(arqEntrada, "rb");
    if (entrada == NULL)
        ErroArq();

    printf("\nPor favor, escreva o arquivo de saida no qual receberá o arquivo compactado: ");
    scanf("%s", arqSaida);

    byte b;
    while(fread(&b, 1, 1, entrada))
    {
        listaFreqBytes[(unsigned char)b]++;
    }

    rewind(entrada);

    HuffNode *raiz = CriarArvore(listaFreqBytes);

    FILE *saida = fopen(arqSaida, "wb");
    if (saida == NULL)
        ErroArq();

    fwrite(listaFreqBytes, 256, sizeof(listaFreqBytes[0]), saida);
    fseek(saida, sizeof(unsigned int), SEEK_CUR);

    unsigned char c;
    unsigned tamanho = 0;
    unsigned char aux = 0;

    while (fread(&c, 1, 1, entrada) >= 1)
    {
        char buffer[1024] = {0};
        PegarCodigo(raiz, c, buffer, 0);

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

    Liberar(raiz);

    fclose(entrada);
    fclose(saida);
}

int DescompactarArq()
{
    char arqEntrada[50];
    char arqSaida[50];
    unsigned listaFreqBytes[256] = {0};

    printf("\nPor favor, escreva o arquivo para ser decompactado: ");
    scanf("%s", arqEntrada);

    FILE *entrada = fopen(arqEntrada, "rb");
    if (entrada == NULL)
        ErroArq();

    printf("\nPor favor, escreva o arquivo de saida no qual receberá as informações do arquivo a ser descompactado: ");
    scanf("%s", arqSaida);

    FILE *saida = fopen(arqSaida, "wb");
    if (saida == NULL)
        ErroArq();

    fread(listaFreqBytes, 256, sizeof(listaFreqBytes[0]), entrada);

    HuffNode *raiz = CriarArvore(listaFreqBytes);

    unsigned tamanho;
    fread(&tamanho, 1, sizeof(tamanho), entrada);

    unsigned posicao = 0;
    unsigned char aux = 0;

    while (posicao < tamanho)
    {
        HuffNode *noAtual = raiz;

        while (noAtual->esq || noAtual->dir)
            noAtual = PrepararBit(entrada, posicao++, &aux) ? noAtual->dir : noAtual->esq;

        fwrite(&(noAtual->letra), 1, 1, saida);
    }

    fseek(entrada, 0L, SEEK_END);
    double tamanhoEntrada = ftell(entrada);

    fseek(saida, 0L, SEEK_END);
    double tamanhoSaida = ftell(saida);

    printf("\n\nArquivo de entrada: %s (%g bytes)\nArquivo de saida: %s (%g bytes)\n\n", arqEntrada, tamanhoEntrada / 1000, arqSaida, tamanhoSaida / 1000);

    Liberar(raiz);

    fclose(entrada);
    fclose(saida);
}

void ErroArq()
{
    printf("\nFalha para abrir arquivo\n");
    exit(0);
}

void Escolher()
{
    printf("Digite o numero da opção desejada: ");

    char opcao;
    scanf("%c", &opcao);

    switch (opcao)
    {
        case '1':
            CompactarArq();
            printf("\n\nDeseja continuar? (s/n) : ");
            scanf("%c", &opcao);
            opcao = getchar();
            if(opcao == 's')
                Escolher();
            else
                break;

        case '2':
            DescompactarArq();
            printf("\n\nDeseja continuar? (s/n) : ");
            scanf("%c", &opcao);
            opcao = getchar();
            if(opcao == 's')
                Escolher();
            else
                break;

        case '9':
            printf("\Saindo... \n\n");
            break;

        default:
            printf("\Valor inválido\n\n");
            break;
    }

}



int main()
{
    setlocale(LC_ALL, "Portuguese");
    printf("\nCompactador e Descompactador\n");

    printf("MENU: \n\n");
    printf("1 - Compactar Arquivo\n");
    printf("2 - Descompactar Arquivo\n");
    printf("9 - Sair\n\n");

    Escolher();

    return 0;
}
