#include "HistoList.h"

// functions of creation
void initHistoricList(hList *L){
    L->head = NULL;
    L->nm_elements = 0;
}

bool isEmptyHistoricList(hList L){
    return L.head == NULL;
}

// inserts element
void insertHistoricItem(hList *L, const char tr[]) {
    hNode *newNode = (hNode*)malloc(sizeof(hNode));
    
    if (newNode == NULL) {
        fprintf(stderr, "Error: Couldn't assign command to historic list.\n");
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

// deletes
void removeHistoricItem(hList *L){
    hPosL p = L->head;
    L->head = L->head->next;
    free(p);
    L->nm_elements--;;
}

// clears historic 
void clearHistoricList(hList *L){
    if (L->head ==NULL)
    {
        fprintf(stderr, "No elements to delete on historic list.\n");
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

// prints historic
void printHistoricList(hList *L, int N) {
    if (L->head == NULL) { 
        fprintf(stderr, "No elements to delete on historic list.\n");
        return;
    }
    hPosL p = L->head;
    if (N == 0) { // only historic
        printf("Printing historic list of commands.\n");
        while (p != NULL) {
            printf("%d. %s\n", p->nm, p->cmd);
            p = p->next;
        }
    } else if (N > 0) { // historic + N 
        if (N > L->nm_elements) { 
            fprintf(stderr, "Error: number of elements out of bounce. There are only %d elements at the moment.\n", L->nm_elements);
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
        N = -N; // historic + -N
        if (N > L->nm_elements) { 
            fprintf(stderr, "Error: number of elements out of bounce. There are only %d elements at the moment.\n", L->nm_elements);
            return;
        }
        int first_to_print = L->nm_elements - N + 1; 
        printf("Printing the last %d commands:\n", N);
        while (p != NULL) {
            if (p->nm >= first_to_print) {
                printf("%d. %s\n", p->nm, p->cmd);
            }
            p = p->next;
        }
    }
}

// gets historic item by number
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

// navigation functions
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