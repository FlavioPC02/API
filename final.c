#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_CARS 512
#define INPUT "../archivio_test_aperti/open_7.txt"
#define MAX_FUN 5500
#define MAX_NODE 30000

FILE *fd2;
char *useless;

//Nodo del bst
typedef struct stations {

    int distance; //distanza della stazione
    int cars[MAX_CARS]; //array che contiene le autonomie delle macchine
    int max_autonomy; //Auto con autonomia maggiore
    int n_cars; //Numero di macchine contenute nella stazione
    struct stations *parent;
    struct stations *left;
    struct stations *right;

} stations;

//Radice del bst
stations *root = NULL;
//stations *st;

//--GESTIONE BST---

//Cerca il nodo che contiene la distanza k
stations *tree_search(stations *x, int k){

    //Se ritorna NULL allora non esiste una stazione distante k
    if (x == NULL || k == x->distance)
        return x;

    if (k < x->distance)
        return tree_search(x->left, k);
    else
        return tree_search(x->right, k);

}

//Restituisce l'elemento minimo del bst
stations *tree_minimum(stations *x){

    //Finché il figlio sinistro non è una foglia
    while (x->left != NULL)
    {
        x = x->left;
    }

    return x;
    
}

//Restituisce y, l'elemento successore di x
stations *tree_successor(stations *x){

    stations *y;

    if (x->right != NULL)
    {
        return tree_minimum(x->right);
    }
    y = x->parent;
    while (y != NULL && x == y->right)
    {
        x = y;
        y = y->parent;
    }
    return y;

}

//Inserisce la stazione a distanza z nell'albero
stations *tree_insert (int z){

    stations *new_station;
    new_station = malloc(sizeof(stations));
    new_station->distance = z;
    new_station->left = NULL;
    new_station->right = NULL;
    new_station->parent = NULL;
    new_station->n_cars = 0;
    new_station->max_autonomy = 0;

    stations *y = NULL;
    stations *x = root;

    while (x != NULL)
    {
        y = x;
        //EDGE CASE: La stazione a distanza z già esiste
        if(new_station->distance == x->distance){
            return NULL;
        }

        if (new_station->distance < x->distance)
            x = x->left;
        else
            x = x->right;

    }

    new_station->parent = y;
    
    if (y == NULL)
    {
        root = new_station;
    } else if (new_station->distance < y->distance)
    {
        y->left = new_station;
    } else
    {
        y->right = new_station;
    }
        
    return new_station;

}

bool tree_delete(int k){
    
    stations *z = tree_search(root, k); //Nodo da calcellare
    stations *y; //Verrà aggiornato col nodo da cancellare
    stations *x;

    if(z == NULL)
        return false;

    if (z->left == NULL || z->right == NULL)
        y = z;
    else 
        y = tree_successor(z);

    if (y->left != NULL) 
        x = y->left;
    else
        x = y->right;

    if (x != NULL)
        x->parent = y->parent;

    if (y->parent == NULL)
        root = x;
    else if (y == y->parent->left)
    
        y->parent->left = x;
    else
        y->parent->right = x;

    if (y != z){

        z->distance = y->distance;
        z->max_autonomy = y->max_autonomy;
        z->n_cars = y->n_cars;

        for (int i = 0; i < y->n_cars; i++)
        {
            z->cars[i] = y->cars[i];
        }
        
    }

    free(y);

    return true;

}

//---FUNZIONI---

