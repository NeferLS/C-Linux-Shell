#include "fileList.h"

void *memFile = NULL;
FileInfo OPEN_FILES[MAX_OPEN_FILES];

// inits fundamental df
void initializeOpenFiles() {
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        OPEN_FILES[i].descriptor = i;
        strcpy(OPEN_FILES[i].file_name, "");
        strcpy(OPEN_FILES[i].mode, "");
        OPEN_FILES[i].isUsed = 0;
    }
    
    OPEN_FILES[0].isUsed = 1;
    strcpy(OPEN_FILES[0].file_name, "standard input");
    strcpy(OPEN_FILES[0].mode, "O_RDWR");
    
    OPEN_FILES[1].isUsed = 1;
    strcpy(OPEN_FILES[1].file_name, "standard output");
    strcpy(OPEN_FILES[1].mode, "O_RDWR");
    
    OPEN_FILES[2].isUsed = 1;
    strcpy(OPEN_FILES[2].file_name, "standard error");
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
        perror("Couldn't assign memory to the file.\n");
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
                perror("Couldn't close df.\n");
            } else {
                if (p == L->head) {
                    L->head = p->next; 
                } else {
                    prev->next = p->next;
                }
                L->nm_elements--; 
                free(p);       
                printf("File with df = %d closed.\n", df);

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
    // outside of list
    if (df < MAX_OPEN_FILES && OPEN_FILES[df].isUsed) {
        if (close(df) == -1) {
            perror("Couldn't close df.\n");
        } else {
            OPEN_FILES[df].isUsed = 0;
            strcpy(OPEN_FILES[df].file_name, "");
            strcpy(OPEN_FILES[df].mode, "");
            printf("File with df = %d closed.\n", df);
        }
    } else {
        fprintf(stderr, "Couldn't find file with df = %d\n", df);
    }
}

void clearFileList(fList *L){
    if(L->head == NULL){
        fprintf(stderr, "No elements in File List.\n");
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
    printf("All files have been closed.\n");
}

void printFiles(fList *L){
    printf("open\n");
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (i < 3 || OPEN_FILES[i].isUsed) {
            printf("df: %d -> %s %s\n", i, OPEN_FILES[i].file_name, OPEN_FILES[i].mode);
        } else {
            printf("df: %d -> (free) \n", i);
        }
    }
    fPosL p = L->head;
    while (p != NULL) {
        printf("-> (dup) %d (%s)\n", p->df, p->file_name);
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
        fprintf(stderr, "Couldn't find file with df = %d\n", df);
        return;
    }
  
    if(close(p->df)==-1){
        perror("Couldn't close file\n");
        return;
    }
  
    if(prev==NULL){
        L->head=p->next;
    }else{
        prev->next=p->next;
    }
    free(p);
    printf("File with df = %d closed and freed from File List.\n", df);

    /*we dont let the user delete the 0 1 2 df, we can change this
    if (df >= 3 && df < MAX_OPEN_FILES) {
        OPEN_FILES[df].isUsed = 0;
        strcpy(OPEN_FILES[df].file_name, "");
        strcpy(OPEN_FILES[df].mode, "");
    } */
}

void dupFile(fList *L, int df){
    if(L->head==NULL){
        fprintf(stderr, "File List empty.");
        return;
    }
  
    fPosL p=L->head;

    while(p != NULL && p->df != df){
        p=p->next;
    }
  
    if(p==NULL){
        fprintf(stderr, "Couldn't find file with df = %d\n", df);
        return;
    }
  
    int newDf = dup(p->df);
    if(newDf==-1){
        perror("Couldn't duplicate file.");
        return;
    }

    insertFile(L, newDf, p->file_name, "dup");
    printf("df = %d dupped successfully. New df -> %d.\n",df,newDf);
}

char* getFileName(fList *L, int df) {
    static char standardName[32];

    if (df == 0) {
        snprintf(standardName, sizeof(standardName), "standard input");
        return standardName;
    }
    if (df == 1) {
        snprintf(standardName, sizeof(standardName), "standard output");
        return standardName;
    }
    if (df == 2) {
        snprintf(standardName, sizeof(standardName), "standard error");
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