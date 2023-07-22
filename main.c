#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*struct nodov{
    int autonomia;
    struct nodov *next;
};
typedef struct nodov *veicolo;*/

struct nodos{
    int distanza;
    //veicolo parco;
    int *veicoli;
    int nVeicoli;
    char colore; //r, b
    struct nodos *left, *right, *father;
};
typedef struct nodos *stazione;

struct trees{
    stazione root;
};

typedef struct trees *alberos;
alberos stazioni;
int numStazioni;

struct nodog{
    int g;
    int dist;
    struct nodog *next;
};

struct list{
    struct nodog *head;
    struct list *next;
};
typedef struct list *listaArchi;

struct graph{
    listaArchi staz;
};
typedef struct graph *grafos;
grafos grafo;

struct nodoOpen{
    struct nodog *nodo;
    struct nodoOpen *next;
    struct nodoOpen *p;
};
struct listaOpen{
    struct nodoOpen *head;
};

struct nodop{
    int dist;
    struct nodop *next;
};
struct listap{
    struct nodop *head;
};

void aggiungiStazione(int, int, int []);
void insertFixup(stazione);
void rotazioneS(stazione);
void rotazioneD(stazione);
stazione cercaStazione(stazione, int);
void demolisciStazione(int);
void deleteFixup(stazione);
stazione trovaSuccessore(stazione);
stazione stazioneMinima(stazione);


short aggiungiAuto(stazione, int);
int cercaAuto(stazione, int);
void ordinaAuto(int[], int, int);
void merge(int[], int, int, int);
void rottamaAuto(stazione, int);

void costruisciGrafoAsc(int, int, stazione);
void costruisciGrafoDesc(int, int, stazione);
void aggiungiArchiAsc(int, int, stazione, listaArchi, int);
void aggiungiArchiDesc(int, int, stazione, listaArchi, int);
void pianificaPercorso(int, int);
void liberaLista(struct listaOpen *l);

void costruisciGrafo2(int inizio, int fine, stazione x, int i, int grafo[][numStazioni]);
void aggiungiArchi2(int inizio, int fine, stazione s, int i, int stazioneMax, int dist, int j, int grafo[][numStazioni]);


//da cancellare
void inorder_tree_walk(stazione x)
{
    if(x!=NULL)
    {
        inorder_tree_walk(x->right);
        printf("%d\n", x->distanza);
        inorder_tree_walk(x->left);
    }
}

void stampaLista(){
    listaArchi cur = grafo->staz;
    while(cur!=NULL){
        printf("%d -> ",cur->head->dist);
        struct nodog *curN = cur->head->next;
        while(curN != NULL){
            printf("%d -> ", curN->dist);
            curN = curN->next;
        }
        printf("\n");
        cur = cur->next;
    }
}