bool aggiungi_stazione(FILE* fd){

    char in[MAX_FUN]; //Buffer fgets
    int i = 0;
    int distance = 0;
    int autonomy = 0;

    useless = fgets(in, MAX_FUN, fd);
    //printf("%s", in);
    
    while(in[i] != ' ' && in[i] != '\n'){

        distance *= 10;
        distance += in[i] - 48;

        i++;
    }
    i++;
    //printf("La distanza è: %d\n", distance);

    stations *stazione = tree_insert(distance);

    if(stazione != NULL)
    {
        
        while(in[i] != ' ' && in[i] != '\n'){

            stazione->n_cars *= 10;
            stazione->n_cars += in[i] - 48;

            i++;
        }
        i++;

        //printf("Il numero di auto è: %d\n", stazione->n_cars);

        //if(stazione->n_cars > 512){
        //    printf("Errore: il numero di auto è maggiore di quello consentito");
        //    return false;
        //}

        for(int j = 0; j < stazione->n_cars; j++){
            autonomy = 0;
            while (in[i] != ' ' && in[i] != '\n'){

                autonomy *= 10;
                autonomy += in[i] - 48;

                i++;
            }
            i++;
            //printf("Autonomia: %d\n", autonomy);
            if(stazione->max_autonomy < autonomy)
                stazione->max_autonomy = autonomy;

            stazione->cars[j] = autonomy;
        }


        return true;
    } else{
        return false;
    }
}

bool demolisci_stazione(FILE *fd){

    char in[MAX_FUN];
    int i = 0;
    int distance = 0;

    useless = fgets(in, MAX_FUN, fd);

    while (in[i] != ' ' && in[i] != '\n')
    {
        distance *= 10;
        distance += in[i] - 48;
        i++;
    }
    
    //printf("%d\n", distance);

    if(tree_delete(distance))
        return true;
    else
        return false;
    

}

bool aggiungi_auto(FILE *fd){

    char in[MAX_FUN];
    int distance = 0;
    int autonomy = 0;
    int i = 0;

    useless = fgets(in, MAX_FUN, fd);

    while (in[i] != ' ' && in[i] != '\n')
    {
        distance *= 10;
        distance += in[i] - 48;
        i++;
    }
    i++;

    stations *station = tree_search(root, distance);

    if(station == NULL)
        return false;

    //fprintf(fd2, "%d ", station->n_cars);

    while(in[i] != ' ' && in[i] != '\n')
    {
        autonomy *= 10;
        autonomy += in[i] - 48;
        i++;
    }
    i++;

    if (autonomy > station->max_autonomy)
        station->max_autonomy = autonomy;

    station->cars[station->n_cars] = autonomy;
    station->n_cars++;

    return true;

}

bool rottama_auto(FILE *fd){

    char in[MAX_FUN];
    int distance = 0;
    int autonomy = 0;
    int i = 0;
    bool flag = false; //True = l'auto esiste ed è stata demolita, false = l'auto non esiste
    int new_max = 0;

    useless = fgets(in, MAX_FUN, fd);

    while (in[i] != ' ' && in[i] != '\n')
    {
        distance *= 10;
        distance += in[i] - 48;
        i++;
    }
    i++;

    stations *station = tree_search(root, distance);

    if(station == NULL)
        return false;

    while(in[i] != ' ' && in[i] != '\n')
    {
        autonomy *= 10;
        autonomy += in[i] - 48;
        i++;
    }
    i++;

    //Scansiono tutto l'array per trovare l'elemento da cancellare
    for (int i = 0; i < station->n_cars; i++)
    {
        //se dovessi trovare l'elemento da cancellare
        if(station->cars[i] == autonomy){

            //Scambio l'elemento da cancellare con l'ultimo elemento
            station->cars[i] = station->cars[station->n_cars-1];
            //"Pulisco" l'ultimo elemento, che ora è quello da cancellare, impostandolo a 0
            station->cars[station->n_cars-1] = 0;
            //Posso decrementare il numero di auto
            station->n_cars--; //In questo modo viene aggiornata anche la condizione del ciclo for, quindi non scansiono l'ultimo elemento che ormai è 0
            //Imposta la flag come true per indicare che ho trovato l'auto
            flag = true;
        }

        if (new_max < station->cars[i])
        {
            new_max = station->cars[i];
        }
        
    }
    
    station->max_autonomy = new_max;

    //printf("%d %d", distance, autonomy);
    
    if (flag)
        return true;
    else
        return false;
    
}

