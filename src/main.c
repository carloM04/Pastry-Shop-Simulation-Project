#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define HEAP_DIM 80000 
#define MAX_CAR 20 
#define CATALOGO_HASH_SIZE 8192 
#define MAGAZZINO_HASH_SIZE	1024 
#define CATALOGO 0
#define MAGAZZINO 1

//tipo di dato che definisce un ingrediente nel catalogo.

typedef struct ingrediente{

	int quantita, slot;
	char nome[MAX_CAR];
	struct ingrediente *next;

} ingrediente;

//tipo di dato che definisce una ricetta nell'albero.

typedef struct ricetta{

	int quantita;
	char nome[MAX_CAR];
	struct ricetta *dx, *sx, *padre; 
	ingrediente *entra;

} ricetta;

//tipo di dato che definisce un lotto.

typedef struct lotto{

	int quantita, data_scadenza, quantita_tot;
	char nome[MAX_CAR];
	struct lotto *dx, *sx, *padre, *entra;

} lotto;

//tipo di dato che definisce un ordine in attesa.

typedef struct ordine_attesa{

	int peso, istante_di_arrivo, numero_elementi_ordinati;
	char nome[MAX_CAR];
	struct ordine_attesa *next;
	struct ricetta *ricetta_ordinata;
	

} ordine_attesa;

//tipo di dato che definisce un ordine pronto.

typedef struct ordine_pronto{

	int peso, istante_di_arrivo, numero_elementi_ordinati;
	char nome[MAX_CAR];
	struct ordine_pronto *next;

} ordine_pronto;

//Dichiarazione della tabella hash che rappresenta il catalogo delle ricette

ricetta *catalogo[CATALOGO_HASH_SIZE];

//Dichiarazione della tabella hash che rappresenta il magazzino

lotto *magazzino[MAGAZZINO_HASH_SIZE];

//dichiarazione della lista degli ordini pronti.

ordine_pronto *lista_pronti = NULL;

//dichiarazione dell'heap che gestisce gli ordini in attesa.

ordine_attesa *heap_attesa[HEAP_DIM];

/*check serve per assicurarsi che la fscanf funzioni correttamente.
dim_attuale_heap e' la dimensione dell'heap degli ordini in attesa.
tempo e' la variabile che rappresenta il tempo.*/

int check, dim_attuale_heap = 0;
unsigned int tempo = 0;

//prototipi delle funzioni

//funzione hash.

uint32_t hash(char *str, int tabella);

/*aggiunge una ricetta al catalogo se non e' pieno e se la ricetta
non e' gia' presente*/

void aggiungi_ricetta(char *ricetta);  

/*cerca nella tabella hash specificata
il nodo con chiave indicata. Restituisce
NULL se non esiste, il puntatore al nodo se esiste*/

ricetta *cerca_tabella_hash_catalogo(char *chiave, int slot);
lotto *cerca_tabella_hash_magazzino(char *chiave, int slot);

/*aggiunge alla tabella specificata 
l'elemento con chiave indicata nel relativo slot*/

void aggiungi_tabella_hash(char *chiave, int tabella, int slot); 

/*Aggiunge un nodo alla lista circolare puntata da una ricetta (testa)*/

ingrediente *aggiungi_nodo_catalogo(ricetta **testa, char *nome, int quantita);

/*Aggiunge un nodo alla lista avente come testa un lotto. Il lotto viene inserito
in ordine crescente di scadenza*/

void aggiungi_nodo_magazzino(lotto **testa, char *nome, int quantita, int data_scadenza);

/*rimuove una ricetta dal catalogo. Non ha effetto se la ricetta non esiste o
se e' tra gli ordini pronti o in attesa di essere preparati*/

void rimuovi_ricetta(char *nome);

/*rifornisce il magazzino leggendo il seguito del comando "rifornisci"*/

void rifornisci(void);

/*controlla se ci sono lotti scaduti nel magazzino, nel qual caso gli rimuove
da esso.*/

void controlla_scadenze(void);

//verifica se gli ordini attualmente in attesa possono essere preparati

void verifica_ordini_attesa(void);

/*viene chiamata ogni qualvolta si riceve il comando "ordine". Se la ricetta
da ordinare non esiste rifiuta l'ordine_attesa. Se invece esiste, in base alla
disponibilita' degli ingredienti mette l'ordine_attesa nell'heap degli ordini
in attesa o nella lista degli ordini pronti.*/

void ordina(int istante_di_arrivo);

