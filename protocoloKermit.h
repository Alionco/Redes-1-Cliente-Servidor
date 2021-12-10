#ifndef PROTOCOLOKERMIT_H
#define PROTOCOLOKERMIT_H

#include <stdint.h>
#include <string.h>

#define MARCADOR_INICIO 126
#define END_USUARIO 1
#define END_SERVIDOR 2 
#define TAM_MAX_MSG 15

/*
A ordem dos dados na mensagem:
    MARCADOR_INICIO -> ENDEREÇO DE DESTINO -> ENDEREÇO DE ORIGEM -> TAMANHO DO CAMPO DE DADOS ->
    -> SEQUÊNCIA -> TIPO DA MENSAGEM -> DADOS -> PARIDADE 
*/

typedef struct pacote_t {

    uint8_t marcadorInicio;
    uint8_t enderecoDestino;
    uint8_t enderecoOrigem;
    unsigned int tamanho;
    uint8_t sequencia;
    uint8_t tipo;
    char dados[TAM_MAX_MSG];
    unsigned int paridade;

} pacote_t;

#endif