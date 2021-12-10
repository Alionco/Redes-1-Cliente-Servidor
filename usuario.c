#include "ConexaoRawSocket.h"
#include "protocoloKermit.h"
#include "comandos.h"
#include <stdlib.h>
#include <unistd.h>

/*
A ordem dos dados na mensagem:
    MARCADOR_INICIO -> ENDEREÇO DE DESTINO -> ENDEREÇO DE ORIGEM -> TAMANHO DO CAMPO DE DADOS ->
    -> SEQUÊNCIA -> TIPO DA MENSAGEM -> DADOS -> PARIDADE 
*/

uint8_t numeroSequenciaReceive = 0;
uint8_t numeroSequenciaSend = 0;

void printChar(char * buf) {
    setvbuf(stdout, NULL, _IONBF, 0); 

    for(int i = 0; i < strlen(buf); i++) {
        printf(">%c< ", buf[i]);
    }
}

void parseEdit(char *dest, char *orig) {

    char *aux1 = calloc(MAX, sizeof(char));
    char *aux2 = calloc(MAX, sizeof(char));
    char *aux3 = calloc(MAX, sizeof(char)); 

    sscanf(orig,"%s%s", aux1, aux2);

    int indiceInicio = strlen(aux2)+3;

    int j = 0;
    for(int i = indiceInicio; i < strlen(orig); i++) {
        aux3[j] = orig[i];
        j++;
    }
    aux3[strlen(aux3)] = '\0';

    printChar(aux3);
    


    strcpy(dest, aux3);

    free(aux1);
    free(aux2);
    free(aux3);
    
}

void incrementaSequenciaReceive(pacote_t *mensagemReceive) {
    numeroSequenciaReceive = mensagemReceive->sequencia;
    if(numeroSequenciaReceive == 15){
        numeroSequenciaReceive = 0;
    }
}

void incrementaSequenciaSend() {

    numeroSequenciaSend++;
    if(numeroSequenciaSend == 16){
        numeroSequenciaSend = 1;
    }
}

int identificaArgumentos(char *comando, char *argumentos, int tipo) {

    switch(tipo) {

        case(CD):
            strncpy(argumentos, &comando[3], strlen(comando));
            break;
        case(LCD):
            strncpy(argumentos, &comando[4], strlen(comando));
            break;
        case(VER):
            strncpy(argumentos, &comando[4], strlen(comando));
            break;
        case(LINHA):
            strncpy(argumentos, &comando[6], strlen(comando));
            break;
        case(LINHAS):
            strncpy(argumentos, &comando[7], strlen(comando));
            break;
        case(EDIT):
            strncpy(argumentos, &comando[5], strlen(comando));
            break;
        case(COMPILAR):
            strncpy(argumentos, &comando[9], strlen(comando));
            break;
        default:
            break;
    }

    return 0;
}

