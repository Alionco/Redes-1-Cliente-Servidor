#include "comandos.h"
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

int ehEdit;
int ehLinhas;
int linhaEdit;

char nomeArq[MAX];

int geraParidade(pacote_t *mensagemSend) {

    int par, temp;

    if(mensagemSend->dados[0] == '\0') {
        temp = 0;
    } else {
        if(strlen(mensagemSend->dados) == 1) {
            temp = mensagemSend->dados[0];
        } else {
            for(int i = 0; i < strlen(mensagemSend->dados)-1; i++) {
                temp = mensagemSend->dados[i] ^ mensagemSend->dados[i+1];
            }
        }
    }

    par = mensagemSend->tamanho ^ mensagemSend->sequencia ^ mensagemSend->tipo ^ temp;

    return par;
}

int verificaParidade(pacote_t *mensagemReceive) {

    int par, temp;

    if(mensagemReceive->dados[0] == '\0') {
        temp = 0;
    } else {
        if(strlen(mensagemReceive->dados) == 1) {
            temp = mensagemReceive->dados[0];
        } else {
            for(int i = 0; i < strlen(mensagemReceive->dados)-1; i++) {
                temp = mensagemReceive->dados[i] ^ mensagemReceive->dados[i+1];
            }
        }
    }

    par = mensagemReceive->tamanho ^ mensagemReceive->sequencia ^ mensagemReceive->tipo ^ temp;

    return par ^ mensagemReceive->paridade;
}

int montaMensagemCD(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos) {

    mensagemSend->marcadorInicio = MARCADOR_INICIO;
    mensagemSend->enderecoDestino = END_SERVIDOR;
    mensagemSend->enderecoOrigem = END_USUARIO;
    mensagemSend->tamanho = sizeof(mensagemSend->dados);
    mensagemSend->sequencia = sequencia;
    mensagemSend->tipo = CD;
    strcpy(mensagemSend->dados, argumentos);
    mensagemSend->paridade = geraParidade(mensagemSend);

    return 0;
}

int executaComandoCD(pacote_t *mensagemReceive) {
    return chdir(mensagemReceive->dados);
}

int executaComandoLCD(char *argumentos) {
    return chdir(argumentos);
}

int montaMensagemLS(pacote_t *mensagemSend, uint8_t sequencia) {
    
    mensagemSend->marcadorInicio = MARCADOR_INICIO;
    mensagemSend->enderecoDestino = END_SERVIDOR;
    mensagemSend->enderecoOrigem = END_USUARIO;
    mensagemSend->tamanho = sizeof(mensagemSend->dados);
    mensagemSend->sequencia = sequencia;
    mensagemSend->tipo = LS;
    strcpy(mensagemSend->dados, "\0");
    mensagemSend->paridade = geraParidade(mensagemSend);
    return 0;
}

int executaComandoLS(char* buffer) {

    char *aux1 = calloc(MAX, sizeof(char));
    char *aux2 = calloc(MAX, sizeof(char));
    char *aux3 = calloc(MAX, sizeof(char));

    FILE *fp;
    fp = popen("ls", "r");

    while (fgets(aux1, 1000, fp) != NULL) {
        strcat(aux2, strcat(aux1, " "));
    }

    // retira os '\n'
    int j = 0;
    for(int i = 0; i < strlen(aux2); i++) {
        if(aux2[i] != '\n') {
            aux3[j] = aux2[i];
            j++;
        }
    }
    strcpy(buffer, aux3);

    free(aux1);
    free(aux2);
    free(aux3);

    return 0;
}

int executaComandoLLS(char* buffer) {

    char *aux1 = calloc(MAX, sizeof(char));
    char *aux2 = calloc(MAX, sizeof(char));
    char *aux3 = calloc(MAX, sizeof(char));

    FILE *fp;
    fp = popen("ls", "r");


    while (fgets(aux1, 1000, fp) != NULL) {
        strcat(aux2, strcat(aux1, " "));
    }

    // retira os '\n'
    int j = 0;
    for(int i = 0; i < strlen(aux2); i++) {
        if(aux2[i] != '\n') {
            aux3[j] = aux2[i];
            j++;
        }
    }
    strcpy(buffer, aux3);

    pclose(fp);
    aux1 = NULL;
    aux2 = NULL;
    aux3 = NULL;

    free(aux1);
    free(aux2);
    free(aux3);
    return 0;
}

