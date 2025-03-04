#include "processList.h"


void initProcessList(pList *L) {
    L->head = NULL;
    L->numElements = 0;
}

void addProcess(pList *L, pid_t pid, const char *command, ProcessStateType type, int priority) {
    pNode *newNode = (pNode *)malloc(sizeof(pNode));
    if (!newNode) {
        perror("Error al crear el proceso");
        return;
    }
    newNode->pid = pid;
    newNode->launch_time = time(NULL); // Obtener el tiempo actual
    newNode->date = localtime(&newNode->launch_time); // Convertir a hora local
    newNode->type = type;
    strncpy(newNode->command, command, 256);
    newNode->command[255] = '\0'; // Asegurar la terminación de la cadena
    newNode->priority = priority;
    newNode->next = L->head;
    L->head = newNode;
    L->numElements++;

    printf("Proceso %d agregado con comando '%s'.\n", pid, command);
}

void showProcessList(pList L) {
    pNode *current = L.head;

    printf("Procesos en la lista:\n");
    while (current!=NULL) {
        char dateStr[20];
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M:%S", current->date); // Formatear la fecha

        printf("PID: %d\n", current->pid);
        printf("Command: %s\n", current->command);
        printf("State: ");
        switch (current->type) {
            case FINISHED: printf("Finished\n"); break;
            case STOPPED: printf("Stopped\n"); break;
            case SIGNALED: printf("Signaled\n"); break;
            case ACTIVE: printf("Active\n"); break;
        }
        printf("Priority: %d\n", current->priority);
        printf("Launch Time: %s\n", dateStr);
        printf("---------------\n");

        current = current->next;
    }
}

void changeState(pList *L,pid_t pid, ProcessStateType nuevoEstado) {
    pNode *current = L->head;

    while (current!=NULL) {
        if (current->pid==pid) {
            current->type = nuevoEstado;
            printf("Estado del proceso %d actualizado a %d.\n", pid, nuevoEstado);
            return;
        }
        current = current->next;
    }
    printf("Proceso con PID %d no encontrado.\n", pid);
}

void deleteProcessByState(pList *L, ProcessStateType estado) {
    pNode *current = L->head;
    pNode *previous = NULL;

    while (current != NULL) {
        if (current->type == estado) {
            if (previous == NULL) {
                L->head = current->next; // Eliminar nodo cabeza
            } else {
                previous->next = current->next; // Eliminar nodo intermedio
            }
            pNode *temp = current;
            current = current->next;
            free(temp); // Liberar la memoria del nodo
            L->numElements--;
        } else {
            previous = current;
            current = current->next;
        }
    }
}

void clearProcessList(pList *L){
    if(L==NULL){
        return;
    }
    pNode *current=L->head;
    pNode *nextNode;

    while(current!=NULL){
        nextNode=current->next;
        free(current);
        current=nextNode;
    }
    L->head=NULL;
    L->numElements=0;
}