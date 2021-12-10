#ifndef COMANDOS_H
#define COMANDOS_H

#include "ConexaoRawSocket.h"
#include "protocoloKermit.h"

#define MAX 10000

// Lista de códigos de retorno
#define ACESSO_PROIBIDO 1
#define DIR_INEXISTENTE 2
#define ARQ_INEXISTENTE 3
#define LINHA_INEXISTENTE 4

// Lista de tipos de mensagem
#define CD 0
#define LS 1
#define VER 2
#define LINHA 3
#define LINHAS 4
#define EDIT 5
#define COMPILAR 6
#define ACK 8
#define NACK 9
#define LINHAS_INI_FIM 10
#define LS_CONTEUDO 11
#define CONTEUDO_ARQUIVO 12
#define FIM_TRANSMISSAO 13
#define ERRO 15

#define LCD 99

int geraParidade(pacote_t *mensagemSend);
int verificaParidade(pacote_t *mensagemReceive);

int montaMensagemCD(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos);
int executaComandoCD(pacote_t *mensagemReceive);

int executaComandoLCD();

int montaMensagemLS(pacote_t *mensagemSend, uint8_t sequencia);
int executaComandoLS(char* buffer);

int executaComandoLLS();

int montaMensagemVER(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos);
int executaComandoVER(pacote_t *mensagemReceive, char *buffer);

int montaMensagemLINHA(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos);
int executaComandoLINHA(pacote_t *mensagemReceive);

int montaMensagemLINHAS(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos);
int executaComandoLINHAS(pacote_t *mensagemReceive);

int montaMensagemLINHAS_INI_FIM(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos);
int executaComandoLINHAS_INI_FIM(pacote_t *mensagemReceive, char *buffer);

int montaMensagemEDIT(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos);
int executaComandoEDIT(pacote_t *mensagemReceive);

int montaMensagemCOMPILAR(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos);
int executaComandoCOMPILAR();

int montaMensagemCONTEUDO_ARQUIVO(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos);
int executaComandoCONTEUDO_ARQUIVO(char *string);

int montaMensagemFIM_TRANSMISSAO(pacote_t *mensagemSend, uint8_t sequencia, char *argumentos);

#endif
