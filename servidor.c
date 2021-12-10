#include "ConexaoRawSocket.h"
#include "protocoloKermit.h"
#include "comandos.h"
#include <stdlib.h>

uint8_t numeroSequenciaSend = 0;
uint8_t numeroSequenciaReceive = 0;
int global = 0;

void printChar(char * buf) {
    setvbuf(stdout, NULL, _IONBF, 0); 

    for(int i = 0; i < strlen(buf); i++) {
        printf(">%c< ", buf[i]);
    }
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

int aguardaResposta(int socket, pacote_t *mensagemSend, pacote_t *mensagemReceive) {

    setvbuf(stdout, NULL, _IONBF, 0); 

    int paraEspera = 1;
    int tipoResposta;

    while(paraEspera) {

        recv(socket, mensagemReceive, sizeof(pacote_t), 0);


        if(mensagemReceive->enderecoDestino == END_SERVIDOR && mensagemReceive->enderecoOrigem == END_USUARIO && mensagemReceive->sequencia == numeroSequenciaReceive + 1) { // se a mensagem é do usuario para este servidor
            
            incrementaSequenciaReceive(mensagemReceive);

            switch(mensagemReceive->tipo) {

                case(ACK):
                    printf("Servidor recebeu uma msg ACK\n");
                    tipoResposta = ACK;

                    paraEspera = 0;
                    break;
                case(NACK):
                    printf("Servidor recebeu uma msg NACK... reenviando ultima msg.\n");
                    printf("Ultima mensagem era um: %d\n", mensagemSend->tipo);

                    incrementaSequenciaSend();
                    mensagemSend->sequencia = numeroSequenciaSend;  
                    send(socket, mensagemSend, sizeof(pacote_t), 0);
                    aguardaResposta(socket, mensagemSend, mensagemReceive);

                    paraEspera = 0;
                    break;
                case(ERRO):
                    printf("Servidor recebeu uma msg ERRO\n");
                    
                    paraEspera = 0;
                    break;
                case(FIM_TRANSMISSAO):
                    printf("Servidor recebeu uma msg FIM_TRANSMISSAO\n");

                    tipoResposta = FIM_TRANSMISSAO;
                    
                    paraEspera = 0;
                    break;
            
                case(CONTEUDO_ARQUIVO):
                    printf("Servidor recebeu uma msg CONTEUDO_ARQUIVO\n");

                    tipoResposta = CONTEUDO_ARQUIVO;
                    paraEspera = 0;
                    break;
                default:
                    printf("Servidor entrou em default\n");

                    paraEspera = 0;
                    break;

            }

        }
   
    }

    return tipoResposta;
}


int enviaResposta(int socket, pacote_t *mensagemSend, int tipo, char *buffer) {

    incrementaSequenciaSend();

    int valReturn = 0;

    switch(tipo) {
        case(ACK):
            printf("Servidor vai enviar uma msg ACK\n");

            mensagemSend->marcadorInicio = MARCADOR_INICIO;
            mensagemSend->enderecoDestino = END_USUARIO;
            mensagemSend->enderecoOrigem = END_SERVIDOR;
            mensagemSend->tamanho = sizeof(mensagemSend->dados);
            mensagemSend->sequencia = numeroSequenciaSend;
            mensagemSend->tipo = ACK;
            mensagemSend->paridade = geraParidade(mensagemSend);

            break;

        case(NACK):
            printf("Servidor vai enviar uma msg NACK\n");

            mensagemSend->marcadorInicio = MARCADOR_INICIO;
            mensagemSend->enderecoDestino = END_USUARIO;
            mensagemSend->enderecoOrigem = END_SERVIDOR;
            mensagemSend->tamanho = sizeof(mensagemSend->dados);
            mensagemSend->sequencia = numeroSequenciaSend;
            mensagemSend->tipo = NACK;
            mensagemSend->paridade = geraParidade(mensagemSend);

            break;

        case(ERRO):
            printf("Servidor vai enviar uma msg ERRO\n");

            mensagemSend->marcadorInicio = MARCADOR_INICIO;
            mensagemSend->enderecoDestino = END_USUARIO;
            mensagemSend->enderecoOrigem = END_SERVIDOR;
            mensagemSend->tamanho = sizeof(mensagemSend->dados);
            mensagemSend->sequencia = numeroSequenciaSend;
            mensagemSend->tipo = ERRO;
            strcpy(mensagemSend->dados, buffer);
            mensagemSend->paridade = geraParidade(mensagemSend);

            break;

        case(FIM_TRANSMISSAO):
            printf("Servidor vai enviar uma msg FIM_TRANSMISSAO\n");

            mensagemSend->marcadorInicio = MARCADOR_INICIO;
            mensagemSend->enderecoDestino = END_USUARIO;
            mensagemSend->enderecoOrigem = END_SERVIDOR;
            mensagemSend->tamanho = sizeof(mensagemSend->dados);
            mensagemSend->sequencia = numeroSequenciaSend;
            mensagemSend->tipo = FIM_TRANSMISSAO;
            mensagemSend->paridade = geraParidade(mensagemSend);
                    
            break;

        case(LS_CONTEUDO):
            printf("Servidor vai enviar uma msg LS_CONTEUDO\n");
            
            mensagemSend->marcadorInicio = MARCADOR_INICIO;
            mensagemSend->enderecoDestino = END_USUARIO;
            mensagemSend->enderecoOrigem = END_SERVIDOR;
            mensagemSend->tamanho = sizeof(mensagemSend->dados);
            mensagemSend->sequencia = numeroSequenciaSend;
            mensagemSend->tipo = LS_CONTEUDO;
            strcpy(mensagemSend->dados, buffer);
            mensagemSend->paridade = geraParidade(mensagemSend);

            break;

        case(CONTEUDO_ARQUIVO):
            printf("Servidor vai enviar uma msg CONTEUDO_ARQUIVO\n");

            mensagemSend->marcadorInicio = MARCADOR_INICIO;
            mensagemSend->enderecoDestino = END_USUARIO;
            mensagemSend->enderecoOrigem = END_SERVIDOR;
            mensagemSend->tamanho = sizeof(mensagemSend->dados);
            mensagemSend->sequencia = numeroSequenciaSend;
            mensagemSend->tipo = CONTEUDO_ARQUIVO;
            strcpy(mensagemSend->dados, buffer);
            mensagemSend->paridade = geraParidade(mensagemSend);

        
            break;

        default:
            printf("Servidor entrou em default\n");
            break;
    }

    send(socket, mensagemSend, sizeof(pacote_t), 0);
    return valReturn;
}


int main() {

    pacote_t *mensagemReceive;
    mensagemReceive = calloc(1,sizeof(pacote_t));
    pacote_t *mensagemSend;
    mensagemSend = calloc(1,sizeof(pacote_t));
    int socket = ConexaoRawSocket("lo");

    int tipo, ret, paridade, resposta;
    
    char *buffer = calloc(MAX*10,sizeof(char));
    char *dadosEnv = calloc(15,sizeof(char));

    buffer[0] = '\0';


    while(1) {

        recv(socket, mensagemReceive, sizeof(pacote_t), 0);
        int count = 0;

        if(mensagemReceive->enderecoDestino == END_SERVIDOR && mensagemReceive->enderecoOrigem == END_USUARIO && mensagemReceive->sequencia == numeroSequenciaReceive + 1) { // se a mensagem é do usuario para este servidor

            incrementaSequenciaReceive(mensagemReceive);

            switch(mensagemReceive->tipo) {

                case(ACK):
                    printf("Servidor recebeu uma msg ACK\n");
                    paridade = verificaParidade(mensagemReceive);
                    if(paridade != 0) {
                        enviaResposta(socket, mensagemSend, NACK, NULL);
                        break;
                    }

                    break;
                case(NACK):
                    printf("Servidor recebeu uma msg NACK\n");
                    paridade = verificaParidade(mensagemReceive);
                    if(paridade != 0) {
                        enviaResposta(socket, mensagemSend, NACK, NULL);
                        break;
                    }
                    break;

                case(ERRO):
                    printf("Servidor recebeu uma msg ERRO\n");
                    paridade = verificaParidade(mensagemReceive);
                    if(paridade != 0) {
                        enviaResposta(socket, mensagemSend, NACK, NULL);
                        break;
                    }

                    break;
                case(FIM_TRANSMISSAO):
                    printf("Servidor recebeu uma msg FIM_TRANSMISSAO\n");
                    paridade = verificaParidade(mensagemReceive);
                    if(paridade != 0) {
                        enviaResposta(socket, mensagemSend, NACK, NULL);
                        break;
                    }

                    break;
                case(CD):
                    printf("Servidor recebeu uma msg CD\n");

                    paridade = verificaParidade(mensagemReceive);
                    if(paridade != 0) {
                        enviaResposta(socket, mensagemSend, NACK, NULL);
                        break;
                    }

                    ret = executaComandoCD(mensagemReceive);
                    if(ret == -1) {
                        tipo = ERRO;
                    } else if(ret == 0) {
                        tipo = ACK;
                    }
                    enviaResposta(socket, mensagemSend, tipo, "2");
                    break;

                case(LS):
                    printf("Servidor recebeu uma msg LS\n");

                    paridade = verificaParidade(mensagemReceive);
/*                     if(paridade != 0) {
                        enviaResposta(socket, mensagemSend, NACK, NULL);
                        break;
                    } */

                    if(global <= 3) {
                        enviaResposta(socket, mensagemSend, NACK, NULL);
                        global++;
                        break;
                    }

                    executaComandoLS(buffer);
                    for(int i = 0; i < strlen(buffer); i++) {
                        dadosEnv[count] = buffer[i];
                        count++;
                        if(count == 15) {
                            count = 0;
                            enviaResposta(socket, mensagemSend, LS_CONTEUDO, dadosEnv);
                            aguardaResposta(socket, mensagemSend, mensagemReceive);
                        }
                    }
                    if(count > 0) {
                        dadosEnv[count-1] = '\n';
                        dadosEnv[count] = '\0'; 

                        enviaResposta(socket, mensagemSend, LS_CONTEUDO, dadosEnv);
                        aguardaResposta(socket, mensagemSend, mensagemReceive);
                    }

                    enviaResposta(socket, mensagemSend, FIM_TRANSMISSAO, NULL);
                    aguardaResposta(socket, mensagemSend, mensagemReceive);

                    break;
                case(VER):
                    printf("Servidor recebeu uma msg VER\n");

                    paridade = verificaParidade(mensagemReceive);
                    if(paridade != 0) {
                        enviaResposta(socket, mensagemSend, NACK, NULL);
                        break;
                    }

                    ret = executaComandoVER(mensagemReceive, buffer);
                    if(ret == -1) {
                        enviaResposta(socket, mensagemSend, ERRO, "3");
                        break;
                    }

                    // se o arquivo existe...

                    for(int i = 0; i < strlen(buffer); i++) {
                        dadosEnv[count] = buffer[i];
                        count++;
                        if(count == 15) {
                            count = 0;
                            enviaResposta(socket, mensagemSend, CONTEUDO_ARQUIVO, dadosEnv);
                            aguardaResposta(socket, mensagemSend, mensagemReceive);
                        }
                    }
                    if(count > 0) {
                        dadosEnv[count-1] = '\n';
                        dadosEnv[count] = '\0'; 

                        enviaResposta(socket, mensagemSend, CONTEUDO_ARQUIVO, dadosEnv);
                        aguardaResposta(socket, mensagemSend, mensagemReceive);
                    }

                    enviaResposta(socket, mensagemSend, FIM_TRANSMISSAO, NULL);
                    aguardaResposta(socket, mensagemSend, mensagemReceive);

                    break;
                case(LINHA):
                    printf("Servidor recebeu uma msg LINHA\n");

                    paridade = verificaParidade(mensagemReceive);
                    if(paridade != 0) {
                        enviaResposta(socket, mensagemSend, NACK, NULL);
                        break;
                    }

                    ret = executaComandoLINHA(mensagemReceive);
                    if(ret == -1) {
                        tipo = ERRO;
                    } else if(ret == 0) {
                        tipo = ACK;
                    }
                    enviaResposta(socket, mensagemSend, tipo, "3");
                    
                    break;
                case(LINHAS):
                    printf("Servidor recebeu uma msg LINHAS\n");

                    paridade = verificaParidade(mensagemReceive);
                    if(paridade != 0) {
                        enviaResposta(socket, mensagemSend, NACK, NULL);
                        break;
                    }

                    ret = executaComandoLINHAS(mensagemReceive);
                    if(ret == -1) {
                        tipo = ERRO;
                    } else if(ret == 0) {
                        tipo = ACK;
                    }
                    enviaResposta(socket, mensagemSend, tipo, "3");

                    break;
                case(EDIT):
                    printf("Servidor recebeu uma msg EDIT\n");

                    paridade = verificaParidade(mensagemReceive);
                    if(paridade != 0) {
                        enviaResposta(socket, mensagemSend, NACK, NULL);
                        break;
                    }

                    ret = executaComandoEDIT(mensagemReceive);
                    if(ret == -1) {
                        tipo = ERRO;
                    } else if(ret == 0) {
                        tipo = ACK;
                    }
                    enviaResposta(socket, mensagemSend, tipo, "3");

                    break;
                case(COMPILAR):
                    printf("Servidor recebeu uma msg COMPILAR\n");

                    paridade = verificaParidade(mensagemReceive);
                    if(paridade != 0) {
                        enviaResposta(socket, mensagemSend, NACK, NULL);
                        break;
                    }

                    ret = executaComandoCOMPILAR(mensagemReceive, buffer);
                    if(ret == -1) {
                        tipo = ERRO;
                        enviaResposta(socket, mensagemSend, tipo, "3");
                        break;
                    }

                    for(int i = 0; i < strlen(buffer); i++) {
                        dadosEnv[count] = buffer[i];
                        count++;
                        if(count == 15) {
                            count = 0;
                            enviaResposta(socket, mensagemSend, CONTEUDO_ARQUIVO, dadosEnv);
                            aguardaResposta(socket, mensagemSend, mensagemReceive);
                        }
                    }
                    if(count > 0) {
                        dadosEnv[count-1] = '\n';
                        dadosEnv[count] = '\0'; 

                        enviaResposta(socket, mensagemSend, CONTEUDO_ARQUIVO, dadosEnv);
                        aguardaResposta(socket, mensagemSend, mensagemReceive);
                    }

                    enviaResposta(socket, mensagemSend, FIM_TRANSMISSAO, NULL);
                    aguardaResposta(socket, mensagemSend, mensagemReceive);
                    
                    break;
                case(LINHAS_INI_FIM):
                    printf("Servidor recebeu uma msg LINHAS_INI_FIM\n");

                    paridade = verificaParidade(mensagemReceive);
                    if(paridade != 0) {
                        enviaResposta(socket, mensagemSend, NACK, NULL);
                        break;
                    }

                    ret = executaComandoLINHAS_INI_FIM(mensagemReceive, buffer);
                    if(ret == 0) {

                        for(int i = 0; i < strlen(buffer); i++) {
                            dadosEnv[count] = buffer[i];
                            count++;
                            if(count == 15) {
                                count = 0;
                                enviaResposta(socket, mensagemSend, CONTEUDO_ARQUIVO, dadosEnv);
                                aguardaResposta(socket, mensagemSend, mensagemReceive);
                            }
                        }
                        if(count > 0) {
                            dadosEnv[count-1] = '\n';
                            dadosEnv[count] = '\0'; 

                            enviaResposta(socket, mensagemSend, CONTEUDO_ARQUIVO, dadosEnv);
                            aguardaResposta(socket, mensagemSend, mensagemReceive);
                        }

                        enviaResposta(socket, mensagemSend, FIM_TRANSMISSAO, NULL);
                        aguardaResposta(socket, mensagemSend, mensagemReceive);
                        break;

                    } else if(ret == 1) {
                        tipo = ACK;
                    }
                    enviaResposta(socket, mensagemSend, tipo, buffer);
                    break;

                case(CONTEUDO_ARQUIVO):
                    printf("Servidor recebeu uma msg CONTEUDO_ARQUIVO\n");

                    paridade = verificaParidade(mensagemReceive);
                    if(paridade != 0) {
                        enviaResposta(socket, mensagemSend, NACK, NULL);
                        break;
                    }
                    
                    strcat(buffer, mensagemReceive->dados);
                    enviaResposta(socket, mensagemSend, ACK, NULL);
                    resposta = aguardaResposta(socket, mensagemSend, mensagemReceive);
                    while(resposta == CONTEUDO_ARQUIVO) {
                        recv(socket, mensagemReceive, sizeof(pacote_t), 0);
                        strcat(buffer, mensagemReceive->dados);
                        enviaResposta(socket, mensagemSend, ACK, NULL);
                        resposta = aguardaResposta(socket, mensagemSend, mensagemReceive);
                    }
                    printChar(buffer);

                    executaComandoCONTEUDO_ARQUIVO(buffer);
                    tipo = ACK;
                    enviaResposta(socket, mensagemSend, tipo, NULL);

                    break;
                default:
                    printf("Servidor entrou em default\n");

                    break;

            }

        }

        free(buffer);
        free(dadosEnv);
        dadosEnv = NULL;
        buffer = NULL;
        buffer = calloc(MAX,sizeof(char));
        dadosEnv = calloc(15,sizeof(char));

   
    }

    return 0;

}