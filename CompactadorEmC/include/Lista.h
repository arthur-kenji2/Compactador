#ifndef LISTA_H
#define LISTA_H

typedef struct
{
    char letra[2];
    int freq;
    struct HuffNode *esq, *dir;
} HuffNode;

typedef struct noLista
{
    HuffNode *dado;
    HuffNode *prox;
} noLista;

extern void insertionSort(struct noLista *f);
extern void insereLista(struct noLista *f);

#endif // LISTA_H
