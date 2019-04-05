#include "./SocketTcp/socket_tcp.hpp"

#define HOME "HTTP/1.1 200 OK\n\
Date: Wed, 19 Apr 2017 16:34:20 GMT\n\
Content-Type: text/html; charset=UTF-8\n\
\n <!DOCTYPE html>\n\
<html class=\"client-nojs\" lang=\"it\" dir=\"ltr\">\n\
<head>\n\
<title>web site</title>\n\
<body>\n\
<h1>Hello world</h1>\n\
</body>\n\
</html>"

#define ERROR_FILE_MISSING "Error 404: file not found"
#define MAXCHAR 1000

typedef struct {

		//oke
		ServerConnection* conn;
		ServerTcp* myself;

} Params;

int get_file_size(char* filename){

	FILE *fp;
	long lSize;

	fp = fopen(filename, "r");
  if (fp == NULL){
      printf("An error has ecoured when opening the file\n");
  }

	fseek (fp , 0 , SEEK_END);
	lSize = ftell(fp);

	return lSize;

}

char* read_file(char* filename){

	FILE *fp;
	long lSize;
	char * buffer;
	size_t result;

  fp = fopen(filename, "r");
  if (fp == NULL){
      printf("An error has ecoured when opening the file\n");
  }

	// calcolo la lunghezza del file
	fseek (fp , 0 , SEEK_END);
	lSize = ftell(fp);

	// torniamo all'inizio del file
	rewind(fp);

	// alloco la memoria per contenere l'intero file
	buffer = (char*) malloc ( sizeof ( char ) * lSize );

	// copio il file nel buffer
	result = fread (buffer, 1, lSize, fp);

	//Se il file non esiste
	if (result != lSize)
		return strdup("Error");

	fclose (fp);

	return buffer;

}

void* request(void* params) {

  // Mi prendo la risposta da fornire al client
  static char* home =	 strdup(HOME);

  // Lo si fa per il casting
  Params* p = (Params*) params;
  // Salvo i valori puntati in due variabili locali
  ServerConnection* conn = (ServerConnection*) p->conn;
  ServerTcp* myself = (ServerTcp*) p->myself;
  // elimino i parametri dallo heap
  free(p);
  // Ricevo le richieste
  char* richiesta_client = conn->receive_message();
  // Invio le risposte

/*	long lSize = get_file_size("index.html");
	char* page = (char*) malloc ( sizeof ( char ) * lSize );


	page = read_file("index.html");
	printf("page ->%s\n", page);

	conn->send_message(page);
	printf("Pagina inviata\n");

*/
    conn->send_message(home);
	// Disconnetto questa conn dalla lista
  myself->disconnect(conn);
  // Termino il thread
  pthread_exit(NULL);

}

int main (int argc, char const *argv[]){

  int port = atoi(argv[1]);

	// Idea di Wang, far diventare questo un SINGLETON (DP GOF)
  ServerTcp* myself = new ServerTcp(port);

  while(true){

    ServerConnection* conn = myself->accept_connection();

    /*

    		Dobbiamo mallocare la uno spazio in memoria
    		con i parametri in modo che sia sempre visibile
    		al thread cui dovrà lavorarci, altrimenti verrebbe
    		deallocata alla fine del while.
    		Così facendo alloco uno spazio nello heap, lo faccio
    		puntare da una variabile e passo la stessa alla funzione
    		pthread_create, che la utilizzerà per il thread stesso.
    		Alla fine del ciclo il puntatore verrà deallocato, senza
    		toccare i parametri nello heap che verranno poi eliminati
    		dal thread dopo averne prelevato i valori.

    */



				Params* params = (Params*) malloc(sizeof(Params));

				// Assegno i valori all'area di memoria
				params->conn = conn;
				params->myself = myself;

		// Alloco il thread_id
    pthread_t thread_id;
    // Creo il thread che verrà messo in coda dal gestore di processi
    pthread_create(&thread_id, NULL, request,	 (void*) params);

  }

  delete(myself);

  return 0;

}