int montaMensagemVER(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos){     

    mensagemSend->marcadorInicio = MARCADOR_INICIO;
    mensagemSend->enderecoDestino = END_SERVIDOR;
    mensagemSend->enderecoOrigem = END_USUARIO;
    mensagemSend->tamanho = sizeof(mensagemSend->dados);
    mensagemSend->sequencia = sequencia;
    mensagemSend->tipo = VER;
    strcpy(mensagemSend->dados, argumentos);
    mensagemSend->paridade = geraParidade(mensagemSend);

    return 0;
}

int executaComandoVER(pacote_t *mensagemReceive, char *buffer) {

    FILE *arq;
    arq = fopen(mensagemReceive->dados, "r");
    if(arq == NULL) {
        return -1;
    }

    fseek(arq, 0L, SEEK_END);
    unsigned long int tam_arq = ftell(arq);
    fclose(arq);

    char *aux = calloc(tam_arq, sizeof(char));
    char *cmd = calloc(MAX, sizeof(char));
    sprintf(cmd, "cat -n '%s'", mensagemReceive->dados);

    FILE *fp;
    fp = popen(cmd, "r");

    fgets(aux, tam_arq, fp);

    while(!feof(fp)) { 
        strcat(buffer, aux);
        fgets(aux, tam_arq, fp);
    }

/*     do {
        fgets(aux, tam_arq, fp);
        strcat(buffer, aux);

    } while(!feof(fp)); */


    pclose(fp);
    free(aux);
    free(cmd);
    return 0;
}

int montaMensagemLINHA(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos) {
        
    char *aux = calloc(MAX, sizeof(char));
    char *aux2 = calloc(MAX, sizeof(char));
    
    sscanf(argumentos,"%s%s", aux, aux2);
    aux2[strlen(aux2)] = '\0'; // so o segundo argumento import aqui


    mensagemSend->marcadorInicio = MARCADOR_INICIO;
    mensagemSend->enderecoDestino = END_SERVIDOR;
    mensagemSend->enderecoOrigem = END_USUARIO;
    mensagemSend->tamanho = sizeof(mensagemSend->dados);
    mensagemSend->sequencia = sequencia;
    mensagemSend->tipo = LINHA;
    strcpy(mensagemSend->dados, aux2);
    mensagemSend->paridade = geraParidade(mensagemSend);

    free(aux);
    free(aux2);

    return 0;
}

int executaComandoLINHA(pacote_t *mensagemReceive) {

    FILE* arq;
    arq = fopen(mensagemReceive->dados, "r");
    if(arq == NULL) {
        return -1;
    }
    strcpy(nomeArq,mensagemReceive->dados);
    fclose(arq);
    return 0; 
}

int montaMensagemLINHAS(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos) {

    char *aux1 = calloc(MAX, sizeof(char));
    char *aux2 = calloc(MAX, sizeof(char));
    char *aux3 = calloc(MAX, sizeof(char));

    sscanf(argumentos,"%s%s%s", aux1, aux2, aux3);
    aux3[strlen(aux3)] = '\0';  

    mensagemSend->marcadorInicio = MARCADOR_INICIO;
    mensagemSend->enderecoDestino = END_SERVIDOR;
    mensagemSend->enderecoOrigem = END_USUARIO;
    mensagemSend->tamanho = sizeof(mensagemSend->dados);
    mensagemSend->sequencia = sequencia;
    mensagemSend->tipo = LINHAS;
    if(aux3[0] == '\0') {
        strcpy(mensagemSend->dados, aux2);
    } else {
        strcpy(mensagemSend->dados, aux3);
    }
    mensagemSend->paridade = geraParidade(mensagemSend);

    free(aux1);
    free(aux2);
    free(aux3);

    return 0;
}

int executaComandoLINHAS(pacote_t *mensagemReceive) {

    FILE* arq;
    arq = fopen(mensagemReceive->dados, "r");
    if(arq == NULL) {
        return -1;
    }
    strcpy(nomeArq,mensagemReceive->dados);
    ehLinhas = 1;
    fclose(arq);
    return 0; 
}