/*le due funzioni che seguono lavorano insieme. Ossia la seconda viene chiamata
solo se la prima restituisce 1. La prima verifica che nel magazzino siano
disponibili gli ingredienti per preparare la ricetta del puntatore indicato in
numero pari a "numero_elementi". Restituisce quindi 1 se sono disponibili,
0 se no. La seconda rimuove dal magazzino gli ingredienti usati per 
preparare la ricetta identificata dal puntatore in numero pari a "numero_elementi"*/

int verifica_presenza_ingredienti(ricetta *p, int numero_elementi);

void rinnova_magazzino(ricetta *p, int numero_elementi);

//inseriscono un nodo nella lista indicata

void inserisci_lista_pronti(int istante_di_arrivo, int peso, char *nome, int numero_elementi_ordinati);

void inserisci_lista_attesa(int instante_di_arrivo, int peso, char *nome, int numero_elementi_ordinati, ricetta *ricetta_ordinata);

/*Va chiamata ogni volta che passa il corriere. Carica gli ordini pronti sul
corriere, rimuovendoli quindi dalla coda dei pronti, e stampa in uscita il
risultato*/

void carica_corriere(int capienza);

//libera le tabelle hash(non usata per risparmiare tempo)

void libera_tabelle_hash(void);

//inserisce una nuova ricetta nel catalogo.

ricetta *inserisci_albero_catalogo(int slot, char *nome_ricetta);

/*Se la sorgente e' NULL allora inserisci un lotto di un ingrediente che 
ancora non esiste nel magazzino. Se la sorgente non e' NULL allora 
inserisce un lotto di un ingrediente gia' esistente con data di scadenza
piu' piccola*/

void inserisci_albero_magazzino(lotto *sorgente, int slot, char *nome, int quantita, int data_scadenza);

//restituiscono il successore del sotto albero con la radice indicata

lotto *trova_successore_magazzino(lotto *radice);
ricetta *trova_successore_catalogo(ricetta *radice);

//controlla se ci sono lotti scaduti nell'albero indicato

void visita_albero_e_controlla(lotto *radice, int slot);

//rimuovono il nodo indicato dall'albero indicato

void rimuovi_nodo_albero_catalogo(ricetta *rim, int slot);
void rimuovi_nodo_albero_magazzino(lotto *rim, int slot);

//deallocano un albero la cui radice e' indicata

void visita_rimuovendo_catalogo(ricetta *radice);
void visita_rimuovendo_magazzino(lotto *radice);


int main(void){

	unsigned int periodicita_corriere, capienza, i;
	char car, str[MAX_CAR];

	for(i = 0;i < CATALOGO_HASH_SIZE;i++){
	
		catalogo[i] = NULL;

	}

	for(i = 0;i < MAGAZZINO_HASH_SIZE;i++){

		magazzino[i] = NULL;

	}

	check = fscanf(stdin, "%d", &periodicita_corriere);

	if(check != 1){

		perror("Errore fscanf");
		exit(1);

	}

	check = fscanf(stdin, "%d", &capienza);

	if(check != 1){

		perror("Errore fscanf");
		exit(1);

	}

	getc(stdin);

	car = getc(stdin);

	while(1){

		if(tempo % periodicita_corriere == 0 && tempo != 0){

			carica_corriere(capienza);

		}
	

		if(car == 'a'){

			while(car != ' ')
				car = getc(stdin);

			check = fscanf(stdin, "%s", str);

			if(check != 1){

				perror("Errore fscanf");
				exit(1);

			}

			aggiungi_ricetta(str);
			tempo++;

		}
		else if(car == 'o'){

			while(car != ' ')
				car = getc(stdin);

			ordina(tempo);6
			tempo++;

		}
		else if(car == 'r'){

			getc(stdin);
			car = getc(stdin);

			if(car == 'f'){

				while(car != ' ')
					car = getc(stdin);

				rifornisci();
				verifica_ordini_attesa();
				tempo++;

			}
			else{

				while(car != ' ')
					car = getc(stdin);

				check = fscanf(stdin, "%s", str);

				if(check != 1){

					perror("Errore fscanf");
					exit(1);

				}

				rimuovi_ricetta(str);
				tempo++;


			}

		}
		else{

			perror("Errore, comando non valido.\n");
			exit(1);

		}

		car = getc(stdin);

		if(car == EOF)
			break;

		if(car == '\n'){

			car = getc(stdin);

			if(car == EOF)
				break;

		}
		
		controlla_scadenze();
	}

	if(tempo % periodicita_corriere == 0 && tempo != 0){

		carica_corriere(capienza);

	}

	return 0;
}

