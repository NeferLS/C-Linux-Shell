// para variables globales
#ifndef GLOBALS_H
#define GLOBALS_H

#include "types.h"
#include "memoryList.h"
#include "fileList.h"
#include "HistoList.h"
#include "dirList.h"
#include "processList.h"

extern void *memFile;
extern memList MEMLIST;
extern hList HIS;
extern fList FILELIST;
extern dList DIRLIST;
extern pList PROCLIST;
extern FileInfo OPEN_FILES[MAX_OPEN_FILES];
extern char **environ;

#endif // GLOBALS_H