#include <stdio.h>
#include <stdlib.h>

int main()
{

    typedef struct
    {
        char letra[2];
        int freq;
    } LetraFreq;

    LetraFreq noLetra[255];
    int qtd = 0;


    for(int i=0; i<255; i++)
    {
        noLetra[i].letra[0] = '0';
        noLetra[i].letra[1] = '0';
        noLetra[i].freq = 0;
    }


    char c;
    FILE *file;
    file = fopen("texto.txt", "r");
    if (file)
    {
        printf("Desordenado: \n");
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
                    printf("%c - %d\n", noLetra[i].letra[0], noLetra[i].freq);
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
                noLetra[i].letra[0] = noLetra[j].letra[0];
                noLetra[j].freq = a;
                noLetra[j].letra[0] = c;
                noLetra[j].letra[1] = h;
            }

    printf("\nOrdenado:\n");
        for(int i = 0; i < qtd; i++)
        {
            printf("%c - ", noLetra[i].letra[0]);
            printf("%d\n", noLetra[i].freq);
        }





    return 0;
}