uint32_t hash(char *str, int tabella){

	uint32_t valore_hash = 0;

	for(int i = 0; str[i] != '\0';i++){

		valore_hash += str[i];
		valore_hash *=str[i];

	}

	if(tabella == CATALOGO)
		return valore_hash % CATALOGO_HASH_SIZE;
	else
		return valore_hash % MAGAZZINO_HASH_SIZE;

}

void aggiungi_ricetta(char *ricetta){

	uint32_t slot;
	char car;

	slot = hash(ricetta, CATALOGO);

	if(cerca_tabella_hash_catalogo(ricetta, slot) != NULL){

		printf("ignorato\n");

		//pulisco la linea

		car = getc(stdin);

		while(car != '\n')
			car = getc(stdin);

		return;
	}

	aggiungi_tabella_hash(ricetta, CATALOGO, slot);
	printf("aggiunta\n");
	return;


}


ricetta *cerca_tabella_hash_catalogo(char *chiave, int slot){

	int ris;
	ricetta *cerca;

	cerca = catalogo[slot];

	while(cerca != NULL){

		ris = strcmp(chiave, cerca->nome);

		if(ris > 0){

			cerca = cerca->dx;

		}
		else if(ris < 0){

			cerca = cerca->sx;

		}
		else{

			return cerca;

		}

	}

	return NULL;

}

lotto *cerca_tabella_hash_magazzino(char *chiave, int slot){

	int ris;
	lotto *cerca;

	cerca = magazzino[slot];

	while(cerca != NULL){

		ris = strcmp(chiave, cerca->nome);

		if(ris > 0){

			cerca = cerca->dx;

		}
		else if(ris < 0){

			cerca = cerca->sx;

		}
		else{

			return cerca;

		}

	}
	
	return NULL;
}


void aggiungi_tabella_hash(char *chiave, int tabella, int slot){

	char str[MAX_CAR], car;
	int peso, scadenza;

	if(tabella == CATALOGO){

		int peso_tot = 0;

		ricetta *nuova_ricetta = inserisci_albero_catalogo(slot, chiave);
		ingrediente *inserito = NULL, *p;

		car = getc(stdin);

		while(car != '\n'){
	
			check = fscanf(stdin, "%s", str);

			if(check != 1){

				perror("Errore fscanf");
				exit(1);

			}

			check = fscanf(stdin, " %d", &peso);

			peso_tot += peso;
			
			if(check != 1){

				perror("Errore fscanf");
				exit(1);

			}

			p = aggiungi_nodo_catalogo(&nuova_ricetta, str, peso);

			if(p != NULL){

				inserito = p;

			}

			car = getc(stdin);

		}

		nuova_ricetta->quantita = peso_tot;

		if(inserito != NULL)
			inserito->next = nuova_ricetta->entra;

	}

	else{

		lotto *p;

		check = fscanf(stdin, " %d", &peso);

		if(check != 1){

			perror("Errore fscanf");
			exit(1);
		}

		check = fscanf(stdin, " %d", &scadenza);
		
		if(check != 1){

				perror("Errore fscanf");
				exit(1);

		}

		if(scadenza > tempo){

			p = cerca_tabella_hash_magazzino(chiave, slot);

			if(p == NULL){

				inserisci_albero_magazzino(NULL, slot, chiave, peso, scadenza);	

			}
			else{

				if(p->data_scadenza >= scadenza){

					inserisci_albero_magazzino(p, slot, chiave, peso, scadenza);

				}
				else{

					p->quantita_tot += peso;
					aggiungi_nodo_magazzino(&p, chiave, peso, scadenza);

				}

			}

		}

		car = getc(stdin);

		while(car != '\n'){
	
			check = fscanf(stdin, "%s", str);

			if(check != 1){

				perror("Errore fscanf");
				exit(1);

			}	
			
			check = fscanf(stdin, " %d", &peso);

			if(check != 1){

				perror("Errore fscanf");
				exit(1);

			}

			check = fscanf(stdin, " %d", &scadenza);

			if(check != 1){

				perror("Errore fscanf");
				exit(1);

			}

			if(scadenza > tempo){

				uint32_t slot;

				slot = hash(str, MAGAZZINO);
				p = cerca_tabella_hash_magazzino(str, slot);

				if(p == NULL){

					inserisci_albero_magazzino(NULL, slot, str, peso, scadenza);

				}
				else{

					if(p->data_scadenza >= scadenza){

						inserisci_albero_magazzino(p, slot, str, peso, scadenza);

					}
					else{

						p->quantita_tot += peso;
						aggiungi_nodo_magazzino(&p, str, peso, scadenza);

					}

				}
	
			}

			car = getc(stdin);

		}

	}

}