int aguardaResposta(int socket, pacote_t *mensagemSend, pacote_t *mensagemReceive) {

    setvbuf(stdout, NULL, _IONBF, 0); 

    int paraEspera = 1;
    int tipoResposta;

    while(paraEspera) {

        recv(socket, mensagemReceive, sizeof(pacote_t), 0);

        if(mensagemReceive->enderecoDestino == END_USUARIO && mensagemReceive->enderecoOrigem == END_SERVIDOR && mensagemReceive->sequencia == numeroSequenciaReceive + 1) { // se a mensagem é do servidor para este usuario
            
            incrementaSequenciaReceive(mensagemReceive);

            switch(mensagemReceive->tipo) {

                case(ACK):
                    printf("Usuario recebeu uma msg ACK\n");

                    tipoResposta = ACK;
                    paraEspera = 0;
                    break;
                case(NACK):
                    printf("Usuario recebeu uma msg NACK... reenviando ultima msg.\n");
                    printf("Ultima mensagem era um: %d\n", mensagemSend->tipo);

                    incrementaSequenciaSend();
                    mensagemSend->sequencia = numeroSequenciaSend;  
                    send(socket, mensagemSend, sizeof(pacote_t), 0);
                    aguardaResposta(socket, mensagemSend, mensagemReceive);

                    paraEspera = 0;
                    break;
                case(ERRO):
                    printf("Usuario recebeu uma msg ERRO\n");
                    if(strcmp("1", mensagemReceive->dados) == 0) {
                        printf("Erro 1: acesso proibido / sem permissao\n");
                    } else if(strcmp("2", mensagemReceive->dados) == 0) {
                        printf("Erro 2: diretorio inexistente\n");
                    } else if(strcmp("3", mensagemReceive->dados) == 0) {
                        printf("Erro 3: arquivo inexistente\n");
                    } else if(strcmp("4", mensagemReceive->dados) == 0) {
                        printf("Erro 4: linha inexistente\n");
                    }

                    tipoResposta = ERRO;
                    paraEspera = 0;
                    break;
                case(FIM_TRANSMISSAO):
                    printf("Usuario recebeu uma msg FIM_TRANSMISSAO\n");

                    incrementaSequenciaSend();
                    mensagemSend->marcadorInicio = MARCADOR_INICIO;
                    mensagemSend->enderecoDestino = END_SERVIDOR;
                    mensagemSend->enderecoOrigem = END_USUARIO;
                    mensagemSend->tamanho = sizeof(mensagemSend->dados);
                    mensagemSend->sequencia = numeroSequenciaSend;
                    mensagemSend->tipo = ACK;
                    mensagemSend->paridade = geraParidade(mensagemSend);
                    send(socket, mensagemSend, sizeof(pacote_t), 0);
                    
                    paraEspera = 0;
                    break;
                case(LS_CONTEUDO):
                    //printf("Usuario recebeu uma msg LS_CONTEUDO... imprimindo o que foi recebido e enviando ACK\n");

                    printf("%s", mensagemReceive->dados);

                    incrementaSequenciaSend();
                    mensagemSend->marcadorInicio = MARCADOR_INICIO;
                    mensagemSend->enderecoDestino = END_SERVIDOR;
                    mensagemSend->enderecoOrigem = END_USUARIO;
                    mensagemSend->tamanho = sizeof(mensagemSend->dados);
                    mensagemSend->sequencia = numeroSequenciaSend;
                    mensagemSend->tipo = ACK;
                    mensagemSend->paridade = geraParidade(mensagemSend);
                    send(socket, mensagemSend, sizeof(pacote_t), 0);

                    //aguarda o prox LS_CONTEUDO
                    aguardaResposta(socket, mensagemSend, mensagemReceive);
                    
                    paraEspera = 0;
                    break;
                case(CONTEUDO_ARQUIVO):
                    //printf("Usuario recebeu uma msg CONTEUDO_ARQUIVO\n");

                    printf("%s", mensagemReceive->dados);

                    incrementaSequenciaSend();
                    mensagemSend->marcadorInicio = MARCADOR_INICIO;
                    mensagemSend->enderecoDestino = END_SERVIDOR;
                    mensagemSend->enderecoOrigem = END_USUARIO;
                    mensagemSend->tamanho = sizeof(mensagemSend->dados);
                    mensagemSend->sequencia = numeroSequenciaSend;
                    mensagemSend->tipo = ACK;
                    mensagemSend->paridade = geraParidade(mensagemSend);
                    send(socket, mensagemSend, sizeof(pacote_t), 0);

                    //aguarda o prox CONTEUDO_ARQ
                    aguardaResposta(socket, mensagemSend, mensagemReceive);

                    paraEspera = 0;
                    break;
                default:
                    printf("Usuário entrou em default\n");

                    paraEspera = 0;
                    break;

            }

        }
   
    }

    return tipoResposta;
}

