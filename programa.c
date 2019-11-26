#include "programa.h"
#include <string.h>

int matrizCustosVizinhos[MAX][3], prox_roteador;
informacoesRoteador_t infoRoteador;
int slen = sizeof(socketRoteador);


int main(int argc, char *argv[]) {

  	int id_inicio;
	ordem = -1;
	flagEnvia = 1;
    //roteador instanciado
  	id_inicio = atoi(argv[1]);
	ROTATUAL = id_inicio;


  	getRoteadorConfig(&infoRoteador, id_inicio);  // pego os ids, portas e ips


	if (id_inicio >= N_ROTEADORES || id_inicio < 1) {
		die("Esse id de roteador não existe. Observe o arquivo de configuração.\nEncerando roteador!\n");
	}


	ordem = inicializa(matrizCustosVizinhos, id_inicio, listaVizinhos, &infoRoteador, infoVizinhos); 			// adiciona os vizinho diretos e os custos a matriz de roteamento

	

	//=============== DEBUG ===========
	//printaInforoteador(&infoRoteador);
	//printaRota();
	//printaVizinhos(infoVizinhos, ordem);
	//printamatrizCustosVizinhos(matrizCustosVizinhos, ordem);
	//printaTabelaRoteamento(tabRoteamento);
	//printaRotas();




	if((infoRoteador.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
		die("Não foi possível criar o Socket!");
	}

	memset((char*) &socketRoteador, 0, sizeof(socketRoteador));
	socketRoteador.sin_family = AF_INET;
	socketRoteador.sin_port = htons(infoRoteador.porta);
	socketRoteador.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(infoRoteador.sock, (struct sockaddr*) &socketRoteador, sizeof(socketRoteador)) == -1)
		die("Não foi possível conectar o socket com a porta\n");

  	// Threads para mandar e receber mensagem
	pthread_t tids[3];
	pthread_create(&tids[0], NULL, (void *)sender, id_inicio);
	pthread_create(&tids[1], NULL, (void *)receiver, id_inicio);
	pthread_create(&tids[2], NULL, (void *)distVector, id_inicio);
	pthread_create(&tids[2], NULL, (void *)timer, id_inicio);
	pthread_join(tids[0], NULL);
	pthread_join(tids[1], NULL);

	return 0;
}


void die(char *s){
    perror(s);
    exit(1);
}
//le o arquivo roteador config e atribui a struct rota
void getRoteadorConfig(informacoesRoteador_t *infoRoteador, int id_inicio){

	int i = 1, j = 0;
	FILE *arquivo = fopen("roteador.config","r");

	while(fscanf(arquivo,"%d %d %s",&rota[i].id, &rota[i].port, rota[i].ip)!= EOF){
		if (rota[i].id == id_inicio){
			infoRoteador->id = id_inicio;
			infoRoteador->porta = rota[i].port;
			strcpy(infoRoteador->ip, rota[i].ip);
		}
		i++;
	}
}

void *timer(int id_inicio){
	clock_t aux, tDecorrido;

	for(int i = 0; i < ordem; i++){
		infoVizinhos[i].timer = clock();
	}
	
	while(1){ 

		aux = clock();
		for(int i = 0; i < ordem; i++){
			tDecorrido = ((aux - infoVizinhos[i].timer) / (CLOCKS_PER_SEC / 1000));
			
			if(tDecorrido > 20000){
				infoVizinhos[i].timer = clock();
				tDecorrido = 0;
				flagEnvia = 1;
			}
		}
		
	}
	
}