ingrediente *aggiungi_nodo_catalogo(ricetta **testa, char *nome, int quantita){

	ingrediente *nuovo_nodo = malloc(sizeof(ingrediente));

	if(nuovo_nodo == NULL){

		perror("Errore malloc.\n");
		exit(1);

	}

	strcpy(nuovo_nodo->nome, nome);
	nuovo_nodo->quantita = quantita;
	nuovo_nodo->next = (*testa)->entra;
	nuovo_nodo->slot = hash(nome, MAGAZZINO);
	(*testa)->entra = nuovo_nodo;	

	if(nuovo_nodo->next == NULL){

		return nuovo_nodo;

	}
	
	return NULL;
	
}

void aggiungi_nodo_magazzino(lotto **testa, char *nome, int quantita, int data_scadenza){

	lotto *nuovo_nodo = malloc(sizeof(lotto)), *prec = *testa, *succ = (*testa)->entra;

	if(nuovo_nodo == NULL){

		perror("Errore malloc.\n");
		exit(1);

	}

	if((*testa)->data_scadenza >= data_scadenza){

		perror("Errore critico scadenza testa lista.\n");
		exit(1);

	}

	strcpy(nuovo_nodo->nome, nome);
	nuovo_nodo->quantita_tot = 0;
	nuovo_nodo->quantita = quantita;
	nuovo_nodo->data_scadenza = data_scadenza;
	nuovo_nodo->dx = NULL;
	nuovo_nodo->sx = NULL;
	nuovo_nodo->padre = NULL;

	for(;succ != NULL; prec = succ, succ = succ->entra){

		if(succ->data_scadenza >= data_scadenza)
			break;

	}

	prec->entra = nuovo_nodo;
	nuovo_nodo->entra = succ; 

}

void rimuovi_ricetta(char *nome){

	uint32_t slot = hash(nome, CATALOGO);
	int i;
	ricetta *p = cerca_tabella_hash_catalogo(nome, slot);

	//caso 1: ricetta non presente nel catalogo

	if(p == NULL){

		printf("non presente\n");

	}
	else{

		//caso 2: ricetta presente ma ordinata

		for(i = 0;i < dim_attuale_heap;i++){

			if(!strcmp((heap_attesa[i]->nome), nome)){

				printf("ordini in sospeso\n");
				return;

			}

		}

		ordine_pronto *q = lista_pronti;

		while(q != NULL){

			if(!strcmp(q->nome, nome)){

				printf("ordini in sospeso\n");
				return;

			}

			q = q->next;

		}

		//caso 3: ricetta si puo' eliminare

		rimuovi_nodo_albero_catalogo(p, slot);

		printf("rimossa\n");

	}
}

//lenta

void rifornisci(void){

	char str[MAX_CAR];
	uint32_t slot;

	check = fscanf(stdin, "%s", str);

	if(check != 1){

			perror("Errore fscanf");
			exit(1);

	}

	slot = hash(str, MAGAZZINO);

	aggiungi_tabella_hash(str, MAGAZZINO, slot);

	printf("rifornito\n");

}

//lenta ma va tenuta

void controlla_scadenze(void){

	int i;

	for(i = 0;i < MAGAZZINO_HASH_SIZE;i++){

		if(magazzino[i] != NULL){
			//potenzialmente molto lenta
			visita_albero_e_controlla(magazzino[i], i);	
			
		}

	}

}

//da velocizzare con un heap al posto della lista attesa

void verifica_ordini_attesa(void){ 

	int dim_nuova_heap = dim_attuale_heap, i;

	if(dim_attuale_heap == 0){

		return;

	}

	for(i = 0;i < dim_attuale_heap;i++){

		if(verifica_presenza_ingredienti(heap_attesa[i]->ricetta_ordinata, (heap_attesa[i])->numero_elementi_ordinati)){

			dim_nuova_heap--;
			rinnova_magazzino(heap_attesa[i]->ricetta_ordinata, (heap_attesa[i])->numero_elementi_ordinati);
			inserisci_lista_pronti((heap_attesa[i])->istante_di_arrivo, (heap_attesa[i])->peso, (heap_attesa[i])->nome, (heap_attesa[i])->numero_elementi_ordinati);

			free(heap_attesa[i]);
			heap_attesa[i] = NULL;

		}

	}

	if(dim_nuova_heap != 0 && dim_nuova_heap != dim_attuale_heap){

		int pos_vuota = 0, cont;

		while(heap_attesa[pos_vuota] != NULL)
			pos_vuota++;

		for(i = 0, cont = 0; cont < dim_nuova_heap;i++){

			if(heap_attesa[i] != NULL){

				cont++;

				if(i > pos_vuota){

					heap_attesa[pos_vuota] = heap_attesa[i];
					heap_attesa[i] = NULL;


					if(cont < dim_nuova_heap){

						while(heap_attesa[pos_vuota] != NULL)
							pos_vuota++;

					}
				}
			}

		}
		
	}

	dim_attuale_heap = dim_nuova_heap;

}