int main() {
    char input[100], comando[100];
    stazioni = malloc(sizeof(struct trees));
    stazioni->root = NULL;
    numStazioni=0;

    while(fgets(input, 100, stdin) != NULL ){
        sscanf(input, "%s", comando);
        if(strcmp(comando, "aggiungi-stazione") == 0){
            int dist, numV;
            sscanf(input, "%*s %d %d", &dist, &numV);
            if(dist < 0 || numV < 0)
                puts("non aggiunta");
            else{
                if(numV != 0) {
                    int veicoli[numV], i = 0;
                    char *autonomie = strtok(input, " ");
                    autonomie = strtok(NULL, " ");
                    autonomie = strtok(NULL, " ");
                    while (autonomie != NULL) {
                        autonomie = strtok(NULL, " ");
                        if (autonomie != NULL) {
                            veicoli[i] = atoi(autonomie);
                            if (veicoli[i] < 0) {
                                puts("non aggiunta");
                                break;
                            }
                            i++;
                        }
                    }
                    if(i != numV)
                        puts("non aggiunta");
                    else if(veicoli[i-1] > 0){
                        ordinaAuto(veicoli, 0, numV-1);
                        aggiungiStazione(dist, numV, veicoli);
                    }
                }
                else
                    aggiungiStazione(dist, numV, NULL);


            }
        }
        else if(strcmp(comando, "demolisci-stazione")==0) {
            int dist;
            sscanf(input, "%*s %d", &dist);
            demolisciStazione(dist);
        }

        else if(strcmp(comando, "aggiungi-auto")==0){
            int dist, autonomia;
            sscanf(input, "%*s %d %d", &dist, &autonomia);
            if(dist<0 || autonomia<0)
                puts("non aggiunta");
            else{
                stazione s = cercaStazione(stazioni->root, dist);
                if(s==NULL)
                    puts("non aggiunta");
                else{
                    if (aggiungiAuto(s, autonomia) == 1) {
                        ordinaAuto(s->veicoli, 0, s->nVeicoli-1);
                        puts("aggiunta");
                    } else
                        puts("non aggiunta");
                }
            }
        }
        else if(strcmp(comando, "rottama-auto")==0){
            int dist, autonomia;
            sscanf(input, "%*s %d %d", &dist, &autonomia);
            if(dist<0 || autonomia<0)
                puts("non rottamata");
            else{
                stazione s = cercaStazione(stazioni->root, dist);
                if(s == NULL)
                    puts("non rottamata");
                else
                    rottamaAuto(s, autonomia);
            }
        }
        else if(strcmp(comando, "pianifica-percorso")==0){
            int inizio, fine;
            sscanf(input, "%*s %d %d", &inizio, &fine); //tenere vecchio grafo?
            if(stazioni->root == NULL)
                puts("nessun percorso");
            else{//    if(grafo==NULL) //controlla
                grafo = malloc(sizeof(struct graph));
                grafo->staz = NULL;
                if(inizio == fine)
                    printf("%d\n", inizio);
                else{
                    if (inizio < fine)
                        costruisciGrafoAsc(inizio, fine, stazioni->root);
                    else
                        costruisciGrafoDesc(inizio, fine, stazioni->root);
                    //stampaLista();
                    pianificaPercorso(inizio, fine);
                }
            }

        }
    }
    return 0;
}

void aggiungiArchi2(int inizio, int fine, stazione s, int i, int autonomiaMax, int dist, int j, int grafo[][numStazioni]){//riempio matrice parzialmente
    if(s!=NULL){
        aggiungiArchi2(inizio, fine, s->left, i, autonomiaMax, dist, j, grafo);

        printf("aggiungi archi con %d, j=%d \n", dist, j);
        if((dist <= s->distanza && dist + autonomiaMax >= s->distanza) || (dist > s->distanza && dist - autonomiaMax <= s->distanza))
            grafo[i][j] = 1;
        else
            grafo[i][j] = 0;
        j++;

        aggiungiArchi2(inizio, fine, s->right, i, autonomiaMax, dist, j, grafo);
    }
}

void costruisciGrafo2(int inizio, int fine, stazione x, int i, int grafo[][numStazioni]){
    if(x != NULL){
        costruisciGrafo2(inizio, fine, x->left, i, grafo);
        printf("costruisci grafo con %d, i=%d\n", x->distanza, i);
        aggiungiArchi2(inizio, fine, x, i, x->veicoli[x->nVeicoli-1], x->distanza, 0, grafo);
        printf("ho aggiunto archi di %d\n", x->distanza);
        i++;
        costruisciGrafo2(inizio, fine, x->right, i, grafo);
    }
}

/*void pianificaPercorso2(int inizio, int fine){
    int costi[numStazioni];
    for(int i=1; i<numStazioni; i++)
        costi[i] = INT_MAX;
    costi[0] = 0;

    listaArchi cura = grafo->staz;
    struct nodog *curg;

    while(cura != NULL){
        curg = cura->head;
        while(curg != NULL){
          if(curg->dist/10 < costi[curg->g])
                costi[curg->g] =
        }
    }
}*/

