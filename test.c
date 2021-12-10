#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main() {

    printf("Alo!\n");

    unsigned int x;

    if (x < 0) {

        printf("é menor que zero\n");

    } else {
        
        printf("nao é menor que zero\n");
    }

    setvbuf(stdout, NULL, _IONBF, 0); 


    char *aux = calloc(1000, sizeof(char));

    char *str = calloc(15, sizeof(char));

    fgets(aux, 1000, stdin);
    printf("ENTRADA: %s\n", aux);
    int count  = 0;

    for(int i = 0; i < strlen(aux); i++) {
        str[count] = aux[i];
        count++;
        if(count == 15) {
            count = 0;
            printf("SAIDA: %s\n", str);
        }
    }

    str[count-1] = '\0';
    printf("SAIDA: %s\n", str);


    
    free(aux);
    free(str);


    return 0;
}