bool trova_percorso(stations *partenza, stations *arrivo){

    int adj[MAX_NODE][3] = {0};
    int c = 0;
    int percorso[MAX_NODE] = {0};
    int b = 0;
    int n_node[MAX_NODE] = {0};
    stations *stazione_corrente = NULL;

    //Se la stazione di partenza è minore di quella di arrivo
    if(partenza->distance < arrivo->distance){

        stazione_corrente = partenza;

        adj[c][0] = stazione_corrente->distance;
        adj[c][1] = stazione_corrente->distance + stazione_corrente->max_autonomy;
        //adj[c][2] = 0;    

        while (stazione_corrente->distance != arrivo->distance)
        {
            c++;
            stazione_corrente = tree_successor(stazione_corrente);
            adj[c][0] = stazione_corrente->distance;
            adj[c][1] = stazione_corrente->distance + stazione_corrente->max_autonomy;
            //adj[c][2] = 0;

        }

        for (int i = 0; i < c; i++)
        {
            int a = i+1;
            while (adj[a][0] <= adj[i][1] && a <= c)
            {
                if(adj[a][2] == 0)
                    adj[a][2] = adj[i][0];
                a++;
            }
        }

        b = c;
        int y = 0;
        //Ripercorro al contrario lungo il percorso più veloce
        //Finchè la stazione non è collegata a qualcosa o arriva alla partenza
        while (adj[b][2] != 0)
        {
            percorso[y] = adj[b][0];
            y++;
            int l=0, r=b, m=l+(r-l)/2;

            while (adj[m][0] != adj[b][2])
            {
                if (adj[b][2] > adj[m][0]){
                    l = m;
                    m = l + (r-l)/2;
                } else {
                    r = m;
                    m = l + (r-l)/2;
                }
            }
            b = m;        
        }

        //for (int i = 0; i <= c; i++)
        //{
        //    for (int j = 0; j < 3; j++)
        //    {
        //        printf("%d ", adj[i][j]);
        //    }
        //    printf("\n");
        //}     
        //printf("\n");

        if(adj[b][0] == partenza->distance)
            percorso[y] = adj[b][0];
        else
            return false;
    
        for (int i = y; i > 0; i--)
        {
            fprintf(fd2, "%d", percorso[i]);
            fprintf(fd2, " ");
        }
        fprintf(fd2, "%d", percorso[0]);

    }
    else{ //Se la stazione di partenza è maggiore di quella di arrivo

        int z = 0;
        stazione_corrente = arrivo;

        adj[c][0] = stazione_corrente->distance;
        adj[c][1] = stazione_corrente->distance - stazione_corrente->max_autonomy;
        if (adj[c][1]<0)
            adj[c][1] = 0;
        //adj[c][2] = 0;

        while (stazione_corrente->distance != partenza->distance)
        {
            c++;
            stazione_corrente = tree_successor(stazione_corrente);
            adj[c][0] = stazione_corrente->distance;
            adj[c][1] = stazione_corrente->distance - stazione_corrente->max_autonomy;
            if (adj[c][1]<0)
                adj[c][1] = 0;
            //adj[c][2] = 0;
        }

        for (int i = c; i >= 0; i--)
        {
            int a = i-1;
            z = n_node[i];
            while (adj[a][0] >= adj[i][1] && a >= 0)
            {
                if (n_node[a] == 0)
                {
                    adj[a][2] = adj[i][0];
                    n_node[a] = z+1;
                } else if(n_node[a] == z+1){
                    adj[a][2] = adj[i][0];
                }
                a--;
            }
            
        }

        //for (int i = 0; i <= c; i++)
        //{
        //    for (int j = 0; j < 3; j++)
        //    {
        //        printf("%d ", adj[i][j]);
        //    }
        //    printf("\n");
        //}     
        //printf("\n");

        //return false;

        b = 0;
        int y = 0;
        int i = 0;
        //Ripercorro al contrario lungo il percorso più veloce
        //Finchè la stazione non è collegata a qualcosa o arriva alla partenza

        while (adj[b][2] != 0)
        {
            percorso[y] = adj[b][0];
            y++;
            //int l=b, r=c, m=l+(r-l)/2;
            for (i = b; i <= c; i++)
            {
                if (adj[i][0] == adj[b][2])
                    break;
            }
            b = i;
            
            //while (adj[m][0] != adj[b][2])
            //{
            //    if (adj[b][2] > adj[m][0]){
            //        l = m;
            //        m = l + (r-l)/2;
            //    } else {
            //        r = m;
            //        m = l + (r-l)/2;
            //    }
            //}
            //b = m;
        }
        if(adj[b][0] == partenza->distance)
            percorso[y++] = adj[b][0];
        else
            return false;


        //for (int j = 0; j < y; j++)
        //{
        //    printf("%d ", percorso[j]);
        //}
        
        for (int i = y-1; i > 0; i--)
        {
            fprintf(fd2, "%d", percorso[i]);
            fprintf(fd2, " ");
        }
        fprintf(fd2, "%d", percorso[0]);

    }

    return true;
}