void pianificaPercorso(int inizio, int fine){ //da finire ma non mi convince, aggiungo nodi nel grafo desc?
    struct listaOpen *open = malloc(sizeof (struct listaOpen));
    struct listaOpen *closed = malloc(sizeof(struct listaOpen));
    struct nodog *cur;
    open->head = malloc(sizeof (struct nodoOpen));

    listaArchi curl = grafo->staz;
    /*while(curl!= NULL){
        if(curl->head->dist == inizio) {
            open->head->nodo = curl->head;
            break;
        }
        curl = curl->next;
    }*/
    open->head->nodo = curl->head;
    open->head->next = NULL;
    open->head->p = NULL;
    closed->head = NULL;

    while(1){
        struct nodoOpen *n = open->head;
        // printf("tolgo da open: %d\n", open->head->nodo->dist);
        if(n == NULL){
            puts("nessun percorso");
            break;
        }
        open->head = n->next; //tolgo nodo con costo minore da open

        n->next = closed->head;
        closed->head = n; //inserisco nodo con costo minore in closed

        if(closed->head->nodo->dist == fine)
            break;

        //printf("closed->head->nodo->dist=%d\n", closed->head->nodo->dist );

        listaArchi curNodoG = grafo->staz;
        while(curNodoG != NULL){
            //printf("curNodoG->head->dist: %d\n", curNodoG->head->dist);
            if(curNodoG->head->dist == closed->head->nodo->dist)
                break;
            curNodoG = curNodoG->next;
        }
        if(curNodoG == NULL){
            puts("nessun percorso");
            break;
        }
        cur = curNodoG->head->next;
      //  printf("curNodog: %d\n", curNodoG->head->dist);

        //inserimento ordinato dei nodi adiacenti in open
        while(cur!=NULL){
        //    printf("cur: %d\n", cur->dist);
            cur->g ++;
            struct nodoOpen *curClose = closed->head;
            short isClosed = 0;
            while(curClose != NULL){ //controllo se il nodo non è già stato espanso
                if(cur->dist == curClose->nodo->dist) {
                    isClosed = 1;
                    break;
                }
                curClose = curClose->next;
            }
            if(isClosed == 0) {
                struct nodoOpen *curOpen = open->head;
                struct nodoOpen *prec = NULL;
                while (curOpen != NULL) {
                    //controllo se il nodo è già in open
                    if (cur->dist == curOpen->nodo->dist) {
                        if (cur->g < curOpen->nodo->g)
                            curOpen->nodo->g = cur->g;
                        break;
                    }
                    if (cur->dist / 10 + cur->g < curOpen->nodo->dist/10 + curOpen->nodo->g) {
                        struct nodoOpen *temp = malloc(sizeof(struct nodoOpen));
                        temp->nodo = cur;
                        temp->p = closed->head;
                        if (prec == NULL) {
                            temp->next = open->head;
                            open->head = temp;
                        //    printf("aggiungo a open: %d\n", temp->nodo->dist);
                            break;
                        }
                        temp->next = curOpen;
                        prec->next = temp;
                        break;
                      //  printf("aggiungo a open: %d\n", temp->nodo->dist);
                    }
                    prec = curOpen;
                    curOpen = curOpen->next;
                }
                if (curOpen == NULL) {
                    struct nodoOpen *temp = malloc(sizeof(struct nodoOpen));
                    temp->nodo = cur;
                    temp->p = closed->head;
                    if(prec==NULL){
                        temp->next = open->head;
                        open->head = temp;
                    }
                    else{
                        prec->next = temp;
                        temp->next = NULL;
                    }
                   // printf("aggiungo a open: %d\n", temp->nodo->dist);
                }
            }
            cur = cur->next;
        }
    }

    //controllo se posso
    if(closed->head->nodo->dist == fine) {
        //struct nodoOpen *curcl = closed->head->p;
        struct nodoOpen *curcl = closed->head;
        struct listaOpen *percorso = malloc(sizeof (struct listaOpen));
        //percorso->head = closed->head;
        //percorso->head->next = NULL;
        percorso->head = NULL;

        while(curcl != NULL){

            struct nodoOpen *parent = curcl->p;
            struct nodoOpen *succ = curcl->next;
            while(parent != succ){
                curcl->next = succ->next;
                free(succ);
                succ = curcl->next;
            }

            curcl->next = percorso->head;
            percorso->head = curcl;
            curcl = curcl->p;
        }

        /*while(curcl != NULL){
            curcl->next = percorso->head;
            percorso->head = curcl;
            curcl = curcl->p;
        }*/

        char perc[100];
        curcl = percorso->head;
        sprintf(perc, "%d", curcl->nodo->dist);
        strcat(perc, " ");
        curcl = curcl->next;
        while(curcl != NULL){
            int size = (int)((ceil(log10(curcl->nodo->dist))+1)*sizeof(char));
            char string[size];
            sprintf(string, "%d", curcl->nodo->dist);
            strcat(perc,string);
            if(curcl->nodo->dist != fine)
                strcat(perc, " ");
            curcl = curcl->next;
        }
        puts(perc);
        liberaLista(percorso);
        free(closed);
    }
    else
        liberaLista(closed);
    liberaLista(open);
    //liberaLista(closed);

    curl = grafo->staz;
    listaArchi precl = NULL;
    while(curl != NULL){
        cur = curl->head;
        struct nodog *prec = NULL;
        while(cur!=NULL){
            prec = cur;
            cur = cur->next;
            free(prec);
        }
        precl = curl;
        curl = curl->next;
        free(precl);
    }
    free(grafo);
    grafo = NULL;
}

