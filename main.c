#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct nodov{
    int a;
    struct nodov *next;
};
struct veicoli{
    struct nodov *head;
};
typedef struct veicoli* v;

struct nodos{
    int distanza;
    int g;
    short visited;
    struct nodov *autoMax;
    char colore; //r, b
    struct nodos *left, *right, *father;
    v parco;
};
typedef struct nodos *stazione;

struct trees{
    stazione root;
};

typedef struct trees *alberos;
alberos stazioni;

struct nodoOpen{
    struct nodos *nodo;
    struct nodoOpen *next;
    struct nodoOpen *p;
};
struct listaOpen{
    struct nodoOpen *head;
};

void aggiungiStazione(int, int, int []);
void insertFixup(stazione);
void rotazioneS(stazione);
void rotazioneD(stazione);
stazione cercaStazione(stazione, int);
void demolisciStazione(int);
void deleteFixup(stazione);
stazione trovaSuccessivo(stazione);
stazione stazioneMinima(stazione);
stazione trovaPrecedente(stazione);
stazione stazioneMassima(stazione);

short aggiungiAuto(stazione, int);
void rottamaAuto(stazione, int);
void cercaAutoMax(stazione);
void liberaParco(v);

void liberaLista(struct listaOpen *l);
void stampaPercorsoAsc(struct listaOpen*, int);

void pianificaPercorsoSenzaGrafoAsc(stazione,stazione);
void pianificaPercorsoSenzaGrafoDesc(stazione,stazione);

