#ifndef _PROGRAMA_
#define _PROGRAMA_

//grafo
#include "grafo.h"

//Simple udp client
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

//threads
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define ATTEMPTS_S 3
#define N_ROTEADORES 5
#define INF 999
#define TAM_IP 10
#define TAM_FILA 1123456
#define TAM_MENSAGEM 100


enum msg_type{
    NONE, FALHA, ACK,
};

struct sockaddr_in socketRoteador;

typedef struct{
    int origem;
	int destino; 
	char mensagem[100];

    int teste;
    int custo;
    int vetorCustos[N_ROTEADORES];
	enum msg_type type;
}msg;

struct roteador{
	int id;
	int port;
	char ip[16];
};

typedef struct{
    int distancia;
    int proxSalto;
}distSalto_t;

typedef struct{
    int id;
    int porta;
    int sock;
    int qtdVizinhos;
    char ip[TAM_IP];
}informacoesRoteador_t;

typedef struct{
    int id;
    int custo;
    int porta;
    clock_t timer;
    char ip[TAM_IP];
} roteadorVizinho_t;


struct roteador rota[100];

int tabRoteamento[N_ROTEADORES][N_ROTEADORES];
int listaVizinhos[N_ROTEADORES];
roteadorVizinho_t infoVizinhos[N_ROTEADORES];
int ordem;
int ROTATUAL;
int flagEnvia;


void die(char *s);

void getRoteadorConfig();

void send_n(msg mensagem, int prox_roteador, int roteador_atual );
void *sender(int roteador);
void *receiver(int roteador);
void *distVector(int roteador);
void *timer(int roteador);
void printaTabelaRoteamento();
int getEnlace(int idIni, int idFin);
#endif