void liberaLista(struct listaOpen *l){
    struct nodoOpen *cur = l->head;
    struct nodoOpen *prec = NULL;
    while(cur != NULL){
        prec = cur;
        cur = cur->next;
        free(prec);
    }
    free(l);
}

void costruisciGrafoAsc(int inizio, int fine, stazione x){ //anche al contrario!
    if(x != NULL){
        costruisciGrafoAsc(inizio, fine, x->right);

        if(x->distanza>=inizio && x->distanza<=fine){
            struct list *temp = malloc(sizeof(struct list));
            temp->head = malloc(sizeof(struct nodog));
            temp->head->dist = x->distanza; //da controllare
            temp->head->next = NULL;
            //temp->head->g = i;
            temp->head->g=0;
            temp->next = grafo->staz;
            grafo->staz = temp;
            if(x->distanza < fine)
                aggiungiArchiAsc(inizio, fine, stazioni->root, temp, x->veicoli[x->nVeicoli-1]);
            }
        costruisciGrafoAsc(inizio, fine, x->left); //invertiti per leggere i valori dal maggiore al minore
    }
}

void costruisciGrafoDesc(int inizio, int fine, stazione x){ //anche al contrario!
    if(x != NULL){
        costruisciGrafoDesc(inizio, fine, x->left);

        if(x->distanza>=fine && x->distanza<=inizio){
            struct list *temp = malloc(sizeof(struct list));
            temp->head = malloc(sizeof(struct nodog));
            temp->head->dist = x->distanza; //da controllare
            temp->head->next = NULL;
            temp->head->g=0;
            temp->next = grafo->staz;
            grafo->staz = temp;
            //printf("aggiungo a grafo: %d -> ", temp->head->dist);
            if(x->distanza > fine)
                aggiungiArchiDesc(inizio, fine, stazioni->root, temp, x->veicoli[x->nVeicoli-1]);
            //printf("\n");
        }

        costruisciGrafoDesc(inizio, fine, x->right); //invertiti per leggere i valori dal maggiore al minore
    }
}

void aggiungiArchiAsc(int inizio, int fine, stazione s, listaArchi nodo, int autonomiaMax){
    if(s!=NULL){
        aggiungiArchiAsc(inizio, fine, s->right, nodo, autonomiaMax);
        //printf("inizio=%d e nodo->head->dist=%d\n", inizio, nodo->head->dist);
        if(nodo->head->dist != s->distanza && s->distanza <= fine && s->distanza >= inizio &&
            ((nodo->head->dist <= s->distanza && nodo->head->dist + autonomiaMax >= s->distanza) ||
            (nodo->head->dist > s->distanza && nodo->head->dist - autonomiaMax <= s->distanza))){
            //printf("aggiungo arco: %d\n", s->distanza);
            struct nodog *temp = malloc(sizeof(struct nodog));
            temp->dist = s->distanza;
            temp->g = 0;
            temp->next = nodo->head->next;
            nodo->head->next = temp;
            //printf("%d -> ", temp->dist);
        }

        aggiungiArchiAsc(inizio, fine, s->left, nodo, autonomiaMax);
    }
}

void aggiungiArchiDesc(int inizio, int fine, stazione s, listaArchi nodo, int autonomiaMax){
    if(s!=NULL){
        aggiungiArchiDesc(inizio, fine, s->left, nodo, autonomiaMax);

        if(nodo->head->dist != s->distanza && s->distanza >= fine && s->distanza <= inizio &&
            ((nodo->head->dist <= s->distanza && nodo->head->dist + autonomiaMax >= s->distanza) ||
            (nodo->head->dist > s->distanza && nodo->head->dist - autonomiaMax <= s->distanza))){
            struct nodog *temp = malloc(sizeof(struct nodog));
            temp->dist = s->distanza;
            temp->g = 0;
            temp->next = nodo->head->next;
            nodo->head->next = temp;
            //printf("%d -> ", temp->dist);
        }

        aggiungiArchiDesc(inizio, fine, s->right, nodo, autonomiaMax);
    }
}

