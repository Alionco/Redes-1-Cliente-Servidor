# GRR20196992 Lucas Alionço
# Disciplina: Redes de Computadores (CI1058)
# Professor: Luiz Carlos Pessoa Albini

parametrosCompilacao = -Wall -g #-std=c99 
main1 = usuario
main2 = servidor

all: usuario servidor

usuario: usuario.c ConexaoRawSocket.o comandos.o
	gcc $(parametrosCompilacao) usuario.c ConexaoRawSocket.o comandos.o -o usuario

servidor: servidor.c ConexaoRawSocket.o comandos.o
	gcc $(parametrosCompilacao) servidor.c ConexaoRawSocket.o comandos.o -o servidor

ConexaoRawSocket.o: ConexaoRawSocket.c
	gcc $(parametrosCompilacao) -c ConexaoRawSocket.c

comandos.o: comandos.c
	gcc $(parametrosCompilacao) -c comandos.c

clean:
	rm -f *.o

purge: clean
	rm -f usuario servidor