void *distVector(int roteador){

	while(1){
		if(flagEnvia == 1){

			int sockt, s;
			struct sockaddr_in si_dest, si_other;
			unsigned int slen = sizeof(si_dest);

			//Cria o UDP socket
			if((sockt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
				die("socket");
			}

			unsigned int slen1 = sizeof(si_other);
			memset((char *) &si_other, 0, sizeof(si_other));
			si_other.sin_family = AF_INET;
			si_other.sin_port = htons(rota[roteador].port);
			
			msg vetorDistancia;
			int i = 0;
			int j;
			while (i < ordem){
				vetorDistancia.teste = 1;
				vetorDistancia.origem = roteador;
				vetorDistancia.destino = infoVizinhos[i].id;
				vetorDistancia.custo = infoVizinhos[i].custo;
				memset(vetorDistancia.mensagem,0, sizeof(vetorDistancia.mensagem));
				j = 0;
				while (j < N_ROTEADORES){
					vetorDistancia.vetorCustos[j] = tabRoteamento[roteador][j];
					j++;
				}
				
				
				memset((char *) &si_dest, 0, sizeof(si_dest));
				si_dest.sin_family = AF_INET;
				si_dest.sin_port = htons(rota[infoVizinhos[i].id].port + 10);
	
				
				if (inet_aton(rota[infoVizinhos[i].id].ip , &si_dest.sin_addr) == 0){ //address to number
					fprintf(stderr, "inet_aton() send_n() failed\n"),
					exit(1);
				}

				if (sendto(sockt, &vetorDistancia, sizeof(vetorDistancia), 0 , (struct sockaddr *) &si_dest, slen)==-1){
	      			printf("Não consegiu enviar vetor distancia\n");
					die("\n sendto() send_n()\n");
	   			}

				for(int k = 0; k < ordem; k++){
					infoVizinhos[k].timer = clock();
				}

				int j = 1;
				printf("\nVetor distancia enviado = ");
				while (j < N_ROTEADORES){
					printf("%d ", vetorDistancia.vetorCustos[j]);
					j++;
				}
				printf("\n");

				i++;
			}
			close(sockt);
			flagEnvia = 0;

		}
	}
}



//envia mensagem
void send_n(msg mensagem, int prox_roteador, int roteador_atual) {
	
  	int sockt, s;
  	struct sockaddr_in si_dest, si_other;
  	unsigned int slen = sizeof(si_dest);

	//Cria o UDP socket
	if((sockt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
	  	die("socket");
	}

 	memset((char *) &si_dest, 0, sizeof(si_dest));
	si_dest.sin_family = AF_INET;
	si_dest.sin_port = htons(rota[prox_roteador].port + 10);

	unsigned int slen1 = sizeof(si_other);
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(rota[mensagem.origem].port);


	if (inet_aton(rota[prox_roteador].ip , &si_dest.sin_addr) == 0){ //address to number
	    fprintf(stderr, "inet_aton() send_n() failed\n"),
	    exit(1);
	}

	int attempts = ATTEMPTS_S;
	struct timeval timeout;
	enum msg_type timeo = NONE;

	timeout.tv_sec = 1;
	timeout.tv_usec = 10;

	msg ack, ack_falha;
  //tenta mandar a mensagem 3 vezes e verifica se não ocorreu timeout
	while(attempts--){

		memset(ack_falha.mensagem, '\0', sizeof(ack_falha.mensagem));
    	ack_falha.type = FALHA;

		memset(ack.mensagem, '\0', sizeof(ack.mensagem));
    	ack.type = NONE;

	    if (sendto(sockt, &mensagem, sizeof(mensagem), 0 , (struct sockaddr *) &si_dest, slen)==-1){
	      		die("\n sendto() send_n()\n");
	   	}

	   	setsockopt(sockt, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));

	   	if (recvfrom(sockt, &ack, sizeof(ack), 0, (struct sockaddr *) &si_dest, &slen) == -1)
	  		printf("\n....Falha ao enviar. Enviando mensagem novamente...\n");

	   	if(ack.type==NONE) ack_falha.type = FALHA;

	   	timeo = ack.type;

    	if (ack.type == ACK) break;

	}
  //verifica se ocorreu timeout
	if (timeo == NONE){
	 	printf("\n....Timeout.\n");
    	//caso ocorra falha ele retorna mensagem de erro pro roteador que enviou a mensagem
		if(ack_falha.type==FALHA){
			printf("Ocorreu uma falha\n");
		}
	}
	if(timeo == ACK){ //mensagem entregue com sucesso
		  printf("\n....Mensagem %s foi entregue roteador %d \n", mensagem.mensagem,prox_roteador);
  	}
  	else{
  		printf("\n....Mensagem foi perdida\n");

  	}

  	close(sockt);
}


// //ESCREVE MENSAGEM E MANDA PARA O DESTINO
void *sender(int roteador) {

  while(1) {
    //Estrutura para ler mensagem e roteador de destino
    msg m;
	m.teste = 0;
    m.destino = 0;
    m.origem = roteador;
    memset(m.mensagem,0, sizeof(m.mensagem));

    printf("\n>>ID do roteador de destino:\n");
	scanf("%d", &m.destino);
    getchar();
    if(m.destino > 0 && m.destino < N_ROTEADORES){
		printf("\n>>Entre com a mensagem:\n");
		fgets(m.mensagem, 100, stdin);
		m.mensagem[strlen(m.mensagem) - 1] = '\0';

		//procura o próximo caminho
		prox_roteador = getEnlace(roteador, m.destino);
		printf("\nRoteador %d encaminhando mensagem %s para %d \n",roteador,m.mensagem,prox_roteador);
		send_n(m, prox_roteador, roteador);
	}
	else printf("Esse roteador não existe\n");

  }
  pthread_exit(NULL);
}



//fica aguardando receber mensagem (confirmação, erro, ou pacote)
void *receiver(int roteador){

	struct sockaddr_in si_me, si_other;
    int s, i, slen = sizeof(si_other) , recv_len;
    int myport = rota[roteador].port + 10;

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    // zera a estrutura
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(myport);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //liga o socket e a porta
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
    //fica esperando chegar alguma mensagem

    while(1){
 		//estrutura para receber a mensagem
 		msg mens;

        mens.destino = mens.origem = 0;
        mens.type = NONE;
        memset(mens.mensagem,0,sizeof(mens.mensagem));
        fflush(stdout);

        if ((recv_len = recvfrom(s, &mens, sizeof(mens), 0, (struct sockaddr *) &si_other, &slen)) == -1){
            die("\nrecvfrom() receiver()\n");
        }
		
		if(mens.teste == 1){     //se for vetor distancia, recalcula tab Roteamento

			int j = 1;
			printf("\nVetor distancia recebido = ");
			while (j < N_ROTEADORES){
				printf("%d ", mens.vetorCustos[j]);
				j++;
			}
			printf("\n");

			atualizaTabRoteamento(mens);
			printaTabelaRoteamento();
			
		}
        else{
			//verifica se o roteador estanciado é o destino final da mensagem
			if(mens.destino == roteador){
				printf("\n ## Pacote recebido de %d | mensagem > %s < \n",mens.origem,mens.mensagem);
			//envia confirmação de recebimento
				msg ack;
				memset(ack.mensagem, '\0', sizeof(ack.mensagem));
				ack.type = ACK;

				if (sendto(s, &ack, sizeof(ack), 0, (struct sockaddr*) &si_other, slen) == -1){
						die("\nsendto() receiver()\n");
				}
			}
			else if (mens.type==FALHA){ // verifica se não ocorreu erro em entregar o pacote
				printf("\n..... pacote perdido \n");

			}else{ //confirma recebimento e envia para o próximo até chegar ao destino
					msg ack;
					memset(ack.mensagem, '\0', sizeof(ack.mensagem));
					ack.type = ACK;

					if (sendto(s, &ack, sizeof(ack), 0, (struct sockaddr*) &si_other, slen) == -1){
						die("\nsendto() receiver()\n");
					}

					prox_roteador = getEnlace(roteador, mens.destino);
					printf("\nRoteador %d encaminhando mensagem %s para %d \n",roteador,mens.mensagem,prox_roteador);
					send_n(mens,prox_roteador,roteador);
			}
		}

     }

    close(s);
    pthread_exit(NULL);
   return 0;
}
