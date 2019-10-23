#include <stdio.h>
#include <stdlib.h>

int main()
{
    typedef struct
    {
        char letra;
        int freq;
    } LetraFreq;

    LetraFreq noLetra[255];


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
                if(noLetra[i].letra == NULL)
                {
                    noLetra[i].letra = c;
                    noLetra[i].freq  = 1;
                    break;
                }
                if(noLetra[i].letra == c)
                    noLetra[i].freq += 1;

            }
        fclose(file);
        }



    return 0;
}