int main() {

    setvbuf(stdout, NULL, _IONBF, 0); 

    int socket = ConexaoRawSocket("lo");

    pacote_t *mensagemReceive;
    mensagemReceive = calloc(1, sizeof(pacote_t));
    pacote_t *mensagemSend;
    mensagemSend = calloc(1, sizeof(pacote_t));

    int resposta;

    while(1) {

        char *comando = calloc(MAX,sizeof(char));
        char *argumentos = calloc(MAX,sizeof(char));
        char *dadosEnv = calloc(15,sizeof(char));
        char *aux = calloc(MAX,sizeof(char));

        int count = 0;

        argumentos[0] = '\0';

        fgets(comando, MAX, stdin);
        comando[strlen(comando)-1] = '\0';
        //fflush(stdout);

        if(strncmp(comando, "cd", 2) == 0) {
            printf("Usuario entrou com comando CD\n");
            identificaArgumentos(comando, argumentos, CD);
            incrementaSequenciaSend();
            montaMensagemCD(mensagemSend, numeroSequenciaSend, argumentos);
            send(socket, mensagemSend, sizeof(pacote_t), 0);
            aguardaResposta(socket, mensagemSend, mensagemReceive);

        } else if(strncmp(comando, "lcd", 3) == 0) {
            printf("Usuario entrou com comando LCD\n");
            identificaArgumentos(comando, argumentos, LCD);

            int ret = executaComandoLCD(argumentos);
            if(ret == -1){
                printf("Erro. Diretorio local nao existente\n");
            }
            
        } else if(strncmp(comando, "ls", 2) == 0) {
            printf("Usuario entrou com comando LS\n");

            incrementaSequenciaSend();
            montaMensagemLS(mensagemSend, numeroSequenciaSend);
            send(socket, mensagemSend, sizeof(pacote_t), 0);

            aguardaResposta(socket, mensagemSend, mensagemReceive);

        } else if(strncmp(comando, "lls", 3) == 0) {
            printf("Usuario entrou com comando LLS\n");
            char *buffer = calloc(MAX,sizeof(char));
            executaComandoLLS(buffer);
            printf("%s\n", buffer);
            free(buffer);

        } else if(strncmp(comando, "ver", 3) == 0) {
            printf("Usuario entrou com comando VER\n");
            identificaArgumentos(comando, argumentos, VER);

            incrementaSequenciaSend();
            montaMensagemVER(mensagemSend, numeroSequenciaSend, argumentos);
            send(socket, mensagemSend, sizeof(pacote_t), 0);

            aguardaResposta(socket, mensagemSend, mensagemReceive);

        } else if(strncmp(comando, "linhas", 6) == 0) {
            printf("Usuario entrou com comando LINHAS\n");
            identificaArgumentos(comando, argumentos, LINHAS);

            incrementaSequenciaSend();
            montaMensagemLINHAS(mensagemSend, numeroSequenciaSend, argumentos);
            send(socket, mensagemSend, sizeof(pacote_t), 0);
            resposta = aguardaResposta(socket, mensagemSend, mensagemReceive);

            if(resposta == ACK) {

                printf("Usuario vai enviar comando LINHAS_INI_FIM\n");

                incrementaSequenciaSend();
                montaMensagemLINHAS_INI_FIM(mensagemSend, numeroSequenciaSend, argumentos);
                send(socket, mensagemSend, sizeof(pacote_t), 0);
                resposta = aguardaResposta(socket, mensagemSend, mensagemReceive);
                
            }

        } else if(strncmp(comando, "linha", 5) == 0) {
            printf("Usuario entrou com comando LINHA\n");
            identificaArgumentos(comando, argumentos, LINHA);

            incrementaSequenciaSend();
            montaMensagemLINHA(mensagemSend, numeroSequenciaSend, argumentos);
            send(socket, mensagemSend, sizeof(pacote_t), 0);
            resposta = aguardaResposta(socket, mensagemSend, mensagemReceive);

            if(resposta == ACK) {

                printf("Usuario vai enviar comando LINHAS_INI_FIM\n");

                incrementaSequenciaSend();
                montaMensagemLINHAS_INI_FIM(mensagemSend, numeroSequenciaSend, argumentos);
                send(socket, mensagemSend, sizeof(pacote_t), 0);
                aguardaResposta(socket, mensagemSend, mensagemReceive);
                
            }

        } else if(strncmp(comando, "edit", 4) == 0) {
            printf("Usuario entrou com comando EDIT\n");
            identificaArgumentos(comando, argumentos, EDIT);

            incrementaSequenciaSend();
            montaMensagemEDIT(mensagemSend, numeroSequenciaSend, argumentos);
            send(socket, mensagemSend, sizeof(pacote_t), 0);
            resposta = aguardaResposta(socket, mensagemSend, mensagemReceive);

            if(resposta == ACK) {

                printf("Usuario vai enviar comando LINHAS_INI_FIM\n");

                incrementaSequenciaSend();
                montaMensagemLINHAS_INI_FIM(mensagemSend, numeroSequenciaSend, argumentos);
                send(socket, mensagemSend, sizeof(pacote_t), 0);
                resposta = aguardaResposta(socket, mensagemSend, mensagemReceive);

                if(resposta == ACK) {

                    printf("Usuario vai enviar comando CONTEUDO_ARQUIVO\n");
                    parseEdit(aux, argumentos);

                    for(int i = 0; i < strlen(aux); i++) {
                        dadosEnv[count] = aux[i];
                        count++;
                        if(count == 15) {
                            count = 0;
                            incrementaSequenciaSend();
                            montaMensagemCONTEUDO_ARQUIVO(mensagemSend, numeroSequenciaSend, dadosEnv);
                            send(socket, mensagemSend, sizeof(pacote_t), 0);
                            aguardaResposta(socket, mensagemSend, mensagemReceive);
                        }
                    }
                    if(count > 0) {

                        dadosEnv[count] = '\0';
                        incrementaSequenciaSend();
                        montaMensagemCONTEUDO_ARQUIVO(mensagemSend, numeroSequenciaSend, dadosEnv);
                        send(socket, mensagemSend, sizeof(pacote_t), 0);
                        aguardaResposta(socket, mensagemSend, mensagemReceive);

                    }

                    incrementaSequenciaSend();
                    montaMensagemFIM_TRANSMISSAO(mensagemSend, numeroSequenciaSend, argumentos);
                    send(socket, mensagemSend, sizeof(pacote_t), 0);
                    aguardaResposta(socket, mensagemSend, mensagemReceive);
                }
            }
        } else if(strncmp(comando, "compilar", 8) == 0) {
            printf("Usuario entrou com comando COMPILAR\n");
            identificaArgumentos(comando, argumentos, COMPILAR);

            incrementaSequenciaSend();
            montaMensagemCOMPILAR(mensagemSend, numeroSequenciaSend, argumentos);

            send(socket, mensagemSend, sizeof(pacote_t), 0);

            aguardaResposta(socket, mensagemSend, mensagemReceive);

        } else {

            printf("Comando inválido.\n");
        }

        free(comando);
        free(argumentos);
        free(dadosEnv);
        free(aux);
        comando = NULL;
        argumentos = NULL;
        dadosEnv = NULL;
        aux = NULL;

    }

    return 0;
    
}