int montaMensagemLINHAS_INI_FIM(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos) {

    char *aux1 = calloc(MAX, sizeof(char));
    char *aux2 = calloc(MAX, sizeof(char));

    switch(mensagemSend->tipo) {
        case(LINHA):
            sscanf(argumentos, "%s", aux1);
            aux1[strlen(aux1)] = '\0';
            
            break;
        case(EDIT):
            sscanf(argumentos, "%s", aux1);
            aux1[strlen(aux1)] = '\0';
            
            break;
        case(LINHAS):

            sscanf(argumentos, "%s%s", aux1, aux2);


            int flag = 0;
            for(int i = 0; i < strlen(aux2); i++) {
                if(!isdigit(aux2[i])) {
                    flag = 1;
                    break;
                }
            }
            if(flag == 0) {
                strcat(strcat(aux1," "),aux2);
            }

            aux1[strlen(aux1)] = '\0';

            break;
        default:
            break;
    }

    mensagemSend->marcadorInicio = MARCADOR_INICIO;
    mensagemSend->enderecoDestino = END_SERVIDOR;
    mensagemSend->enderecoOrigem = END_USUARIO;
    mensagemSend->tamanho = sizeof(mensagemSend->dados);
    mensagemSend->sequencia = sequencia;
    mensagemSend->tipo = LINHAS_INI_FIM;
    strcpy(mensagemSend->dados, aux1);
    mensagemSend->paridade = geraParidade(mensagemSend);

    free(aux1);
    free(aux2);

    return 0;
}

int executaComandoLINHAS_INI_FIM(pacote_t *mensagemReceive, char *buffer) {
    
    char *aux1 = calloc(1000, sizeof(char));
    char *aux2 = calloc(1000, sizeof(char));

    sscanf(mensagemReceive->dados, "%s%s", aux1, aux2);

    if(aux2[0] == '\0') { // é comando LINHA, so tem um parametro e o aux2 é vazio

        if(ehEdit == 1) { // o que veio antes era um EDIT

            linhaEdit = atoi(aux1); // salva a referencia da linha que tem q ser editada
            free(aux1);
            free(aux2);
            return 1; 
        } else if(ehLinhas == 1) { // comando eh linhas mas so tem a linha de começo

            ehLinhas = 0;
            char *aux = calloc(MAX, sizeof(char));
            char *cmd = calloc(MAX, sizeof(char));

            sprintf(cmd, "cat -n %s | sed -n ''%d',$p'", nomeArq, atoi(aux1));

            FILE *fp;
            fp = popen(cmd, "r");

            fgets(aux, MAX, fp);

            while(!feof(fp)) {
                strcat(buffer, aux);
                fgets(aux, MAX, fp);
            }

            pclose(fp);
            free(aux1);
            free(aux2);
            free(aux);
            free(cmd);
            return 0;

        }

        char *aux = calloc(MAX, sizeof(char));
        char *cmd = calloc(MAX, sizeof(char));

        sprintf(cmd, "cat -n %s | sed -n ''%d'p'", nomeArq, atoi(aux1));

        FILE *fp;
        fp = popen(cmd, "r");

        fgets(aux, MAX, fp);

        while(!feof(fp)) {
            strcat(buffer, aux);
            fgets(aux, MAX, fp);
        }

        pclose(fp);
        free(aux1);
        free(aux2);
        free(aux);
        free(cmd);
        return 0;

    }

    // caso seja LINHAS com dois argumentos

    char *aux = calloc(MAX, sizeof(char));
    char *cmd = calloc(MAX, sizeof(char));

    sprintf(cmd, "cat -n %s | sed -n ''%d','%d'p'", nomeArq, atoi(aux1), atoi(aux2));

    FILE *fp;
    fp = popen(cmd, "r");

    fgets(aux, MAX, fp);

    while(!feof(fp)) {
        strcat(buffer, aux);
        fgets(aux, MAX, fp);
    }

    pclose(fp);
    free(aux1);
    free(aux2);
    free(aux);
    free(cmd);
    return 0;
}

int montaMensagemEDIT(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos) {

    char *aux = calloc(MAX, sizeof(char));
    char *aux2 = calloc(MAX, sizeof(char));

    sscanf(argumentos,"%s%s", aux, aux2);
    aux2[strlen(aux2)] = '\0'; // so o segundo argumento importa aqui 

    mensagemSend->marcadorInicio = MARCADOR_INICIO;
    mensagemSend->enderecoDestino = END_SERVIDOR;
    mensagemSend->enderecoOrigem = END_USUARIO;
    mensagemSend->tamanho = sizeof(mensagemSend->dados);
    mensagemSend->sequencia = sequencia;
    mensagemSend->tipo = EDIT;
    strcpy(mensagemSend->dados, aux2);
    mensagemSend->paridade = geraParidade(mensagemSend);

    free(aux);
    free(aux2);

    return 0;
}