int main() {
    char input[17000], comando[100];
    stazioni = malloc(sizeof(struct trees));
    stazioni->root = NULL;

    while(fgets(input, 17000, stdin) != NULL ){
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
                    aggiungiStazione(dist, numV, veicoli);
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
                if(s==NULL) {
                    puts("non aggiunta");
                }
                else{
                    if (aggiungiAuto(s, autonomia) == 1) {
                        //ordinaAuto(s->veicoli, 0, s->nVeicoli-1);
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
            else{
                if(inizio == fine)
                    printf("%d\n", inizio);
                else{
                    if (inizio < fine) {
                        pianificaPercorsoSenzaGrafoAsc(cercaStazione(stazioni->root, inizio), cercaStazione(stazioni->root, fine));
                    }
                    else {
                        pianificaPercorsoSenzaGrafoDesc(cercaStazione(stazioni->root, inizio), cercaStazione(stazioni->root, fine));
                    }

                }
            }

        }
    }
    return 0;
}

void pianificaPercorsoSenzaGrafoDesc(stazione inizio, stazione fine){
    if(inizio==NULL || fine==NULL){
        puts("nessun percorso");
        return;
    }

    struct listaOpen *open = malloc(sizeof (struct listaOpen));
    open->head = malloc(sizeof(struct nodoOpen));
    open->head->nodo = inizio;
    open->head->next = NULL;
    open->head->p = NULL;
    open->head->nodo->g = 0;

    struct nodoOpen *fineOp = NULL;

    struct listaOpen *closed = malloc(sizeof(struct listaOpen));
    closed->head = NULL;

    while(1){

        if(open->head==NULL){
            puts("nessun percorso");
            liberaLista(closed);
            free(open);
            return;
        }


        struct nodoOpen *n = open->head;
        if(n->next == NULL)
            fineOp = NULL;
        open->head = open->head->next;
        n->next = closed->head;
        closed->head = n;

        if(closed->head->nodo->distanza == fine->distanza)
            break;

        closed->head->nodo->visited = 1;

        stazione curSt = trovaPrecedente(closed->head->nodo);
        int distMax;
        if(closed->head->nodo->autoMax != NULL)
            distMax = closed->head->nodo->distanza - closed->head->nodo->autoMax->a;
        else
            distMax = closed->head->nodo->distanza;
        int g = closed->head->nodo->g + 1;
        struct nodoOpen *curOp = open->head;

        while(curSt != NULL && curOp != NULL && curSt->distanza > curOp->nodo->distanza){
            if(curSt->visited == 0){
                struct nodoOpen *curUnv = open->head;
                struct nodoOpen *precUnv = NULL;
                while(curUnv != NULL){
                    //controllo se è già in open
                    if(curUnv->nodo->distanza == curSt->distanza){
                        if(g < curUnv->nodo->g){
                            curUnv->nodo->g = g;
                            curUnv->p = closed->head;
                            struct nodoOpen *curUnv2 = open->head;
                            struct nodoOpen *precUnv2 = NULL;
                            while(curUnv2 != curUnv){
                                if(curUnv2->nodo->g > g || (curUnv2->nodo->g == g && curUnv2->nodo->distanza > curUnv->nodo->distanza)){
                                    if(precUnv==NULL)//è possibile?
                                        open->head = open->head->next;
                                    else
                                        precUnv->next = curUnv->next;

                                    if(precUnv2 == NULL){
                                        curUnv->next = open->head->next;
                                        open->head = curUnv;
                                    }
                                    else{
                                        precUnv2->next = curUnv;
                                        curUnv->next = curUnv2;
                                    }
                                    break;
                                }
                                precUnv2 = curUnv2;
                                curUnv2 = curUnv2->next;
                            }
                        }
                        break;
                    }
                    else if(curUnv->nodo->g > g || (curUnv->nodo->g == g && curUnv->nodo->distanza > curSt->distanza)) {
                        struct nodoOpen *temp = malloc(sizeof(struct nodoOpen));
                        temp->nodo = curSt;
                        temp->nodo->g = g;
                        temp->p = closed->head;
                        if(precUnv == NULL){
                            temp->next = open->head;
                            open->head = temp;
                        }
                        else{
                            precUnv->next = temp;
                            temp->next = curUnv;
                        }
                        break;
                    }
                    precUnv = curUnv;
                    curUnv = curUnv->next;
                }
                if(curUnv == NULL){
                    struct nodoOpen *temp = malloc(sizeof(struct nodoOpen));
                    temp->nodo = curSt;
                    temp->nodo->g = g;
                    temp->p = closed->head;
                    fineOp = temp;
                    if(precUnv == NULL){
                        temp->next = NULL;
                        open->head = temp;
                    }
                    else{
                        precUnv->next = temp;
                        temp->next = NULL;
                    }
                }
            }
            curSt = trovaPrecedente(curSt);
        }

        if(fineOp != NULL && fineOp->nodo->distanza >= distMax){
            curSt = trovaPrecedente(fineOp->nodo);
            while(curSt != NULL && curSt->distanza >= distMax){
                if(curSt->visited == 0){
                    struct nodoOpen *curUnv = open->head;
                    struct nodoOpen *precUnv = NULL;
                    while(curUnv != NULL){
                        //controllo se è già in open
                        if(curUnv->nodo->distanza == curSt->distanza){
                            if(g < curUnv->nodo->g){
                                curUnv->nodo->g = g;
                                curUnv->p = closed->head;
                                struct nodoOpen *curUnv2 = open->head;
                                struct nodoOpen *precUnv2 = NULL;
                                while(curUnv2 != curUnv){
                                    if(curUnv2->nodo->g > g || (curUnv2->nodo->g == g && curUnv2->nodo->distanza > curUnv->nodo->distanza)){
                                        if(precUnv==NULL)//è possibile?
                                            open->head = open->head->next;
                                        else
                                            precUnv->next = curUnv->next;

                                        if(precUnv2 == NULL){
                                            curUnv->next = open->head->next;
                                            open->head = curUnv;
                                        }
                                        else{
                                            precUnv2->next = curUnv;
                                            curUnv->next = curUnv2;
                                        }
                                        break;
                                    }
                                    precUnv2 = curUnv2;
                                    curUnv2 = curUnv2->next;
                                }
                            }
                            break;
                        }
                        else if(curUnv->nodo->g > g || (curUnv->nodo->g == g && curUnv->nodo->distanza > curSt->distanza)) {
                            struct nodoOpen *temp = malloc(sizeof(struct nodoOpen));
                            temp->nodo = curSt;
                            temp->nodo->g = g;
                            temp->p = closed->head;
                            fineOp = temp;
                            if(precUnv == NULL){
                                temp->next = open->head;
                                open->head = temp;
                            }
                            else{
                                precUnv->next = temp;
                                temp->next = curUnv;
                            }
                            break;
                        }
                        precUnv = curUnv;
                        curUnv = curUnv->next;
                    }
                    if(curUnv == NULL){
                        struct nodoOpen *temp = malloc(sizeof(struct nodoOpen));
                        temp->nodo = curSt;
                        temp->nodo->g = g;
                        temp->p = closed->head;
                        if(precUnv == NULL){
                            temp->next = NULL;
                            open->head = temp;
                        }
                        else{
                            precUnv->next = temp;
                            temp->next = NULL;
                        }
                    }
                }
                curSt = trovaPrecedente(curSt);
            }
        }

        if(curOp == NULL)
            while(curSt != NULL && curSt->distanza >= distMax){
                if(curSt->visited == 0){
                    struct nodoOpen *curUnv = open->head;
                    struct nodoOpen *precUnv = NULL;
                    while(curUnv != NULL){
                        //controllo se è già in open
                        if(curUnv->nodo->distanza == curSt->distanza){
                            if(g < curUnv->nodo->g){
                                curUnv->nodo->g = g;
                                curUnv->p = closed->head;
                                struct nodoOpen *curUnv2 = open->head;
                                struct nodoOpen *precUnv2 = NULL;
                                while(curUnv2 != curUnv){
                                    //puts("qui");
                                    if(curUnv2->nodo->g > g || (curUnv2->nodo->g == g && curUnv2->nodo->distanza > curUnv->nodo->distanza)){
                                        if(precUnv==NULL)//è possibile?
                                            open->head = open->head->next;
                                        else
                                            precUnv->next = curUnv->next;

                                        if(precUnv2 == NULL){
                                            curUnv->next = open->head->next;
                                            open->head = curUnv;
                                        }
                                        else{
                                            precUnv2->next = curUnv;
                                            curUnv->next = curUnv2;
                                        }
                                        break;
                                    }
                                    precUnv2 = curUnv2;
                                    curUnv2 = curUnv2->next;
                                }
                            }
                            break;
                        }
                        else if(curUnv->nodo->g > g || (curUnv->nodo->g == g && curUnv->nodo->distanza > curSt->distanza)) {
                            struct nodoOpen *temp = malloc(sizeof(struct nodoOpen));
                            temp->nodo = curSt;
                            temp->nodo->g = g;
                            temp->p = closed->head;
                            fineOp = temp;
                            if(precUnv == NULL){
                                temp->next = open->head;
                                open->head = temp;
                            }
                            else{
                                precUnv->next = temp;
                                temp->next = curUnv;
                            }
                            break;
                        }
                        precUnv = curUnv;
                        curUnv = curUnv->next;
                    }
                    if(curUnv == NULL){
                        struct nodoOpen *temp = malloc(sizeof(struct nodoOpen));
                        temp->nodo = curSt;
                        temp->nodo->g = g;
                        temp->p = closed->head;
                        if(precUnv == NULL){
                            temp->next = NULL;
                            open->head = temp;
                        }
                        else{
                            precUnv->next = temp;
                            temp->next = NULL;
                        }
                    }
                }
                curSt = trovaPrecedente(curSt);
            }
    }

    stampaPercorsoAsc(closed, fine->distanza);
    liberaLista(open);
    //setVisited(stazioni->root);
    //setVisited2(fine, inizio);
}
void pianificaPercorsoSenzaGrafoAsc(stazione inizio, stazione fine){
    if(inizio==NULL || fine==NULL){
        puts("nessun percorso");
        return;
    }

    struct listaOpen *open = malloc(sizeof (struct listaOpen));
    open->head = malloc(sizeof(struct nodoOpen));
    open->head->nodo = inizio;
    open->head->next = NULL;
    open->head->p = NULL;
    open->head->nodo->g = 0;

    struct nodoOpen *fineOp = NULL;

    struct listaOpen *closed = malloc(sizeof(struct listaOpen));
    closed->head = NULL;

    while(1){

        if(open->head==NULL){
            puts("nessun percorso");
            liberaLista(closed);
            free(open);
            return;
        }


        struct nodoOpen *n = open->head;
        if(n->next == NULL)
            fineOp = NULL;
        open->head = open->head->next;
        n->next = closed->head;
        closed->head = n;

        if(closed->head->nodo->distanza == fine->distanza)
            break;

        closed->head->nodo->visited = 1;
        stazione curSt = trovaSuccessivo(closed->head->nodo);
        int distMax = closed->head->nodo->distanza + closed->head->nodo->autoMax->a;
        struct nodoOpen *curOp = open->head;
        int g = closed->head->nodo->g + 1;
        while(curSt != NULL && curOp != NULL && curSt->distanza < curOp->nodo->distanza){
            if(curSt->visited == 0){
                struct nodoOpen *curUnv = open->head;
                struct nodoOpen *precUnv = NULL;
                while(curUnv != NULL){
                    //controllo se è già in open
                    if(curUnv->nodo->distanza == curSt->distanza){
                        if(g < curUnv->nodo->g){
                            curUnv->nodo->g = g;
                            curUnv->p = closed->head;
                            struct nodoOpen *curUnv2 = open->head;
                            struct nodoOpen *precUnv2 = NULL;
                            while(curUnv2 != curUnv){
                                if(curUnv2->nodo->g > g || (curUnv2->nodo->g == g && curUnv2->nodo->distanza > curUnv->nodo->distanza)){
                                    if(precUnv==NULL)//è possibile?
                                        open->head = open->head->next;
                                    else
                                        precUnv->next = curUnv->next;

                                    if(precUnv2 == NULL){
                                        curUnv->next = open->head->next;
                                        open->head = curUnv;
                                    }
                                    else{
                                        precUnv2->next = curUnv;
                                        curUnv->next = curUnv2;
                                    }
                                    break;
                                }
                                precUnv2 = curUnv2;
                                curUnv2 = curUnv2->next;
                            }
                        }
                        break;
                    }
                    else if(curUnv->nodo->g > g || (curUnv->nodo->g == g && curUnv->nodo->distanza > curSt->distanza)) {
                        struct nodoOpen *temp = malloc(sizeof(struct nodoOpen));
                        temp->nodo = curSt;
                        temp->nodo->g = g;
                        temp->p = closed->head;
                        if(precUnv == NULL){
                            temp->next = open->head;
                            open->head = temp;
                        }
                        else{
                            precUnv->next = temp;
                            temp->next = curUnv;
                        }
                        break;
                    }
                    precUnv = curUnv;
                    curUnv = curUnv->next;
                }
                if(curUnv == NULL){
                    struct nodoOpen *temp = malloc(sizeof(struct nodoOpen));
                    temp->nodo = curSt;
                    temp->nodo->g = g;
                    temp->p = closed->head;
                    fineOp = temp;
                    if(precUnv == NULL){
                        temp->next = NULL;
                        open->head = temp;
                    }
                    else{
                        precUnv->next = temp;
                        temp->next = NULL;
                    }
                }
            }
            curSt = trovaSuccessivo(curSt);
        }

        if(fineOp != NULL && fineOp->nodo->distanza <= distMax){
            curSt = trovaSuccessivo(fineOp->nodo);
            while(curSt != NULL && curSt->distanza <= distMax){
                if(curSt->visited == 0){
                    struct nodoOpen *curUnv = open->head;
                    struct nodoOpen *precUnv = NULL;
                    while(curUnv != NULL){
                        //controllo se è già in open
                        if(curUnv->nodo->distanza == curSt->distanza){
                            if(g < curUnv->nodo->g){
                                curUnv->nodo->g = g;
                                curUnv->p = closed->head;
                                struct nodoOpen *curUnv2 = open->head;
                                struct nodoOpen *precUnv2 = NULL;
                                while(curUnv2 != curUnv){
                                    if(curUnv2->nodo->g > g || (curUnv2->nodo->g == g && curUnv2->nodo->distanza > curUnv->nodo->distanza)){
                                        if(precUnv==NULL)//è possibile?
                                            open->head = open->head->next;
                                        else
                                            precUnv->next = curUnv->next;

                                        if(precUnv2 == NULL){
                                            curUnv->next = open->head->next;
                                            open->head = curUnv;
                                        }
                                        else{
                                            precUnv2->next = curUnv;
                                            curUnv->next = curUnv2;
                                        }
                                        break;
                                    }
                                    precUnv2 = curUnv2;
                                    curUnv2 = curUnv2->next;
                                }
                            }
                            break;
                        }
                        else if(curUnv->nodo->g > g || (curUnv->nodo->g == g && curUnv->nodo->distanza > curSt->distanza)) {
                            struct nodoOpen *temp = malloc(sizeof(struct nodoOpen));
                            temp->nodo = curSt;
                            temp->nodo->g = g;
                            temp->p = closed->head;
                            if(precUnv == NULL){
                                temp->next = open->head;
                                open->head = temp;
                            }
                            else{
                                precUnv->next = temp;
                                temp->next = curUnv;
                            }
                            break;
                        }
                        precUnv = curUnv;
                        curUnv = curUnv->next;
                    }
                    if(curUnv == NULL){
                        struct nodoOpen *temp = malloc(sizeof(struct nodoOpen));
                        temp->nodo = curSt;
                        temp->nodo->g = g;
                        temp->p = closed->head;
                        fineOp = temp;
                        if(precUnv == NULL){
                            temp->next = NULL;
                            open->head = temp;
                        }
                        else{
                            precUnv->next = temp;
                            temp->next = NULL;
                        }
                    }
                }
                curSt = trovaSuccessivo(curSt);
            }
        }

        if(curOp == NULL){
            while(curSt != NULL && curSt->distanza <= distMax){
                if(curSt->visited == 0){
                    struct nodoOpen *curUnv = open->head;
                    struct nodoOpen *precUnv = NULL;
                    while(curUnv != NULL){
                        //controllo se è già in open
                        if(curUnv->nodo->distanza == curSt->distanza){
                            if(g < curUnv->nodo->g){
                                curUnv->nodo->g = g;
                                curUnv->p = closed->head;
                                struct nodoOpen *curUnv2 = open->head;
                                struct nodoOpen *precUnv2 = NULL;
                                while(curUnv2 != curUnv){
                                    if(curUnv2->nodo->g > g || (curUnv2->nodo->g == g && curUnv2->nodo->distanza > curUnv->nodo->distanza)){
                                        if(precUnv==NULL)//è possibile?
                                            open->head = open->head->next;
                                        else
                                            precUnv->next = curUnv->next;

                                        if(precUnv2 == NULL){
                                            curUnv->next = open->head->next;
                                            open->head = curUnv;
                                        }
                                        else{
                                            precUnv2->next = curUnv;
                                            curUnv->next = curUnv2;
                                        }
                                        break;
                                    }
                                    precUnv2 = curUnv2;
                                    curUnv2 = curUnv2->next;
                                }
                            }
                            break;
                        }
                        else if(curUnv->nodo->g > g || (curUnv->nodo->g == g && curUnv->nodo->distanza > curSt->distanza)) {
                            struct nodoOpen *temp = malloc(sizeof(struct nodoOpen));
                            temp->nodo = curSt;
                            temp->nodo->g = g;
                            temp->p = closed->head;
                            if(precUnv == NULL){
                                temp->next = open->head;
                                open->head = temp;
                            }
                            else{
                                precUnv->next = temp;
                                temp->next = curUnv;
                            }
                            break;
                        }
                        precUnv = curUnv;
                        curUnv = curUnv->next;
                    }
                    if(curUnv == NULL){
                        struct nodoOpen *temp = malloc(sizeof(struct nodoOpen));
                        temp->nodo = curSt;
                        temp->nodo->g = g;
                        temp->p = closed->head;
                        fineOp = temp;
                        if(precUnv == NULL){
                            temp->next = NULL;
                            open->head = temp;
                        }
                        else{
                            precUnv->next = temp;
                            temp->next = NULL;
                        }
                    }
                }
                curSt = trovaSuccessivo(curSt);
            }
        }
    }

    stampaPercorsoAsc(closed, fine->distanza);
    liberaLista(open);
}



void stampaPercorsoAsc(struct listaOpen *closed, int fine){
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
            succ->nodo->visited = 0;
            free(succ);
            succ = curcl->next;
        }

        curcl->next = percorso->head;
        percorso->head = curcl;
        curcl = curcl->p;
    }


    curcl = percorso->head;
    while(curcl != NULL){
        printf("%d", curcl->nodo->distanza);
        if(curcl->nodo->distanza != fine)
            printf(" ");
        curcl = curcl->next;
    }
    printf("\n");
    liberaLista(percorso);
    free(closed);
}

