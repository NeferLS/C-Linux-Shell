#pragma once

#include "types.h"

typedef struct fNode* fPosL;

typedef struct fNode {
    int df;
    char file_name[MAX_SIZE];
    char md[10]; 
    fPosL next;
} fNode;

typedef struct {
    fNode *head;
    int nm_elements;
} fList;

typedef struct {
    int descriptor;
    char file_name[MAX_SIZE];
    char mode[10];
    int isUsed;
} FileInfo;


bool isEmptyFileList(fList L);
void initFileList(fList *L);
void insertFile(fList *L, int df, const char file_name[], const char md[]);
void deleteFileinFileList(fList *L, int df);
void clearFileList(fList *L);
void printFiles(fList *L);
void closeFile(fList *L, int df);
void dupFile(fList *L, int df);
char *getFileName(fList *L, int df);