void aggiungiStazione(int dist, int numV, int v[]){
    stazione temp = malloc(sizeof(struct nodos));
    temp->distanza = dist;
    temp->veicoli = malloc(sizeof(int) * 152);
    temp->nVeicoli = 0;
    if(numV != 0){
        for (int i = 0; i < numV; i++) {
            if (aggiungiAuto(temp, v[i]) == 0) {
                free(temp->veicoli);
                free(temp);
                puts("non aggiunta");
                return;
            }
        }
        ordinaAuto(temp->veicoli, 0, numV-1);
    }
    if(stazioni->root == NULL){
        temp->left = NULL;
        temp->right = NULL;
        temp->father = NULL;
        stazioni->root = temp;
        insertFixup(temp);
        puts("aggiunta");
        numStazioni++;
        return ;
    }

    stazione x = stazioni->root, y = NULL;

    while(x != NULL){
        y = x;
        if(temp->distanza < x->distanza)
            x = x->left;
        else if(temp->distanza > x->distanza)
            x = x->right;
        else {
            free(temp->veicoli);
            free(temp);
            puts("non aggiunta");
            return ;//stazione già presente
        }

    }
    temp->father = y;
    if(temp->distanza < y->distanza)
        y->left = temp;
    else y->right = temp;
    temp->left = NULL;
    temp->right = NULL;
    temp->colore = 'r';
    insertFixup(temp);
    puts("aggiunta");
    numStazioni++;
}

void demolisciStazione(int dist){
    stazione s = cercaStazione(stazioni->root, dist);

    if(s==NULL) {
        puts("non demolita");
        return;
    }

    stazione y = NULL, x = NULL;
    if(s->left == NULL || s->right == NULL)
        y = s;
    else
        y = trovaSuccessore(s);

    if(y->left != NULL)
        x = y->left;
    else
        x = y->right;
    if(x != NULL)
        x->father = y->father;
    if(y->father == NULL)
        stazioni->root = x;
    else if(y == y->father->left)
        y->father->left = x;
    else
        y->father->right = x;
    if(y != s){
        //sostituzione nodi!: metto in s le cose di y
        s->distanza = y->distanza;
        s->nVeicoli = y->nVeicoli;
        free(s->veicoli);
        s->veicoli = y->veicoli;
    }
    if(y->colore == 'b' && x != NULL)
        deleteFixup(x);
    free(y->veicoli);
    free(y);
    puts("demolita");
    numStazioni--;
}

stazione trovaSuccessore(stazione s){
    if(s->right != NULL)
        return stazioneMinima(s->right);
    stazione y = s->father;
    while(y != NULL && s == y->right){
        s = y;
        y = y->father;
    }
    return y;
}

stazione stazioneMinima(stazione s){
    while(s->left != NULL)
        s = s->left;
    return s;
}

void deleteFixup(stazione s){
    if(s->colore == 'r' || s->father == NULL)
        s->colore = 'b';
    else if(s == s->father->left){
        stazione w = s->father->right;
        if(w->colore == 'r'){
            w->colore = 'b';
            s->father->colore = 'r';
            rotazioneS(s->father);
            w = s->father->right;
        }
        if(w->left->colore == 'b' && w->right->colore == 'b'){
            w->colore = 'r';
            s = s->father;
            deleteFixup(s);
        }
        else{
            if(w->right->colore == 'b'){
                w->left->colore = 'b';
                w->colore = 'r';
                rotazioneD(w);
                w = s->father->right;
            }
            w->colore = s->father->colore;
            s->father->colore = 'b';
            w->right->colore = 'b';
            rotazioneS((s->father));
        }
    }
    else{
        stazione w = s->father->left;
        if(w->colore == 'r'){
            w->colore = 'b';
            s->father->colore = 'r';
            rotazioneD(s->father);
            w = s->father->left;
        }
        if(w->right->colore == 'b' && w->left->colore == 'b')
        {
            w->colore = 'r';
            s = s->father;
            deleteFixup(s);
        }
        else
        {
            if(w->left->colore == 'b')
            {
                w->right->colore = 'b';
                w->colore = 'r';
                rotazioneS(w);
                w = s->father->left;
            }
            w->colore = s->father->colore;
            s->father->colore = 'b';
            w->left->colore = 'b';
            rotazioneD(s->father);
        }
    }
}

