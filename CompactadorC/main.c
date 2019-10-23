#include <stdio.h>
#include <stdlib.h>
#define ' ' sp

int main()
{

    typedef struct
    {
        char letra;
        int freq;
    } LetraFreq;

    LetraFreq noLetra[255];
    int qtd = 0;


    for(int i=0; i<255; i++)
    {
        noLetra[i].letra = 0;
        noLetra[i].freq = 0;
    }


    char c;
    FILE *file;
    file = fopen("texto.txt", "r");
    if (file)
    {
        while ((c = getc(file)) != EOF)
            for(int i=0; i<255; i++)
            {
                if(noLetra[i].letra == 0)
                {
                    if(noLetra[i].letra == " ")
                    noLetra[i].letra = c;
                    noLetra[i].freq  = 1;
                    qtd++;
                    break;
                }

                if(noLetra[i].letra == c)
                {
                    noLetra[i].freq += 1;
                    break;
                }

            }
        fclose(file);
    }
    printf("Desordenado:\n");
    for(int i = 0; i < qtd; i++)
        for(int i = 0; i < qtd; i++)
        {
            printf("%c - ", noLetra[i].letra);
            printf("%d\n", noLetra[i].freq);
        }

    for (int i = 0; i < qtd; ++i)
        for (int j = i + 1; j < qtd; ++j)
            if (noLetra[i].freq > noLetra[j].freq)
            {
                int a =  noLetra[i].freq;
                char c = noLetra[i].letra;
                noLetra[i].freq = noLetra[j].freq;
                noLetra[i].letra = noLetra[j].letra;
                noLetra[j].freq = a;
                noLetra[j].letra = c;
            }

    printf("Ordenado:\n");
        for(int i = 0; i < qtd; i++)
        {
            printf("%c - ", noLetra[i].letra);
            printf("%d\n", noLetra[i].freq);
        }



    return 0;
}
