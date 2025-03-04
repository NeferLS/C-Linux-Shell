#include "types.h"

typedef enum {
    FINISHED,
    STOPPED,
    SIGNALED,
    ACTIVE
} ProcessStateType;

typedef struct pNode {
    pid_t pid;
    time_t launch_time;
    struct tm *date;
    ProcessStateType type;
    char command[256];
    int priority;
    struct pNode *next;
} pNode;

typedef struct {
    pNode *head;
    int numElements;
} pList;

void initProcessList(pList *L);
void addProcess(pList *L, pid_t pid, const char *command, ProcessStateType type, int priority);
void showProcessList(pList L);
void changeState(pList *L, pid_t pid, ProcessStateType nuevoEstado);
void deleteProcessByState(pList *L, ProcessStateType estado);
void clearProcessList(pList *L);