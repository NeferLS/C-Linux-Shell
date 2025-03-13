#include "HistoList.h"

// creamos una lista vacia
void initHistoricList(hList *L){
    L->head = NULL;
    L->nm_elements = 0;
}

bool isEmptyHistoricList(hList L){
    return L.head == NULL;
}

//inserta un elemento al historial

void insertHistoricItem(hList *L, const char tr[]) {
    hNode *newNode = (hNode *)malloc(sizeof(hNode));
    
    if (newNode == NULL) {
        printf("Error al asignar memoria.\n");
        return;
    }
    
    strncpy(newNode->cmd, tr, MAX_COMM);
    newNode->next = NULL;
    newNode->nm = L->nm_elements + 1;

    if (L->head == NULL) {
        L->head = newNode;
    } else {
        hPosL p = L->head;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = newNode;
    }
    L->nm_elements++;
}

// elimina un elemento del historial
void removeHistoricItem(hList *L){
    
    //si no esta vacia
    hPosL p = L->head;
    L->head = L->head->next;
    free(p);
    L->nm_elements--;;
}

// elimina el historial
void clearHistoricList(hList *L){
    if (L->head ==NULL)
    {
        printf("No hay elementos que eliminar en el historial\n");
        return;
    }

    hPosL p = L->head;
    hPosL temp;
    while (p->next != NULL)
    {
        temp = p;
        p = p->next;
        free(temp);
    }
    L->head = NULL;
    L->nm_elements = 0;
}

// imprime el historial
void printHistoricList(hList *L, int N) {
    if (L->head == NULL) { // si no hay
        printf("No hay elementos que mostrar en la lista\n");
        return;
    }
    hPosL p = L->head;
    if (N == 0) { // si solo ponemos historic
        printf("Imprimiendo todo el historial de comandos\n");
        while (p != NULL) {
            printf("%d. %s\n", p->nm, p->cmd);
            p = p->next;
        }
    } else if (N > 0) { // si ponemos historic N
        if (N > L->nm_elements) { // si ponemos un numero mas grande del numero de elementos
            printf("Error: No hay suficiente historial. Solo hay %d comandos en el historial.\n", L->nm_elements);
            return;
        }
        while (p != NULL) {
            if (p->nm == N) {
                printf("%d. %s\n", p->nm, p->cmd); 
                return;
            }
            p = p->next;
        }
    } else if (N < 0) {
        N = -N; // si ponemos historic -N
        if (N > L->nm_elements) { // si ponemos un numero mas grande del numero de elementos
            printf("Error: Solo hay %d comandos en el historial. No se pueden mostrar los últimos %d.\n", L->nm_elements, N);
            return;
        }
        int first_to_print = L->nm_elements - N + 1; // si no, imprimimos los ultimos N comandos
        printf("Imprimiendo los últimos %d comandos:\n", N);
        while (p != NULL) {
            if (p->nm >= first_to_print) {
                printf("%d. %s\n", p->nm, p->cmd);
            }
            p = p->next;
        }
    }
}

//consigue un item del historial a traves de su numero
char* getHistoricItem(hList *L, int N) {
    hPosL p = L->head;
    int count = 1;
    while (p != NULL) {
        if (count == N) {
            return p->cmd; 
        }
        count++;
        p = p->next;
    }
    return NULL;
}

void initHistoryNav(HistoryNav *nav, hList *L){
    nav->history = L;
    nav->currentPos = -1; //not navegating yet
    nav->lastCommand = L->nm_elements -1;
}

char *navUp(HistoryNav *nav){
    if(nav->currentPos == -1){
        nav->currentPos = nav->lastCommand;
    } 
    if(nav->currentPos > 0){
        nav->currentPos--;
    }
    return getHistoricItem(nav->history, nav->currentPos);
}

char *navDown(HistoryNav *nav){
    if(nav->currentPos == -1 || nav->currentPos == nav->lastCommand){
        nav->currentPos = -1;
        return NULL;
    } 
    else{
       nav->currentPos++;
       return getHistoricItem(nav->history, nav->currentPos); 
    }
}

void resetNav(HistoryNav *nav){
    nav->currentPos = -1;
    nav->lastCommand = nav->history->nm_elements -1;
}