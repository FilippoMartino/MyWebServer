#include "socket_tcp.hpp"

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

typedef struct {

		//oke
		ServerConnection* conn;
		ServerTcp* myself;

} Params;

void* request(void* params) {

	printf("[Webserver thread] eccomi\n");
  // Mi prendo la risposta da fornire al client
  static char* home =	 strdup(HOME);

	printf("[Webserver thread] Ho copiato la stringa di risposta in una variabile\n");
  // Lo si fa per il casting
  Params* p = (Params*) params;
	printf("[Webserver thread] Ho castato la struttura in arrivo in una varabile\n");
  // Salvo i valori puntati in due variabili locali
  ServerConnection* conn = (ServerConnection*) p->conn;
  ServerTcp* myself = (ServerTcp*) p->myself;
	printf("[Webserver thread] Ho creato le due variabili e ho inserito in loro i valori in arrivo dalla struttura\n");
  // elimino i parametri dallo heap
	printf("[Webserver thread] Ho deallocato la struttura dallo heap\n");
  free(p);
  // Ricevo le richieste
  char* richiesta_client = conn->receive_message();
	printf("[Webserver thread] Ho ricevuto la richiesta dal client (%s)\n", richiesta_client);
  // Invio le risposte
  conn->send_message(home);
	printf("[Webserver thread] Ho inviato la risposta\n");
		// Disconnetto questa conn dalla lista
  myself->disconnect(conn);
	printf("[Webserver thread] Mi sono disconnesso\n");
  // Termino il thread
  pthread_exit(NULL);

}

int main (int argc, char const *argv[]){

  int port = atoi(argv[1]);

		// Idea di Wang, far diventare questo un SINGLETON (DP GOF)
  ServerTcp* myself = new ServerTcp(port);

	printf("[webServer] Ho creato l'oggetto\n");

  while(true){

    ServerConnection* conn = myself->accept_connection();

		printf("[webServer] Ho accettato la connessione\n");
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

				printf("[webServer] Ho creato lo spazio in memoria per la strutture Params\n");
				// Assegno i valori all'area di memoria
				params->conn = conn;
				params->myself = myself;

				printf("[webServer] Ho assegnato le variabili alla struttura\n");

				// Alloco il thread_id
    pthread_t thread_id;
    // Creo il thread che verrà messo in coda dal gestore di processi
    pthread_create(&thread_id, NULL, request,	 (void*) params);

		printf("[webServer] Ho creato il thread\n");

  }

	printf("Sto per fare la delete\n");

  delete(myself);

	printf("[webServer] Ho deallocato lo spazio occupato dall'oggetto serverTcp\n");

  return 0;

}
