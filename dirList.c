#include "dirList.h"

void initDirList(dList *L){
    L->head=NULL;
    L->numElements=0;
}

void addDir(dList *L, const char *directory){
    dNode *current= L->head;
    while (current!=NULL){
        if(strcmp(current->directory,directory)==0){
            printf("El directorio ya está en la lista: %s\n", directory);
            return;
        }
        current=current->next;
    }
    

    dNode *newNode= (dNode *)malloc(sizeof(dNode));
    if(!newNode){
        perror("Error al asifnar memoria");
        return;
    }
    strncpy(newNode->directory, directory, 256);
    size_t len=strlen(newNode->directory);
    if(len>0 && newNode->directory[len-1]=='\n'){
        newNode->directory[len-1]='\0';
    }
    newNode->next = L->head;
    L->head = newNode;
    L->numElements++;
    printf("Directorio agregado: %s\n", directory);
}

// Elimina un directorio de la lista
bool deleteDir(dList *L, const char *directory) {
    dNode *current = L->head;
    dNode *previous = NULL;

    while (current != NULL) {
        if (strcmp(current->directory, directory) == 0) {
            if (previous == NULL) {
                L->head = current->next; // Eliminar nodo cabeza
            } else {
                previous->next = current->next; // Eliminar nodo intermedio o final
            }
            free(current);
            L->numElements--;
            return true;
        }
        previous = current;
        current = current->next;
    }
    return false; // Directorio no encontrado
}

// Muestra todos los directorios de la lista
void showDirList(dList L) {
    if(L.head==NULL){
        printf("La lista de directorios esta vacia.\n");
        return;
    }

    dNode *current = L.head;
    while (current != NULL) {
        printf("%s\n", current->directory);
        current = current->next;
    }
}

// Limpia toda la lista y libera la memoria
void clearDirList(dList *L) {
    dNode *current = L->head;
    while (current != NULL) {
        dNode *temp = current;
        current = current->next;
        free(temp);
    }
    L->head = NULL;
    L->numElements = 0;
}

// Importa los directorios del `PATH` del sistema
void importPath(dList *L) {
    const char *pathEnv = getenv("PATH");
    if (!pathEnv) {
        perror("Error: no se pudo obtener la variable PATH");
        return;
    }

    char *path = strdup(pathEnv); // Duplicar cadena para manipularla
    if (!path) {
        perror("Error al duplicar PATH");
        return;
    }

    char *token = strtok(path, ":");
    while (token != NULL) {
        addDir(L, token);
        token = strtok(NULL, ":");
    }
    free(path);
}

// Busca el primer directorio de la lista
char *searchListFirst(dList L) {
    if (L.head != NULL) {
        return L.head->directory;
    }
    return NULL; // Lista vacía
}

// Busca el siguiente directorio después del actual
char *searchListNext(dList L, const char *current) {
    dNode *currentNode = L.head;

    while (currentNode != NULL) {
        if (strcmp(currentNode->directory, current) == 0) {
            if (currentNode->next != NULL) {
                return currentNode->next->directory;
            }
            break; // No hay siguiente directorio
        }
        currentNode = currentNode->next;
    }
    return NULL;
}