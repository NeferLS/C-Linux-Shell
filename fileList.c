#include "fileList.h"

void *memFile = NULL;
FileInfo OPEN_FILES[MAX_OPEN_FILES];

void initializeOpenFiles() {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        OPEN_FILES[i].descriptor = i;
        strcpy(OPEN_FILES[i].file_name, "");
        strcpy(OPEN_FILES[i].mode, "");
        OPEN_FILES[i].isUsed = 0;
    }
    
    OPEN_FILES[0].isUsed = 1;
    strcpy(OPEN_FILES[0].file_name, "entrada estandar");
    strcpy(OPEN_FILES[0].mode, "O_RDWR");
    
    OPEN_FILES[1].isUsed = 1;
    strcpy(OPEN_FILES[1].file_name, "salida estandar");
    strcpy(OPEN_FILES[1].mode, "O_RDWR");
    
    OPEN_FILES[2].isUsed = 1;
    strcpy(OPEN_FILES[2].file_name, "error estandar");
    strcpy(OPEN_FILES[2].mode, "O_RDWR");
}

bool isEmptyFileList(fList L){
    return L.head == NULL;
}

void initFileList(fList *L){
    L->head = NULL;
    L->nm_elements = 0;
    initializeOpenFiles();
}

void insertFile(fList *L, int df, const char file_name[], const char md[]){
    fNode *newFile = (fNode*)malloc(sizeof(fNode));

    if (newFile == NULL){
        perror("Error al asignar memoria para el archivo");
        return;
    }

    newFile->df = df;
    strncpy(newFile->file_name, file_name, sizeof(newFile->file_name) - 1);
    newFile->file_name[sizeof(newFile->file_name) - 1] = '\0'; 

    strncpy(newFile->md, md, sizeof(newFile->md) - 1);
    newFile->md[sizeof(newFile->md) - 1] = '\0';
    newFile->next = NULL;
    L->nm_elements++;

    if(L->head == NULL){
        L->head = newFile;
    } else{
        fPosL p = L->head;
        while (p->next != NULL){
            p = p->next;
        }
        p->next = newFile;
    }

    if (df >= 3 && df < MAX_OPEN_FILES) {
        OPEN_FILES[df].isUsed = 1;
        strcpy(OPEN_FILES[df].file_name, file_name);
        strcpy(OPEN_FILES[df].mode, md);
    }
}

void deleteFileinFileList(fList *L, int df){
    fPosL p = L->head;
    fPosL prev = NULL;

    while (p != NULL) {
        if (p->df == df) {
            if (close(df) == -1) {
                perror("Imposible cerrar descriptor");
            } else {
                if (p == L->head) {
                    L->head = p->next; 
                } else {
                    prev->next = p->next;
                }
                L->nm_elements--; 
                free(p);       
                printf("Archivo con descriptor %d cerrado.\n", df);

                if (df < MAX_OPEN_FILES) {
                    OPEN_FILES[df].isUsed = 0;
                    strcpy(OPEN_FILES[df].file_name, "");
                    strcpy(OPEN_FILES[df].mode, "");
                }
            }
            return;
        }
        prev = p;
        p = p->next;
    }
    // Si llegamos aquí, no estaba en la lista enlazada
    if (df < MAX_OPEN_FILES && OPEN_FILES[df].isUsed) {
        if (close(df) == -1) {
            perror("Imposible cerrar descriptor");
        } else {
            OPEN_FILES[df].isUsed = 0;
            strcpy(OPEN_FILES[df].file_name, "");
            strcpy(OPEN_FILES[df].mode, "");
            printf("Archivo con descriptor %d cerrado.\n", df);
        }
    } else {
        printf("No se encontró el archivo con descriptor %d.\n", df);
    }
}

void clearFileList(fList *L){
    if(L->head == NULL){
        printf("La lista de archivos no tiene elementos\n");
        return;
    }
    fPosL p = L->head;
    fPosL temp;
    while (p != NULL){
        temp = p->next; 
        close(p->df);
        if (p->df >= 3 && p->df < MAX_OPEN_FILES) {
            OPEN_FILES[p->df].isUsed = 0;
            strcpy(OPEN_FILES[p->df].file_name, "");
            strcpy(OPEN_FILES[p->df].mode, "");
        }
        free(p);  
        p = temp;
    }
    L->head = NULL;
    L->nm_elements = 0;
    printf("Se han cerrado todos los archivos que estaban abiertos.\n");
}

void printFiles(fList *L){
    printf("open\n");
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (i < 3 || OPEN_FILES[i].isUsed) {
            printf("descriptor: %d -> %s %s\n", i, OPEN_FILES[i].file_name, OPEN_FILES[i].mode);
        } else {
            printf("descriptor: %d -> no usado\n", i);
        }
    }
    fPosL p = L->head;
    while (p != NULL) {
        printf("-> dup %d (%s)\n", p->df, p->file_name);
        p = p->next;
    }
}

void closeFile(fList *L, int df){
    if(L->head==NULL){
        return;
    }
  
    fPosL p=L->head;
    fPosL prev=NULL;
  
    while(p != NULL && p->df != df){
        prev=p;
        p=p->next;
    }
  
    if(p==NULL){
        printf("No se encontro ningun archivo con el descriptor %d.\n", df);
        return;
    }
  
    if(close(p->df)==-1){
        perror("(perror) Error al cerrar el archivo: ");
        return;
    }
  
    if(prev==NULL){
        L->head=p->next;
    }else{
        prev->next=p->next;
    }
  
    free(p);
    printf("El archivo con el descriptor %d ha sido cerrado y eliminado de la lista de archivos abiertos.\n",df);

    // we dont let the user delete the 0 1 2 df
    if (df >= 3 && df < MAX_OPEN_FILES) {
        OPEN_FILES[df].isUsed = 0;
        strcpy(OPEN_FILES[df].file_name, "");
        strcpy(OPEN_FILES[df].mode, "");
    }
}

void dupFile(fList *L, int df){
    if(L->head==NULL){
        printf("La lista de archivos abiertos esta vacia.\n");
        return;
    }
  
    fPosL p=L->head;

    while(p != NULL && p->df != df){
        p=p->next;
    }
  
    if(p==NULL){
        printf("No se encontro ningun archivo con el descriptor %d.\n", df);
        return;
    }
  
    int newDf = dup(p->df);
    if(newDf==-1){
        perror("(perror) Error al duplicar el descriptor de archivo: ");
        return;
    }

    insertFile(L, newDf, p->file_name, "dup");
    printf("El descriptor %d ha sido duplicado. Nuevo descriptor: %d.\n",df,newDf);
}

char* getFileName(fList *L, int df) {
    static char standardName[32];

    if (df == 0) {
        snprintf(standardName, sizeof(standardName), "entrada estandar");
        return standardName;
    }
    if (df == 1) {
        snprintf(standardName, sizeof(standardName), "salida estandar");
        return standardName;
    }
    if (df == 2) {
        snprintf(standardName, sizeof(standardName), "error estandar");
        return standardName;
    }

    fPosL p = L->head;
    while (p != NULL) {
        if (p->df == df) {
            return p->file_name;
        }
        p = p->next;
    }

    return NULL;  
}