void liberaLista(struct listaOpen *l){
    struct nodoOpen *cur = l->head;
    struct nodoOpen *prec = NULL;
    while(cur != NULL){
        cur->nodo->visited = 0;
        prec = cur;
        cur = cur->next;
        free(prec);
    }
    free(l);
}

void aggiungiStazione(int dist, int numV, int v[]){
    stazione temp = malloc(sizeof(struct nodos));
    temp->distanza = dist;
    temp->autoMax = NULL;
    temp->visited = 0;
    //temp->g = 2147483647;


    if(stazioni->root == NULL){
        temp->parco = malloc(sizeof(struct veicoli));
        temp->parco->head = NULL;
        if(numV != 0){
            for (int i = 0; i < numV; i++) {
                aggiungiAuto(temp, v[i]);
            }
        }

        temp->left = NULL;
        temp->right = NULL;
        temp->father = NULL;
        stazioni->root = temp;
        insertFixup(temp);
        puts("aggiunta");
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
            //liberaParco(temp->parco);
            free(temp);
            puts("non aggiunta");
            return ;//stazione già presente
        }

    }

    temp->parco = malloc(sizeof(struct veicoli));
    temp->parco->head = NULL;
    if(numV != 0){
        for (int i = 0; i < numV; i++) {
            aggiungiAuto(temp, v[i]);
        }
    }

    temp->father = y;
    if(y==NULL)
        stazioni->root = temp;
    else if(temp->distanza < y->distanza)
        y->left = temp;
    else y->right = temp;
    temp->left = NULL;
    temp->right = NULL;
    temp->colore = 'r';
    insertFixup(temp);
    puts("aggiunta");
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
        y = trovaSuccessivo(s);

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
        liberaParco(s->parco);
        s->parco = y->parco;
        s->g = y->g;
        s->visited = y->visited;
        s->autoMax = y->autoMax;
        //free(s->veicoli);

    }
    else {
        //free(y->autoMax);
        liberaParco(y->parco);
        //free(y->veicoli);
    }
    if(y->colore == 'b' && x != NULL)
        deleteFixup(x);
    free(y);
    puts("demolita");
}

