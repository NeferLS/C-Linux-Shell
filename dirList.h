#include "types.h"

typedef struct dNode{
    char directory[256];
    struct dNode *next;
} dNode;

typedef struct{
    dNode *head;
    int numElements;
}dList;

// Declaraciones de funciones para manejar la lista
void initDirList(dList *L);
void addDir(dList *L, const char *directory);
bool deleteDir(dList *L, const char *directory);
void showDirList(dList L);
void clearDirList(dList *L);
void importPath(dList *L);
char *searchListFirst(dList L); // Devuelve el primer directorio
char *searchListNext(dList L, const char *current); // Devuelve el siguiente directorio