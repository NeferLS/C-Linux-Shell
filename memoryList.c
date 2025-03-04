#include "memoryList.h"

bool isEmptyMemList(memList L){
    return L.h==NULL;
}

void initMemList(memList *L) {
    L->h = NULL;
}

void addMemBlock(memList *L, MemoryBlock block) {
    Node *nodo = (Node *)malloc(sizeof(Node));
    if(!nodo){
        return;
    }
    nodo->block = block;
    nodo->next = L->h;
    L->h = nodo;
}

void removeMALLOC(Node *current, size_t size, Node *prev, memList *List){
    while (current != NULL){
       if(current->block.type == MALLOC 
       && current->block.size == size){
            free(current->block.address);
            printf("Liberados %zu bytes de memoria malloc en %p\n", size, current->block.address);

            if(prev == NULL){
                List->h = current->next;
            } else
                prev->next = current->next;

            free(current);
            return;
       }
       prev = current;
       current = current->next;
    }
    printf("No se encontró un bloque malloc de tamaño %zu bytes\n", size);
}

void removeMMAP(Node *current, const char *archivo, Node *prev, memList *List){
    while(current != NULL){
        if(current->block.type == MMAP 
        && strcmp(current->block.extra_info.mmap_info.filename, archivo) == 0){
            munmap(current->block.address, current->block.size);
            close(current->block.extra_info.mmap_info.fd);
            printf("Desmapeado archivo %s de la memoria en %p\n", archivo, current->block.address);

            if(prev == NULL){
                List->h = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("No se encontró un mapeo para el archivo %s\n", archivo);
}

void removeSHMEM(Node *current, int clave, Node *prev, memList *List){
    while(current != NULL){
        if(current->block.type == SHMEM 
        && current->block.extra_info.key == clave){
            shmdt(current->block.address);
            printf("Desvinculada memoria compartida con clave %d en %p\n", clave, current->block.address);
            
            if(prev == NULL){
                List->h = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("No se encontró un bloque de memoria compartida con clave %d\n", clave);
}

void removeADDR(Node *current, void* addr, Node *prev, memList *List){
    while (current != NULL) {
        if (current->block.address == addr) {
            switch (current->block.type) {
                case MALLOC:
                    printf("Liberados %zu bytes de memoria malloc en %p\n", current->block.size, addr);
                    free(addr);
                    break;
                case MMAP:
                    munmap(addr, current->block.size);
                    close(current->block.extra_info.mmap_info.fd);
                    printf("Desmapeado archivo %s de la memoria en %p\n", 
                           current->block.extra_info.mmap_info.filename, addr);
                    break;
                case SHMEM:
                    shmdt(addr);
                    printf("Desvinculada memoria compartida con clave %d en %p\n", 
                           current->block.extra_info.key, addr);
                    break;
            }
            
            if (prev == NULL) {
                List->h = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("No se encontró un bloque de memoria en la dirección %p\n", addr);
}

void printMemoryBlocks(memList *L) {
    Node *current = L->h;
    printf("%-18s %-10s %-10s %-20s %-10s %s\n", "Dir", "Size", "Mode", "File", "Key", "AT");
    while (current != NULL) {
        printf("%-18p %-10zu ", current->block.address, current->block.size);
        switch (current->block.type) {
            case MALLOC:
                printf("%-10s %-20s %-10s %s", "malloc", "-", "-", ctime(&current->block.alloc_time));
                break;
            case MMAP:
                printf("%-10s %-20s %-10s %s", "mmap", current->block.extra_info.mmap_info.filename, "-", ctime(&current->block.alloc_time));
                break;
            case SHMEM:
                printf("%-10s %-20s %-10d %s", "shared", "-", current->block.extra_info.key, ctime(&current->block.alloc_time));
                break;
        }
        current = current->next;
    }
}

void clearMemoryList(memList *L){
    if(L->h==NULL){
        return;
    }

    Node *current=L->h;
    Node *temp;

    while (current!=NULL){
        temp=current;
        current=current->next;
        free(temp);
    }
    L->h=NULL;
}

Node* findSharedBlock(memList *L, int key) {
    Node *current = L->h;
    while (current) {
        if (current->block.type == SHMEM && current->block.extra_info.key == key) {
            return current;
        }
        current = current->next;
    }
    return NULL;  
}