//da velocizzare

int verifica_presenza_ingredienti(ricetta *p, int numero_elementi){

	lotto *q;
	ingrediente *scorri;
	int quantita_necessaria;

	scorri = p->entra; 
	
	quantita_necessaria = (scorri->quantita) * numero_elementi;

	q = cerca_tabella_hash_magazzino(scorri->nome, scorri->slot);

	if(q == NULL)
		return 0;

	if(q->quantita_tot < quantita_necessaria)
		return 0;

	scorri = scorri->next;

	while(scorri != p->entra){

		quantita_necessaria = (scorri->quantita) * numero_elementi;

		q = cerca_tabella_hash_magazzino(scorri->nome, scorri->slot);

		if(q == NULL){

			p->entra = scorri;
			return 0;

		}

		if(q->quantita_tot < quantita_necessaria){

			p->entra = scorri;
			return 0;

		}
			
		scorri = scorri->next;

	}

	return 1;

}

void rinnova_magazzino(ricetta *p, int numero_elementi){

	lotto *prec, *succ, *sorgente;
	int quantita_necessaria,  quantita_attuale, quantita_tot;
	uint32_t ris;
	ingrediente *scorri;

	scorri = p->entra;

	while(1){

		quantita_necessaria = (scorri->quantita) * numero_elementi;	
		ris = hash(scorri->nome, MAGAZZINO);
		sorgente = cerca_tabella_hash_magazzino(scorri->nome, ris);

		if(sorgente == NULL){

			perror("Errore critico.\n");
			exit(1);

		}


		succ = sorgente;
		prec = NULL;
		quantita_attuale = 0;	
		quantita_tot = succ->quantita_tot;
		
		while(quantita_attuale < quantita_necessaria){
			
			if(succ == NULL){

				printf("Errore critico rinnovo.\n");
				exit(1);

			}

			quantita_attuale += succ->quantita;

			if(quantita_attuale > quantita_necessaria){

				quantita_tot -= (quantita_necessaria - (quantita_attuale - succ->quantita));
				succ->quantita_tot = quantita_tot;
				succ->quantita = succ->quantita - (quantita_necessaria - (quantita_attuale - succ->quantita));

			}
			else{

				quantita_tot -= succ->quantita;
				prec = succ;

				succ = succ->entra;

				if(prec != sorgente){

					free(prec);

				}

				if(quantita_attuale == quantita_necessaria && succ != NULL)
					succ->quantita_tot = quantita_tot;

			}

		}
		
		if(succ == NULL){

			rimuovi_nodo_albero_magazzino(sorgente, ris);				

		}
		else if(succ != sorgente){
			
			succ->dx = sorgente->dx;
			succ->sx = sorgente->sx;
			succ->padre = sorgente->padre;

			if(sorgente == magazzino[ris]){

				magazzino[ris] = succ;

			}
			else{

				if((sorgente->padre)->dx == sorgente){

					(sorgente->padre)->dx = succ;

				}
				else{

					(sorgente->padre)->sx = succ;
		
				}

			}

			if(sorgente->dx != NULL){

				(sorgente->dx)->padre = succ;

			}
			if(sorgente->sx != NULL){

				(sorgente->sx)->padre = succ;

			}

			free(sorgente);

		}

		scorri = scorri->next;

		if(scorri == p->entra)
			break;
	}

}

void inserisci_lista_pronti(int istante_di_arrivo, int peso, char *nome, int numero_elementi_ordinati){

	ordine_pronto *nuovo_nodo = malloc(sizeof(ordine_pronto)), *succ, *prec;

	if(nuovo_nodo == NULL){

		perror("Errore malloc.\n");
		exit(1);

	}

	nuovo_nodo->istante_di_arrivo = istante_di_arrivo;
	nuovo_nodo->peso = peso;
	nuovo_nodo->numero_elementi_ordinati = numero_elementi_ordinati;
	strcpy(nuovo_nodo->nome, nome);

	succ = lista_pronti;
	prec = NULL;

	if(succ != NULL){

		for(;succ != NULL;prec = succ, succ = succ->next){

			if(succ->istante_di_arrivo > istante_di_arrivo)
				break;

		}

		if(prec == NULL){

			lista_pronti = nuovo_nodo;

		}
		else{

			prec->next = nuovo_nodo;

		}

	}
	else{

		lista_pronti = nuovo_nodo;

	}

	nuovo_nodo->next = succ;

}


