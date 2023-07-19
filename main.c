#include <stdio.h>
#include<stdlib.h>
#include<string.h>

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



int main() {
    char input[100], comando[100];
    stazioni = malloc(sizeof(struct trees));
    stazioni->root = NULL;

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
                    char *autonomie = strtok(input, " "); //do while?
                    while (autonomie != NULL) {
                        autonomie = strtok(NULL, ",");
                        if (autonomie != NULL) {
                            veicoli[i] = atoi(autonomie);
                            if (veicoli[i] < 0) {
                                puts("non aggiunta");
                                break;
                            }
                            i++;
                        }
                    }
                    if(sizeof(veicoli)/sizeof(veicoli[0]) != numV)
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
            //cose matte
        }
    }
    return 0;
}

void aggiungiStazione(int dist, int numV, int veicoli[]){
    stazione temp = malloc(sizeof(struct nodos));
    temp->distanza = dist;
    temp->veicoli = malloc(sizeof(int) * 152);
    temp->nVeicoli = 0;
    if(numV != 0){
        for (int i = 0; i < numV; i++)
            if (aggiungiAuto(temp, veicoli[i]) == 0) {
                free(temp->veicoli);
                free(temp);
                puts("non aggiunta");
                return;
            }
        ordinaAuto(temp->veicoli, 0, temp->nVeicoli - 1);
    }

    if(stazioni->root == NULL){
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
    if(s->nVeicoli == 0){
        s->veicoli[0] = autonomia;
        s->nVeicoli++;
        return 1;
    }

    if(cercaAuto(s, autonomia) != -1) //se è la prima volta non è detto che sia ordinato
        return 0;

    s->veicoli[s->nVeicoli] = autonomia;
    return 1;
}

int cercaAuto(stazione s, int autonomia){
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
                if (y->colore == 'r') {
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
                if(y->colore == 'r'){
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