void liberaParco(v parco){
    struct nodov *cur = parco->head;
    struct nodov *prec = NULL;

    while(cur != NULL){
        prec = cur;
        cur = cur->next;
        free(prec);
    }
    free(parco);
}

stazione trovaSuccessivo(stazione s){
    if(s->right != NULL)
        return stazioneMinima(s->right);
    stazione y = s->father;
    while(y != NULL && s == y->right){
        s = y;
        y = y->father;
    }
    return y;
}

stazione trovaPrecedente(stazione s){
    if(s->left != NULL)
        return stazioneMassima(s->left);
    stazione y = s->father;
    while(y != NULL && s == y->left){
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

stazione stazioneMassima(stazione s){
    while(s->right != NULL)
        s = s->right;
    return s;
}

void deleteFixup(stazione s){
    if(s->colore == 'r' || s->father == NULL)
        s->colore = 'b';
    else if(s == s->father->left){
        stazione w = s->father->right;
        if(w!=NULL && w->colore == 'r'){
            w->colore = 'b';
            s->father->colore = 'r';
            rotazioneS(s->father);
            w = s->father->right;
        }
        if(w!=NULL && w->left !=NULL && w->left->colore == 'b' && w->right != NULL && w->right->colore == 'b'){
            w->colore = 'r';
            s = s->father;
            deleteFixup(s);
        }
        else{
            if(w!=NULL && w->right !=NULL &&w->right->colore == 'b'){
                if(w->left !=NULL)
                    w->left->colore = 'b';
                w->colore = 'r';
                rotazioneD(w);
                w = s->father->right;
            }
            if(w!=NULL)
                w->colore = s->father->colore;
            s->father->colore = 'b';
            if(w!=NULL && w->right != NULL )
                w->right->colore = 'b';
            rotazioneS((s->father));
        }
    }
    else{
        stazione w = s->father->left;
        if(w!=NULL && w->colore == 'r'){
            w->colore = 'b';
            s->father->colore = 'r';
            rotazioneD(s->father);
            w = s->father->left;
        }
        if(w!=NULL && w->right != NULL && w->right->colore == 'b' && w->left != NULL && w->left->colore == 'b')
        {
            w->colore = 'r';
            s = s->father;
            deleteFixup(s);
        }
        else
        {
            if(w!=NULL && w->left !=NULL && w->left->colore == 'b')
            {
                if(w->right !=NULL )
                    w->right->colore = 'b';
                w->colore = 'r';
                rotazioneS(w);
                w = s->father->left;
            }
            if(w!=NULL)
                w->colore = s->father->colore;
            s->father->colore = 'b';
            if(w!=NULL && w->left != NULL)
                w->left->colore = 'b';
            rotazioneD(s->father);
        }
    }
}

short aggiungiAuto(stazione s, int autonomia){
    if(s == NULL)
        return 0;
    struct nodov *temp = malloc(sizeof(struct nodov));
    temp->a = autonomia;

    if(s->parco->head == NULL)
        temp->next = NULL;
    else
        temp->next = s->parco->head;

    s->parco->head = temp;

    if(s->autoMax == NULL || s->parco->head->a > s->autoMax->a)
        s->autoMax = s->parco->head;

    return 1;
}

void rottamaAuto(stazione s, int autonomia){
    if(s->parco == NULL){
        puts("non rottamata");
        return;
    }
    struct nodov *cur = s->parco->head;
    struct nodov *prec = NULL;
    if(cur == NULL){
        puts("non rottamata");
        return;
    }
    while(cur != NULL){
        if(cur->a == autonomia){
            if(prec==NULL)
                s->parco->head = s->parco->head->next;
            else
                prec->next = cur->next;
            if(cur->a == s->autoMax->a)
                cercaAutoMax(s);
            free(cur);
            puts("rottamata");
            return;
        }
        prec = cur;
        cur = cur->next;
    }
    puts("non rottamata");
}

void cercaAutoMax(stazione s){
    struct nodov *cur = s->parco->head;

    if(cur == NULL){
        s->autoMax = NULL;
        return;
    }

    s->autoMax = cur;
    cur = cur->next;
    while(cur != NULL){
        if(cur->a >= s->autoMax->a)
            s->autoMax = cur;
        cur = cur->next;
    }
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

void insertFixup(stazione z){
    while(z!=NULL && z->father != NULL && z->father->colore == 'r'){
        if(z->father->father != NULL && z->father == z->father->father->left){
            stazione y = z->father->father->right;
            if(y!=NULL && y->colore == 'r'){
                z->father->colore = 'b';
                y->colore = 'b';
                z->father->father->colore = 'r';
                z = z->father->father;
            }
            else{
                if(z == z->father->right){
                    z = z->father;
                    rotazioneS(z);
                }
                z->father->colore = 'b';
                z->father->father->colore = 'r';
                rotazioneD(z->father->father);
            }
        }
        else if(z->father->father != NULL){
            stazione y = z->father->father->left;
            if(y!=NULL && y->colore == 'r'){
                z->father->colore = 'b';
                y->colore = 'b';
                z->father->father->colore = 'r';
                z = z->father->father;
            }
            else{
                if(z == z->father->left){
                    z = z->father;
                    rotazioneD(z);
                }
                z->father->colore = 'b';
                z->father->father->colore = 'r';
                rotazioneS(z->father->father);
            }
        }
    }
    stazioni->root->colore = 'b';
}

void rotazioneS(stazione x){
    stazione y = x->right;
    if(y==NULL)
        return;
    x->right = y->left;
    if(y->left != NULL)
        y->left->father = x;
    if(y!=NULL)
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
    if(y==NULL)
        return;
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