void inserisci_lista_attesa(int istante_di_arrivo, int peso, char *nome, int numero_elementi_ordinati, ricetta *ricetta_ordinata){

	ordine_attesa *nuovo_nodo = malloc(sizeof(ordine_attesa));

	if(nuovo_nodo == NULL){

		perror("Errore malloc.\n");
		exit(1);

	}

	nuovo_nodo->istante_di_arrivo = istante_di_arrivo;
	nuovo_nodo->peso = peso;
	nuovo_nodo->numero_elementi_ordinati = numero_elementi_ordinati;
	strcpy(nuovo_nodo->nome, nome);
	nuovo_nodo->ricetta_ordinata = ricetta_ordinata;

	heap_attesa[dim_attuale_heap - 1] = nuovo_nodo;


}

void ordina(int istante_di_arrivo){

	char str[MAX_CAR];
	int numero_elementi_ordinati, peso;
	ricetta *p;
	uint32_t slot;

	check = fscanf(stdin, "%s", str);

	if(check != 1){

		perror("Errore fscanf");
		exit(1);

	}

	slot = hash(str, CATALOGO);

	p = cerca_tabella_hash_catalogo(str, slot); 

	check = fscanf(stdin, " %d", &numero_elementi_ordinati);

	if(check != 1){

		perror("Errore fscanf");
		exit(1);

	}

	//caso 1: ricetta non presente nel catalogo

	if(p == NULL){

		printf("rifiutato\n");
		return;
	
	}

	//caso 2: ricetta presente nel catalogo

	peso = p->quantita * numero_elementi_ordinati;

	printf("accettato\n");

	if(verifica_presenza_ingredienti(p, numero_elementi_ordinati)){
		
		rinnova_magazzino(p, numero_elementi_ordinati);
		inserisci_lista_pronti(istante_di_arrivo, peso, str, numero_elementi_ordinati);  	


	}
	else{

		dim_attuale_heap++;

		if(dim_attuale_heap >= HEAP_DIM){

			perror("Errore heap attesa pieno\n");
			exit(1);

		}

		inserisci_lista_attesa(istante_di_arrivo, peso, str, numero_elementi_ordinati, p);

	}

}

void carica_corriere(int capienza){

	ordine_pronto *limite = lista_pronti, *prec, *max;
	unsigned int carico = 0;

	//nessun ordine_attesa in "lista_pronti"

	if(limite == NULL){

		printf("camioncino vuoto\n");
		return;

	}

	//il peso del primo ordine_attesa arrivato supera la capienza del camioncino

	if(limite->peso > capienza){

		printf("camioncino vuoto\n");
		return;

	}


	/*identificazione degli ordini da caricare, in ordine_attesa cronologico
	di arrivo. Il puntatore "limite" punta all'ordine_attesa che sara'
	il nuovo inizio della lista "lista_pronti.*/

	while(limite != NULL){

		if(carico + limite->peso > capienza)
			break;

		carico += limite->peso;
		limite = limite->next;

	}

	/*caricamento sul camioncino degli ordini selezionati, in ordine_attesa
	decrescente di peso, e rimozione degli ordini selezionati dalla
	"lista_pronti"*/

	while(lista_pronti != limite){

		max = lista_pronti;
		prec = lista_pronti->next;

		while(prec != limite){

			if(prec->peso > max->peso)
				max = prec;

			prec = prec->next;
		
		}

		printf("%d %s %d\n", max->istante_di_arrivo, max->nome, max->numero_elementi_ordinati);	

		if(max == lista_pronti){

			lista_pronti = max->next;
			free(max);

		}
		else{

			prec = lista_pronti;

			for(;prec->next != max;prec = prec->next);

			prec->next = max->next;
			free(max);
		}

	}
	
}


void libera_tabelle_hash(void){

	int i;

	for(i = 0;i < CATALOGO_HASH_SIZE;i++){

		if(catalogo[i] != NULL){

			visita_rimuovendo_catalogo(catalogo[i]);	

		}

	}

	for(i = 0;i < MAGAZZINO_HASH_SIZE;i++){

		if(magazzino[i] != NULL){

			visita_rimuovendo_magazzino(magazzino[i]);

		}

	}

}