short aggiungiAuto(stazione s, int autonomia){
    if(s == NULL)
        return 0;
    s->veicoli[s->nVeicoli] = autonomia;
    s->nVeicoli ++;
    return 1;
}

int cercaAuto(stazione s, int autonomia){
    if(s->nVeicoli == 0)
        return -1;

    int inizio = 0, fine = s->nVeicoli - 1, pos;

    while(inizio<=fine){
        pos = (inizio+fine)/2;
        if(s->veicoli[pos] == autonomia)
            return pos;
        else if(s->veicoli[pos] > autonomia)
            fine = pos - 1;
        else
            inizio = pos + 1;
    }

    return -1;
}

void ordinaAuto(int veicoli[], int p, int r){
    if(p<r){
        int q = (p+r)/2;
        ordinaAuto(veicoli, p, q);
        ordinaAuto(veicoli, q+1, r);
        merge(veicoli, p, q, r);
    }
}
void merge(int veicoli[], int p, int q, int r){
    int n1 = q-p+1;
    int n2 = r-q;
    int L[n1], R[n2];
    int i, j, k;
    for(i=0; i<n1; i++)
        L[i] = veicoli[p+i];
    for(j=0; j<n2; j++)
        R[j] = veicoli[q+j+1];

    i=0;
    j=0;
    k= p;

    while(i<n1 && j<n2){
        if(L[i] <= R[j]){
            veicoli[k] = L[i];
            i++;
        }
        else{
            veicoli[k] = R[j];
            j++;
        }
        k++;
    }
    while(i<n1){
        veicoli[k] = L[i];
        i++;
        k++;
    }
    while(j<n2){
        veicoli[k] = R[j];
        j++;
        k++;
    }

}

void rottamaAuto(stazione s, int autonomia){
    int posVeicolo = cercaAuto(s, autonomia);
    if(posVeicolo == -1) {
        puts("non rottamata");
        return ;
    }

    for(int i=posVeicolo; i<s->nVeicoli-1; i++)
        s->veicoli[i] = s->veicoli[i+1];

    s->nVeicoli --;
    puts("rottamata");
}

stazione cercaStazione(stazione s, int dist){
    if(s == NULL)
        return NULL;
    else if(dist == s->distanza)
        return s;
    else if(dist < s->distanza)
        return cercaStazione(s->left, dist);
    else
        return cercaStazione(s->right, dist);
}

void insertFixup(stazione temp){
    if(temp == stazioni->root)
        stazioni->root->colore = 'b';
    else{
        stazione x = temp->father;
        if (x->colore == 'r') {
            if (x == x->father->left) {
                stazione y = x->father->right;
                if (y != NULL && y->colore == 'r') {
                    x->colore = 'b';
                    y->colore = 'b';
                    x->father->colore = 'r';
                    insertFixup(x->father);
                }
                else{
                    if(temp == x->right){
                        temp = x;
                        rotazioneS(temp);
                        x = temp->father;
                    }
                    x->colore = 'b';
                    x->father->colore = 'r';
                    rotazioneD(x->father);
                }
            }
            else{
                stazione y = x->father->left;
                if(y != NULL && y->colore == 'r'){
                    x->colore = 'b';
                    y->colore = 'b';
                    x->father->colore = 'r';
                    insertFixup(x->father);
                }
                else{
                    if(temp == x->left){
                        temp = x;
                        rotazioneD(temp);
                        x = temp->father;
                    }
                    x->colore = 'b';
                    x->father->colore = 'r';
                    rotazioneS(x->father);
                }
            }
        }
    }
}

void rotazioneS(stazione x){
    stazione y = x->right;
    x->right = y->left;
    if(y->left != NULL)
        y->left->father = x;
    y->father = x->father;
    if(x->father == NULL)
        stazioni->root = y;
    else if(x == x->father->left)
        x->father->left = y;
    else
        x->father->right = y;
    y->left = x;
    x->father = y;
}

void rotazioneD(stazione x){
    stazione y = x->left;
    x->left = y->right;
    if(y->right != NULL)
        y->right->father = x;
    y->father = x->father;
    if(x->father == NULL)
        stazioni -> root = y;
    else if(x == x->father->right)
        x->father->right = y;
    else
        x->father->left = y;
    y->right = x;
    x->father= y;
}
