#include "grafo.h"
#include "programa.h"

//montar o grafo de roteamento
int inicializa( int mCusto[][MAX],
				int id_inicio, 
				int listaVizinhos[], 
				informacoesRoteador_t *infoRoteador, 
				roteadorVizinho_t infoVizinhos[N_ROTEADORES]){

	int a, b, c, count = 0;

	int i = 0;
	for(int d = 0; d < N_ROTEADORES; d++)
		for(int e = 0; e < N_ROTEADORES; e++)
			tabRoteamento[d][e] = INF;
	
	tabRoteamento[id_inicio][id_inicio] = 0;

	while (i < N_ROTEADORES){        //inicializa os vizinhos
		infoVizinhos[i].id = infoVizinhos[i].porta = infoVizinhos[i].timer = -1;
		infoVizinhos[i].custo = INF;
		i++;

	}
	
	FILE *arquivo = fopen("enlaces.config","r");

	while(fscanf(arquivo,"%d %d %d",&a, &b, &c)!= EOF){
		int aux = 0;
		if(b == id_inicio){
			aux = a;
			a = b;
			b = aux;
		}
		if(a == id_inicio){
			mCusto[count][0] = a;
			mCusto[count][1] = b;
			mCusto[count][2] = c;
			listaVizinhos[count] = b;

			//seta algumas informações dos vizinhos diretos
			infoVizinhos[count].id = b;  //id
			infoVizinhos[count].custo = c;   //custo
			infoVizinhos[count].porta = rota[b].port; //porta
			strcpy(infoVizinhos[count].ip, rota[b].ip);   //ip
			

			tabRoteamento[id_inicio][b] = c;  //inicia valores na tabela de roteamento - apenas vizinhos diretos. O restante é calculado durante a execução
			count ++;
		}
	}

	infoRoteador->qtdVizinhos = count;  //recebe num vizinhos
	return count;
}


// verifica por qual vizinho deve ser a saida da mensagem
int getEnlace(int idIni, int idFin){
	
	int rotMandar = -1;
	int custo_max = INF;

	if(tabRoteamento[idIni][idFin] == INF)  //conheço um caminho pro destino
		return rotMandar;

	for(int i = 0; i < ordem; i++){
		if(tabRoteamento[infoVizinhos[i].id][idFin] != INF){    //se eu conheco
			int custoTotal = tabRoteamento[infoVizinhos[i].id][idFin] + infoVizinhos[i].custo;
			if(custo_max > custoTotal){
				custo_max = custoTotal;
				rotMandar = infoVizinhos[i].id;
			}
		}
	}

	// printf("RETORNANDO ROTEADOR = %d", rotMandar);
	return rotMandar;
	
}


void atualizaTabRoteamento(msg vetorDistancia){
	int alteracao = 0;

	if(tabRoteamento[vetorDistancia.origem][vetorDistancia.origem] == INF)  //pode ter recebido vetores novos mas n atualizou o proprio, flag pra enviar
		alteracao = 1;

	for(int i = 0; i < N_ROTEADORES; i++){
		//recebe vetor distancia do vizinho
		//ex: rot = 1. Recebo vet distancia do rot 2 e copio os valores pra tabela do 1
		if(tabRoteamento[vetorDistancia.origem][i] != vetorDistancia.vetorCustos[i]){  //se os valores sao diferentes adiciono, senao ignoro
			tabRoteamento[vetorDistancia.origem][i] = vetorDistancia.vetorCustos[i];  //adiciona o vetor distacia enviado
		}
	}

	///atualiza as informações do proprio vetor distancia
	for(int i = 0; i < N_ROTEADORES; i++){
		int custo_total = vetorDistancia.vetorCustos[i] + vetorDistancia.custo; //CT = item do vet custos + custo pro pacote chegar aqui

		//percorre coluna - se for INF recebe custo total
		//					se oq tiver na tab for maior csto total = custo total
		if(tabRoteamento[vetorDistancia.destino][i] == INF || tabRoteamento[vetorDistancia.destino][i] > custo_total){ 
			tabRoteamento[vetorDistancia.destino][i] = custo_total;
			alteracao = 1;
		}
	}
		
	if(alteracao == 1)
		flagEnvia = 1;
}



//====================================== PRINTS DE DEBUG ===================================

//========================================================================================== 

void printaInforoteador(informacoesRoteador_t *infoRoteador){
	printf("ID = %d\n", infoRoteador->id);
	printf("PORTA = %d\n", infoRoteador->porta);
	printf("SOCK = %d\n", infoRoteador->sock);
	printf("QTD VIZINHOS = %d\n", infoRoteador->qtdVizinhos);
	printf("IP =  = %s\n", infoRoteador->ip);

}
void printaRota(){
	int i = 0;
	while (i <= N_ROTEADORES){
		printf("ID = %d\n", rota[i].id);
		printf("PORTA = %d\n", rota[i].port);
		printf("SOCK = %s\n\n\n", rota[i].ip);
		i++;
	}

}

void printaVizinhos(roteadorVizinho_t infoVizinhos[N_ROTEADORES], int ordem){
	int i = 0;
	printf("\n\nPRINT VIZINHOS\n");
	while (i < ordem){
		printf("ID = %d\n", infoVizinhos[i].id);
		printf("PORTA = %d\n", infoVizinhos[i].porta);
		printf("IP = %s\n", infoVizinhos[i].ip);
		printf("Custo = %d\n", infoVizinhos[i].custo);
		i++;
	}

}


void printaMatrizCustos(int mCusto[][MAX], int ordem){
	int i = 0;

	while (i < ordem){
		printf("%d - %d - %d\n", mCusto[i][0], mCusto[i][1], mCusto[i][2]);
		i++;
	}
	
}
void printaTabelaRoteamento(){
	for (int i = 1; i < N_ROTEADORES; i++){
		printf("\n");
		for (int j = 1; j < N_ROTEADORES; j++){
			printf("%d |", tabRoteamento[i][j]);
		}
	}
}

void printaRotas(){
	int i = 1;
	while(i < N_ROTEADORES){
		printf("ID = %d\n", rota[i].id);
		printf("PORTA = %d\n", rota[i].port);
		printf("IP = %s\n", rota[i].ip);
		i++;
	}
}