ricetta *inserisci_albero_catalogo(int slot, char *nome_ricetta){

	ricetta *nuova_ricetta = malloc(sizeof(ricetta)), *p = catalogo[slot];

	if(nuova_ricetta == NULL){

		perror("Errore malloc.\n");
		exit(1);

	}

	strcpy(nuova_ricetta->nome, nome_ricetta);
	nuova_ricetta->quantita = 0;
	nuova_ricetta->dx = NULL;
	nuova_ricetta->sx = NULL;
	nuova_ricetta->entra = NULL;

	if(catalogo[slot] == NULL){

		catalogo[slot] = nuova_ricetta;
		nuova_ricetta->padre = NULL;

	}
	else{

		while(1){

			if(strcmp(nome_ricetta, p->nome) >= 0){

				if(p->dx == NULL){

					p->dx = nuova_ricetta;
					nuova_ricetta->padre = p;
					break;

				}
				else{

					p = p->dx;

				}

			}
			else{

				if(p->sx == NULL){

					p->sx = nuova_ricetta;
					nuova_ricetta->padre = p;
					break;

				}
				else{

					p = p->sx;

				}

			}

		}

	}

	return nuova_ricetta;

}

void inserisci_albero_magazzino(lotto *sorgente, int slot, char *nome, int quantita, int data_scadenza){

	lotto *nuovo_lotto = malloc(sizeof(lotto));

	if(nuovo_lotto == NULL){

		perror("Errore malloc.\n");
		exit(1);

	}

	strcpy(nuovo_lotto->nome, nome);
	nuovo_lotto->data_scadenza = data_scadenza;
	nuovo_lotto->quantita = quantita;
	
	if(sorgente == NULL){

		lotto *p = magazzino[slot];

		nuovo_lotto->quantita_tot = quantita;
		nuovo_lotto->dx = NULL;
		nuovo_lotto->sx = NULL;
		nuovo_lotto->entra = NULL;

		if(magazzino[slot] == NULL){

			magazzino[slot] = nuovo_lotto;
			nuovo_lotto->padre = NULL;

		}
		else{

			while(1){

				if(strcmp(nome, p->nome) >= 0){

					if(p->dx == NULL){

						p->dx = nuovo_lotto;
						nuovo_lotto->padre = p;
						break;

					}
					else{

						p = p->dx;

					}

				}
				else{

					if(p->sx == NULL){

						p->sx = nuovo_lotto;
						nuovo_lotto->padre = p;
						break;

					}
					else{

						p = p->sx;

					}

				}


			}

		}

	}
	else{

		nuovo_lotto->quantita_tot = sorgente->quantita_tot + quantita;
		sorgente->quantita_tot = 0;
		nuovo_lotto->entra = sorgente;
		nuovo_lotto->dx = sorgente->dx;
		nuovo_lotto->sx = sorgente->sx;
		nuovo_lotto->padre = sorgente->padre;

		if(magazzino[slot] == sorgente){

			magazzino[slot] = nuovo_lotto;

		}
		else{

			if((sorgente->padre)->sx == sorgente){

				(sorgente->padre)->sx = nuovo_lotto;

			}
			else{

				(sorgente->padre)->dx = nuovo_lotto;

			}

		}

		if(sorgente->dx != NULL){

			(sorgente->dx)->padre = nuovo_lotto;

		}

		if(sorgente->sx != NULL){

			(sorgente->sx)->padre = nuovo_lotto;

		}

		sorgente->dx = NULL;
		sorgente->sx = NULL;
		sorgente->padre = NULL;

	}

}

ricetta *trova_successore_catalogo(ricetta *radice){

	ricetta *succ = radice->dx;

	if(succ == NULL){

		printf("Successore non esiste.\n");
		exit(1);

	}

	while(succ->sx != NULL){

		succ = succ->sx;

	}

	return succ;

}

void rimuovi_nodo_albero_catalogo(ricetta *rim, int slot){

	ricetta *sost, *canc;
	ingrediente *succ, *prec;

	succ = (rim->entra)->next;

	while(succ != rim->entra){

		prec = succ;
		succ = succ->next;
		free(prec);

	}

	free(rim->entra);


	if(rim->sx == NULL || rim->dx == NULL){

		canc = rim;

	}
	else{

		canc = trova_successore_catalogo(rim);

	}

	if(canc->sx != NULL){
		
		sost = canc->sx;
	
	}
	else{

		sost = canc->dx;

	}

	if(sost != NULL){

		sost->padre = canc->padre;

	}
	
	if(canc->padre == NULL){

		catalogo[slot] = sost;

	}
	else if(canc == (canc->padre)->sx){

		(canc->padre)->sx = sost;

	}
	else{

		(canc->padre)->dx = sost;

	}
	
	if(canc != rim){

		canc->dx = rim->dx;
		canc->sx = rim->sx;
		canc->padre = rim->padre;

		if(rim == catalogo[slot]){

			catalogo[slot] = canc;

		}
		else{

			if(rim == (rim->padre)->dx){

				(rim->padre)->dx = canc;

			}
			else{
				(rim->padre)->sx = canc;
				
			}
		}

		if(rim->dx != NULL){

			(rim->dx)->padre = canc;

		}
		if(rim->sx != NULL){
			
			(rim->sx)->padre = canc;

		}
	}

	free(rim);
	
}