bool pianifica_percorso(FILE *fd){

    char in[MAX_FUN];
    int partenza = 0;
    int arrivo = 0;
    int i = 0;

    useless = fgets(in, MAX_FUN, fd);

    while (in[i] != ' ' && in[i] != '\n')
    {
        partenza *= 10;
        partenza += in[i] - 48;
        i++;
    }
    i++;

    while(in[i] != ' ' && in[i] != '\n')
    {
        arrivo *= 10;
        arrivo += in[i] - 48;
        i++;
    }
    i++;

    stations *stazione_partenza = tree_search(root, partenza);
    stations *stazione_arrivo = tree_search(root, arrivo);

    if (stazione_partenza == NULL || stazione_arrivo == NULL)
        return false;
    
    if (trova_percorso(stazione_partenza, stazione_arrivo))
    {
        return true;
    } else{
        return false;
    }
    

    //fprintf(stdout, "%d %d\n", partenza, arrivo);
}

//void inorderTraversal(stations* root) {
//    if (root != NULL) {
//        inorderTraversal(root->left);
//        fprintf(stdout, "%d ", root->distance);
//        inorderTraversal(root->right);
//}
//    }

int main()
{

    //clock_t start = clock();

    char n[20];
    //FILE* fd = fopen(INPUT, "r");
    FILE *fd = stdin;
    int counter = 1;
    //fd2 = fopen("o.txt", "w");
    fd2 = stdout;

    while ((fscanf(fd, "%s ", &n[0])) != -1) {

        //if(counter == 180){
        //
        //    st = tree_search(root, 447);
        //    for (int i = 0; i < st->n_cars; i++)
        //    {
        //        printf("%d ", st->cars[i]);
        //    }
        //    printf("%d", st->max_autonomy);
        //    printf("\n");
        //}

        //fprintf(fd2, "%d) ", counter);
        if (!(strcmp(n, "aggiungi-stazione")))
        {
            if(aggiungi_stazione(fd))
                fprintf(fd2, "aggiunta\n");
            else
                fprintf(fd2, "non aggiunta\n");
        }

        if (!(strcmp(n, "demolisci-stazione")))
        {
            if(demolisci_stazione(fd))
                fprintf(fd2, "demolita\n");
            else
                fprintf(fd2, "non demolita\n");
        }

        if (!(strcmp(n, "aggiungi-auto")))
        {
            if(aggiungi_auto(fd))
                fprintf(fd2, "aggiunta\n");
            else
                fprintf(fd2, "non aggiunta\n");
        }

        if (!(strcmp(n, "rottama-auto")))
        {
            if(rottama_auto(fd))
                fprintf(fd2, "rottamata\n");
            else
                fprintf(fd2, "non rottamata\n");
        }

        if (!(strcmp(n, "pianifica-percorso")))
        {
            if(pianifica_percorso(fd))
                fprintf(fd2, "\n");
            else
                fprintf(fd2, "nessun percorso\n");
        }

        counter++;

    }

    //clock_t end = clock();
    //long double seconds = (float)(end-start)/CLOCKS_PER_SEC;
    //printf("%Lf", seconds);

    return 0;
}