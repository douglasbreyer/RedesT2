#ifndef _GRAFO_
#define _GRAFO_

#include <stdio.h>
#include "grafo.h"
#define MAX 1234


int montaGrafo(int mAdj[][MAX]);
void iniciaFila();
int verificaFila(int num, int a);
void alteraMatriz(int vertice, int anterior, int dist);
void disk(int matriz[][MAX], int inicio, int fim);
void imprimir(int inicio, int fim);


#endif