lotto *trova_successore_magazzino(lotto *radice){

	lotto *succ = radice->dx;

	if(succ == NULL){

		printf("Successore non esiste.\n");
		exit(1);

	}

	while(succ->sx != NULL){

		succ = succ->sx;

	}

	return succ;

}

void rimuovi_nodo_albero_magazzino(lotto *rim, int slot){

	lotto *canc, *sost;

	if(rim->sx == NULL || rim->dx == NULL){

		canc = rim;

	}
	else{

		canc = trova_successore_magazzino(rim);

	}

	if(canc->sx != NULL){

		sost = canc->sx;

	}
	else{

		sost = canc->dx;

	}

	if(sost != NULL){

		sost->padre = canc->padre;

	}

	if(canc->padre == NULL){

		magazzino[slot] = sost;

	}
	else if(canc == (canc->padre)->sx){

		(canc->padre)->sx = sost;

	}
	else{
		
		(canc->padre)->dx = sost;
	
	}

	if(canc != rim){

		canc->dx = rim->dx;
		canc->sx = rim->sx;
		canc->padre = rim->padre;

		if(rim == magazzino[slot]){

			magazzino[slot] = canc;

		}
		else{

			if(rim == (rim->padre)->dx){

				(rim->padre)->dx = canc;

			}
			else{
				
				(rim->padre)->sx = canc;

			}
		}

		if(rim->dx != NULL){

			(rim->dx)->padre = canc;

		}

		if(rim->sx != NULL){

			(rim->sx)->padre = canc;

		}
	}

	free(rim);

}

void visita_albero_e_controlla(lotto *radice, int slot){

	static lotto *succ, *prec, *sorgente;
	static int quantita_tot;

	if(radice != NULL){

		visita_albero_e_controlla(radice->sx, slot);
		visita_albero_e_controlla(radice->dx, slot);
		
		if(radice->data_scadenza <= tempo){

			quantita_tot = radice->quantita_tot - radice->quantita;
			sorgente = radice;	
			succ = radice->entra;

			if(succ != NULL){

				while(succ->data_scadenza <= tempo){

					quantita_tot -= succ->quantita;
					prec = succ;
					succ = succ->entra;
					free(prec);

					if(succ == NULL)
						break;
				}
			}
			
			if(succ == NULL){

				rimuovi_nodo_albero_magazzino(sorgente, slot);	

			}
			else{

				succ->quantita_tot = quantita_tot;
				succ->dx = sorgente->dx;
				succ->sx = sorgente->sx;
				succ->padre = sorgente->padre;
				
				if(magazzino[slot] == sorgente){

					magazzino[slot] = succ;

				}
				else{

					if((sorgente->padre)->dx == sorgente){

						(sorgente->padre)->dx = succ;

					}
					else{

						(sorgente->padre)->sx = succ;

					}

				}

				if(sorgente->dx != NULL){

					(sorgente->dx)->padre = succ;

				}
				if(sorgente->sx != NULL){

					(sorgente->sx)->padre = succ;
				
				}

				free(sorgente);

			}

		}

	}

}

void visita_rimuovendo_catalogo(ricetta *radice){

	static ingrediente *succ, *prec;

	if(radice != NULL){

		visita_rimuovendo_catalogo(radice->sx);
		visita_rimuovendo_catalogo(radice->dx);

		succ = (radice->entra)->next;
		
		while(succ != radice->entra){

			prec = succ;
			succ = succ->next;
			free(prec);

		}
	
		free(radice->entra);
		free(radice);
	}

}

void visita_rimuovendo_magazzino(lotto *radice){

	static lotto *succ, *prec;

	if(radice != NULL){

		visita_rimuovendo_magazzino(radice->sx);
		visita_rimuovendo_magazzino(radice->dx);

		succ = radice;

		while(succ != NULL){

			prec = succ;
			succ = succ->entra;
			free(prec);

		}

	}

}