int executaComandoEDIT(pacote_t *mensagemReceive) {

    FILE* arq;
    arq = fopen(mensagemReceive->dados, "r");
    if(arq == NULL) {
        return -1;
    }
    strcpy(nomeArq,mensagemReceive->dados);
    ehEdit = 1;
    fclose(arq);
    return 0; 
}

int montaMensagemCOMPILAR(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos) {
    
    mensagemSend->marcadorInicio = MARCADOR_INICIO;
    mensagemSend->enderecoDestino = END_SERVIDOR;
    mensagemSend->enderecoOrigem = END_USUARIO;
    mensagemSend->tamanho = sizeof(mensagemSend->dados);
    mensagemSend->sequencia = sequencia;
    mensagemSend->tipo = COMPILAR;
    strcpy(mensagemSend->dados, argumentos);
    mensagemSend->paridade = geraParidade(mensagemSend);

    return 0;
}

int executaComandoCOMPILAR(pacote_t *mensagemReceive, char *buffer) {

    char *aux1 = calloc(MAX, sizeof(char));
    char *aux2 = calloc(MAX, sizeof(char));

    sscanf(mensagemReceive->dados, "%s%s", aux1, aux2);

    if(aux2[0] == '\0') {

        // so tem o nome do arq no aux1

        FILE *arq;
        arq = fopen(aux1, "r");
        if(arq == NULL) {
            return -1;
        }
        fclose(arq);

        FILE* fp;
        char *cmd = calloc(MAX, sizeof(char));
        sprintf(cmd, "gcc '%s' -o saida 2>&1 | cat", aux1);

        fp = popen(cmd, "r");

        char aux[MAX];

        fgets(aux, MAX, fp);

        while(!feof(fp)) {
            strcat(buffer, aux);
            fgets(aux, MAX, fp);
        }

        pclose(fp);
        free(cmd);
        return 0; 

    }

    // tem nome e opcao, opcao no aux1

    FILE *arq;
    arq = fopen(aux2, "r");
    if(arq == NULL) {
        return -1;
    }
    fclose(arq);

    FILE* fp;
    char *cmd = calloc(MAX, sizeof(char));
    sprintf(cmd, "gcc '%s' '%s' -o saida 2>&1 | cat", aux1, aux2);

    fp = popen(cmd, "r");

    char *aux = calloc(MAX, sizeof(char));

    fgets(aux, MAX, fp);

    while(!feof(fp)) {
        strcat(buffer, aux);
        fgets(aux, MAX, fp);
    }

    pclose(fp);
    free(cmd);
    free(aux);
    return 0;
}

int montaMensagemCONTEUDO_ARQUIVO(pacote_t *mensagemSend, uint8_t sequencia, char *conteudo) {
     
    mensagemSend->marcadorInicio = MARCADOR_INICIO;
    mensagemSend->enderecoDestino = END_SERVIDOR;
    mensagemSend->enderecoOrigem = END_USUARIO;
    mensagemSend->tamanho = sizeof(mensagemSend->dados);
    mensagemSend->sequencia = sequencia;
    mensagemSend->tipo = CONTEUDO_ARQUIVO;
    strcpy(mensagemSend->dados, conteudo);
    mensagemSend->paridade = geraParidade(mensagemSend);

    return 0;
}

int executaComandoCONTEUDO_ARQUIVO(char *string){

    char *cmd = calloc(MAX, sizeof(char));

    sprintf(cmd, "sed -i '%ds/.*/'%s'/' '%s'", linhaEdit, string, nomeArq);
    printf("COMANDO: %s\n", cmd);
    printf("STRING: %s\n", string);

    FILE *fp;
    fp = popen(cmd, "r");

    pclose(fp);
    ehEdit = 0;
    linhaEdit = -1;

    free(cmd);

    return 0;
}

int montaMensagemFIM_TRANSMISSAO(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos) {

    mensagemSend->marcadorInicio = MARCADOR_INICIO;
    mensagemSend->enderecoDestino = END_SERVIDOR;
    mensagemSend->enderecoOrigem = END_USUARIO;
    mensagemSend->tamanho = sizeof(mensagemSend->dados);
    mensagemSend->sequencia = sequencia;
    mensagemSend->tipo = FIM_TRANSMISSAO;
    strcpy(mensagemSend->dados, argumentos);
    mensagemSend->paridade = geraParidade(mensagemSend);

    return 0;
}