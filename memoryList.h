#include "types.h"

typedef enum {
    MALLOC,
    MMAP,
    SHMEM
} MemoryType;

typedef struct{
    void *address;
    size_t size;
    time_t alloc_time;
    MemoryType type;
    union {
        int key;           // Para memoria compartida
        struct {
            char filename[256];
            int fd;
        } mmap_info;       // Para archivos mapeados
    } extra_info;
} MemoryBlock;

typedef struct Node {
    MemoryBlock block;
    struct Node *next;
} Node;

typedef struct {
    Node *h;
} memList;

bool isEmptyMemList(memList L);
void initMemList(memList *L);
void addMemBlock(memList *L, MemoryBlock block);
void removeMALLOC(Node *current, size_t size, Node *prev, memList *List);
void removeMMAP(Node *current, const char *archivo, Node *prev, memList *List);
void removeSHMEM(Node *current, int clave, Node *prev, memList *List);
void removeADDR(Node *current, void* addr, Node *prev, memList *List);
void printMemoryBlocks(memList *L);
void clearMemoryList(memList *L);
Node* findSharedBlock(memList *L, int key);
