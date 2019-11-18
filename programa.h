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
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

//threads
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define ATTEMPTS_S 3
#define N_ROTEADORES 7
#define INF 9999
#define TAM_IP 10
#define TAM_FILA 1123456
#define TAM_MENSAGEM 100


pthread_t threadEnviar;
pthread_t threadReceber;
pthread_t threadDesempacotar;
pthread_t threadInformacao;
pthread_t threadVivacidade;
pthread_mutex_t logMutex;
pthread_mutex_t mensagemMutex;
pthread_mutex_t novidadeMutex;





enum msg_type{
    NONE, FALHA, ACK,
};

struct sockaddr_in socketRoteador;

typedef struct{
	  int destino;
	  int origem;
	  char mensagem[100];
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
    int controle;
    int destino;
    int origem;
    char mensagem[TAM_MENSAGEM];
    distSalto_t vetorDistancia[N_ROTEADORES];
}pacote_t;


typedef struct{
    int inicio;
    int fim;
    pacote_t filaPacotes[TAM_FILA];
    pthread_mutex_t mutex;
}filaPacotes_t;



typedef struct{
    int id;
    int porta;
    int sock;
    int qtdVizinhos;
    char ip[TAM_IP];
}informacoesRoteador_t;

typedef struct{
    int id;
    int custoOriginal;
    int saltoOriginal;
    int custo;
    int porta;
    int novidade;
    char ip[TAM_IP];
} roteadorVizinho_t;


struct roteador rota[100];
int tabRoteamento[N_ROTEADORES][N_ROTEADORES];
int listaVizinhos[N_ROTEADORES];
int ordem;

void die(char *s);

void getRoteadorConfig();

void send_n(msg mensagem, int prox_roteador, int roteador_atual );
void *sender(int roteador);
void *receiver(int roteador);
#endif
