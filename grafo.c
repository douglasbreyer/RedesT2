#include "grafo.h"
#include "programa.h"

//int mDISK[2][MAX]; //0->distancia 1-> vertice anterior
//int fila[MAX]; //fila de vertices não visitados
//int ordem=-1;

//montar o grafo de roteamento
int inicializa( int mCusto[][MAX],
				int id_inicio, 
				int listaVizinhos[], 
				informacoesRoteador_t *infoRoteador, 
				roteadorVizinho_t infoVizinhos[N_ROTEADORES],
				// filaPacotes_t *entrada,
                // filaPacotes_t *saida,
                pthread_mutex_t *logMutex,
                pthread_mutex_t *mensagemMutex,
                pthread_mutex_t *novidadeMutex){

	int a, b, c, count = 0;

	int i = 0;
	for(int d = 0; d < N_ROTEADORES; d++){
		for(int e = 0; e < N_ROTEADORES; e++){
			tabRoteamento[d][e] = INF;
		}
	}
	while (i < N_ROTEADORES){        //inicializa os vizinhos
		infoVizinhos[i].novidade = 0;
		infoVizinhos[i].id = infoVizinhos[i].porta = infoVizinhos[i].saltoOriginal = -1;
		infoVizinhos[i].custo = INF;
		i++;

	}
	
	FILE *arquivo = fopen("enlaces.config","r");

	while(fscanf(arquivo,"%d %d %d",&a, &b, &c)!= EOF){
		// printf("Lendo o arquivo\n");
		// printf("%d\n", );
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
			//printf("\n======\n%s - %d\n=======\n", rota[b].ip, rota[b].id);
			
			
		}
		// if(a>ordem)ordem=a;
		// if(b>ordem)ordem=b;
	}
	// i = 0;
	// while (i < count) {
	// 	printf("%d\n", listaVizinhos[i]);
	// 	printf("%d %d %d\n",mCusto[i][0], mCusto[i][1], mCusto[i][2]);
	// 	i++;
	// }
	infoRoteador->qtdVizinhos = count;  //recebe num vizinhos

	pthread_mutex_init(logMutex, NULL);
	pthread_mutex_init(mensagemMutex, NULL);
	pthread_mutex_init(novidadeMutex, NULL);
	
	
	return count;
}


int iniciaComunicacao(int idIni, int idFin){
	printf("Entrou no comunica\n\n\n\n");
}

// verifica por qual vizinho deve ser a saida da mensagem
int getEnlace(int idIni, int idFin){
	int i = 0, j = 0;
	int tem = 0; //tem caminho para o destino
	int rotMandar = 0; //para qual rot mandar
	int custoE = INF;
	printaTabelaRoteamento();
	//flagEnvia = 1;
	if(tabRoteamento[idIni][idFin] != INF){   //verifica se conhece o roteador

		while(i < N_ROTEADORES){
			if(tabRoteamento[i][idFin] != INF){    //corre a coluna do IDFIM verificando se tem algum menor custo
				if(tabRoteamento[i][idFin] < custoE){  //pega o menor custo
					custoE = tabRoteamento[i][idFin];
					rotMandar = i;
					tem = 1;
					printf("\n\n%d\n\n", custoE);
				}
			}
			i++;
		}

		if(tem == 1){      //tem q pegar o menor
			
		}
		else{          //aqui tem q mandar vetor distancia pois n fez comunicaççao com os vizinhos
			printf("NÃO CONHECE O MENOR CAMINHO, PODE SER VIZINHO\n\n\n"); 
			iniciaComunicacao(idIni, idFin);   //TEM Q VERIFICAR SE É VIZINHO PRA COMECAR A COMUNICAR
		}
	}
	//for(int i = 0; i < N_ROTEADORES; i++){
	//	tabRoteamento[idIni][i] = 1;
	//}
	printf("ROTMANDAR = %d\n\n", rotMandar);
	if(rotMandar != INF && rotMandar != 0 && rotMandar != idIni){
		return rotMandar; //retorna qual vizinho deve mandar a mensagem
		printf("\nMANDOU OQ DEVIA MANDAR\n");
	}
	else{
		printf("PEGOU NO ELSE\n");
		return idFin;
	}
	
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
		printf("Novidade = %d\n\n\n", infoVizinhos[i].novidade);
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
	int j = 0;
	for (int i = 0; i < N_ROTEADORES; i++){
		printf("\n");
		for (j = 0; j < N_ROTEADORES; j++){
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