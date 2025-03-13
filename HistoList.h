#include "types.h"

typedef struct hNode* hPosL;

typedef struct hNode{
    char cmd[MAX_COMM];
    int nm;
    hPosL next;
}hNode;

typedef struct {
    hNode *head;
    int nm_elements;
}hList;

typedef struct {
    hList* history;
    int currentPos;
    int lastCommand;
} HistoryNav;

void initHistoricList(hList *L);
bool isEmptyHistoricList(hList L);
void insertHistoricItem(hList *L, const char tr[]);
void removeHistoricItem(hList *L);
void clearHistoricList(hList *L);
void printHistoricList(hList *L, int N);
char* getHistoricItem(hList *, int N);
void initHistoryNav(HistoryNav *nav, hList *L);
char *navUp(HistoryNav *nav);
char *navDown(HistoryNav *nav);
void resetNav(HistoryNav *nav);


