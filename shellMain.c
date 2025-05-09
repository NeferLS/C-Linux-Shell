/*
This is the main page, with all the commands
*/

#include "shellMain.h" 
#include "globals.h"
#include "auxFunctions.h"

// global variables
hList HIS;
fList FILELIST;
memList MEMLIST;
dList DIRLIST;
pList PROCLIST;

char dirUse[MAX_COMM];

void Cmd_pid(char *tr[]) {
    printf("PID: %d\n", getpid());
}

void Cmd_ppid(char *tr[]) {
    printf("PPID: %d\n", getppid());
}

//prints user and computer info
void Cmd_whoisme(char *tr[]) {
    const char *name = getUsername();
    struct utsname sysInfo;

    if (uname(&sysInfo)==-1){
        perror("Error obtaining the system information...\n");
        return;
    }

    printf("\nUser: %s\n", name);
    printf("SO: %s\n",sysInfo.sysname);
    printf("SO version: %s\n",sysInfo.release);
    printf("Node name: %s\n",sysInfo.nodename);
    printf("Kernel version: %s\n",sysInfo.version);
    printf("Architecture: %s\n",sysInfo.machine);
}

// equivalent of cd
void Cmd_chdir(char *tr[]) {
    if (tr[0] == NULL) {
        if (getcwd(dirUse, MAX_COMM) != NULL) {
            printf("%s\n", dirUse);
        } else {
            perror("Couldn't get the current working directory");
        }
    } else if (chdir(tr[0]) == -1) {
        perror("Couldn't found the directory");
    }
}

void Cmd_exit(char *tr[]) {
        exit(0);
}

//date and time of the sistem
void Cmd_date(char *tr[]) {
    time_t now = time(NULL);
    struct tm *local = localtime(&now);

    if (tr[0] == NULL || (strcmp(tr[0], "-?") == 0)){
        printf("use: date [-d(ate)][-t(ime)]\n");
        return;
    } else if (strcmp(tr[0], "-d") == 0) { // date
        printf("%02d/%02d/%d\n", local->tm_mday, local->tm_mon + 1, local->tm_year + 1900);
    } else if (strcmp(tr[0], "-t") == 0) { // time
        printf("%02d:%02d:%02d\n", local->tm_hour, local->tm_min, local->tm_sec);
    } else { 
        printf("Invalid argument for <date>\n");
    }
}

// history of used commands
void Cmd_history(char *tr[]){
    if (tr[0] == NULL) { // only the list
        printf("\n*Used Commands List*\n");
        printHistoricList(&HIS, 0);
    } else { 
        int N = atoi(tr[0]);
        if (N > 0) { //case historic N
            char *command = getHistoricItem(&HIS, N); 
            if (command != NULL) { 
                printf("Repeats the command number... %d from the list: %s\n", N, command);
                char *args[MAX_COMM / 2];
                funcBreakLine(command, args);  
                DoCommand(args); 
            }
        } else if (N < 0) { //case historic -N
            N = -N;  
            printf("\nShowing the last %d commands of the list\n", N);
            printHistoricList(&HIS, -N); 
        } else { 
            perror("Invalid argument for 'historic' \n");
        }
    }
}


void Cmd_help(char *tr[]) {
    if (tr == NULL || tr[0] == NULL) {
        for (int i = 0; comandos[i] != NULL; i++) {
            printf(" - %s\n", comandos[i]);
        }
        return;
    }
    if((strcmp(tr[0], "-?") == 0)){
        printf("use: help [command]\n");
        return;
    }
    for (int i = 0; comandos[i] != NULL; i++) {
        if (strstr(comandos[i], tr[0]) != NULL) {
            printf("%s\n", comandos[i]);
            return;
        }
    }
    printf("Command '%s' no found. Use 'help' to see the available commands.\n", tr[0]);
}

// Opens a file with a mode
void Cmd_open(char *tr[]) {
    int df, mode = 0;
    char modo_str[50] = ""; 

    if (tr[0] == NULL) {
        printFiles(&FILELIST);
        return;
    }

    if (tr[1] == NULL) {
        printf("Error: No access mode was assigned. Use 'help' for more information.\n");
        return;
    }

    for (int i = 1; tr[i] != NULL; i++) {
        if (!strcmp(tr[i], "cr")) { mode |= O_CREAT; strcat(modo_str, "c"); }
        else if (!strcmp(tr[i], "ex")) { mode |= O_EXCL; strcat(modo_str, "e"); }
        else if (!strcmp(tr[i], "ro")) { mode |= O_RDONLY; strcat(modo_str, "r"); }
        else if (!strcmp(tr[i], "wo")) { mode |= O_WRONLY; strcat(modo_str, "w"); }
        else if (!strcmp(tr[i], "rw")) { mode |= O_RDWR; strcat(modo_str, "rw"); }
        else if (!strcmp(tr[i], "ap")) { mode |= O_APPEND; strcat(modo_str, "a"); }
        else if (!strcmp(tr[i], "tr")) { mode |= O_TRUNC; strcat(modo_str, "t"); }
        else { 
            printf("Error: Access mode '%s' invalid.\n", tr[i]);
            return;
        }
    }

    if ((df = open(tr[0], mode, 0644)) == -1) {
        perror("Impossible to open the file");
    } else {
        insertFile(&FILELIST, df, tr[0], modo_str);
        printf("File '%s' opened with descriptor %d with access mode %s.\n", tr[0], df, modo_str);
    }
}

// Duplicates an open file with its fd
void Cmd_dup(char *tr[]) {
    int df, duplicado;

    if (tr[0] == NULL) {
        printFiles(&FILELIST);
        return;
    }

    if ((strcmp(tr[0], "-?") == 0)) {
        printf("use: dup [descriptor]\n");
        return; 
    }

    df = atoi(tr[0]);
    if (df < 0) {
        printf("Error: Invalid descriptor. Cannot be a negative integer.\n");
        return;
    }

    duplicado = dup(df);
    if (duplicado == -1) {
        perror("Error duplicating the file");
        return;
    }

    char *fileName = getFileName(&FILELIST, df);
    if (fileName == NULL) {
        printf("Couldn't find the file.\n");
        close(duplicado); 
        return;
    }
    
    insertFile(&FILELIST, duplicado, fileName, "O_RDWR");
    printf("Descriptor %d duplicated. New descriptor: %d.\n", df, duplicado);
}

// Closes opened files
void Cmd_close(char *tr[]) {
    int df;
    if (tr[0] == NULL) {
        printFiles(&FILELIST);
        return;
    }
    if ((strcmp(tr[0], "-?") == 0)) {
        printf("use: close [descriptor]\n");
        return; 
    }
    df = atoi(tr[0]);

    if (tr[0][0] < '0' || tr[0][0] > '9' || df < 0) {
        printf("Error: Invalid descriptor. Cannot be a negative integer.\n");
        return;
    }
    
    closeFile(&FILELIST,df);
    
}

// mkdir
void Cmd_mkdir(char *tr[]){
    if (tr[0] == NULL || (strcmp(tr[0], "-?") == 0))
    {
      printf("use: mkdir [name]\n");
      return;  
    }
    if (mkdir(tr[0], 0400 | 0200 | 0100) == -1) {
        printf("Error trying to create directory: %s\n", strerror(errno));
    }
}

// creates a file
void Cmd_mkfile (char *tr[]){
    int df;

    if (tr[0]==NULL || (strcmp(tr[0], "-?") == 0)){
        printf("use: mkfile [name]");
        return;  
    }
    if ((df=open(tr[0],O_CREAT | O_RDWR, 0777))==-1){
        printf("Error creating file: %s\n", strerror(errno));
    }
}

// lists files
void Cmd_listfile(char *tr[])
{
    if (tr[0] == NULL || (strcmp(tr[0], "-?") == 0)) {
        printf("use: listfile [-long] [-link] [-acc]\n");
        return;
    }
    int lng = 0, acc = 0, lnk = 0;
    char *directories[100]; 
    int dirCount = 0;
    processAndGetDirectory(tr,directories, &dirCount, NULL, &lng, &acc, &lnk);
    if (dirCount==0)
    {
        perror("At least one directory has to be specified.\n");
        return;
    }
    for (int i = 0;i<dirCount ; i++) {
        aux_listFilesInDir(directories[i], lng, acc, lnk);
    } 

}

// lists directories
void Cmd_listdir(char *tr[]){
    int showHidden = 0, longFormat = 0, showAccessTime = 0, showLink = 0;
    char *directories[100];
    int dirCount = 0;

    if (tr[0] == NULL || (strcmp(tr[0], "-?") == 0)) {
        printf("use: listdir [-hid][-long][-link][-acc]\n");
        return;
    }
    processAndGetDirectory(tr,directories, &dirCount, &showHidden, &longFormat, &showAccessTime, &showLink);

    for(int i=0; i<dirCount; i++){
        DIR *dir;
        struct dirent *entry;

        //verificar si el directorio ya esta abierto
        if((dir=opendir(directories[i]))==NULL){
            perror("opendir");
            continue;
        }
        printf("************%s\n",directories[i]);

        while ((entry=readdir(dir))!=NULL){
            aux_printDir(directories[i], entry, showHidden, longFormat, showAccessTime, showLink);
        }
    closedir(dir);
    }
}

// lists files and directories recursively
void Cmd_reclist(char *tr[]){
    int hid = 0, lng = 0, acc = 0, lnk = 0;
    char *dirNum[100];
    int dirCount = 0;

    if (tr[0] == NULL || (strcmp(tr[0], "-?") == 0)) {
        printf("use: reclist [-hid][-long] [-link] [-acc]\n");
        return;
    }
    processAndGetDirectory(tr, dirNum, &dirCount, &hid, &lng, &acc, &lnk);

    for (int i = 0; i < dirCount; i++) {
        aux_listDir(dirNum[i], hid, lng, acc, lnk, 0); // 0 for recList
    }
}

// same as recList but in inverse order
void Cmd_revlist(char *tr[]){
    int hid = 0, lng = 0, acc = 0, lnk = 0;
    char *dirNum[100];
    int dirCount = 0;
    if (tr[0] == NULL || (strcmp(tr[0], "-?") == 0)) {
        printf("use: revlist [-hid][-long][-link][-acc]\n");
        return;
    }
    processAndGetDirectory(tr, dirNum, &dirCount, &hid, &lng, &acc, &lnk);

    for (int i = 0; i < dirCount; i++) {
        aux_listDir(dirNum[i], hid, lng, acc, lnk, 1); // 1 for revList
    }
}

// deletes an empty dir
void Cmd_erase(char *tr[]){
    if (tr[0] == NULL || (strcmp(tr[0], "-?") == 0)) {
        printf("use: erase [file, dir]\n");
        return;
    }
    aux_erase(tr);
}

// deletes dir recursively
void Cmd_delrec(char *tr[]){
    if (tr[0] == NULL || (strcmp(tr[0], "-?") == 0)) {
        printf("use: delrec [directorio]\n");
        return;
    }
    aux_delrec(tr[0]);
}

// allocates memory
void Cmd_allocate(char *tr[]) {
    if (tr[0] == NULL || tr[1] == NULL){
        aux_memory_blocks(&MEMLIST);
        return;
    } else if(strcmp(tr[0], "-?") == 0){
        printf("use: allocate [-malloc, size] [-mmap, file, mode] [-shared, keyNum, size] [-createshared, keyNum, size]\n");
        return;
    } else if (strcmp(tr[0], "-malloc") == 0 && tr[1] != NULL) {
        aux_allocate_malloc((size_t)atoll(tr[1])); 
    } else if (strcmp(tr[0], "-mmap") == 0 && tr[1] != NULL && tr[2] != NULL) { 
        aux_allocate_nmap(tr[1], tr[2]); 
    } else if (strcmp(tr[0], "-shared") == 0 && tr[1] != NULL && tr[2] != NULL) {
        aux_allocate_shared(atoi(tr[1]), atoi(tr[2]));
    } else if (strcmp(tr[0], "-createshared") == 0 && tr[1] != NULL && tr[2] != NULL) {
        aux_allocate_createshared(atoi(tr[1]), (size_t)atoll(tr[2]));
    }
}

// deallocates memory
void Cmd_deallocate(char *tr[]){
    if (tr[0] == NULL) {
        aux_memory_blocks(&MEMLIST);
        return;
    } else if(strcmp(tr[0], "-?") == 0){
        printf("use: allocate [-malloc, size] [-mmap, archivo, tipo_apertura] [-shared, num_clave, size]\n");
        return;
    }else if (strcmp(tr[0], "-malloc") == 0 && tr[1]!=NULL) {
        aux_deallocate_malloc((size_t)atoll(tr[1])); 
    } else if (strcmp(tr[0], "-mmap") == 0 && tr[1]!=NULL) { 
        aux_deallocate_mmap(tr[1]);
    } else if (strcmp(tr[0], "-shared") == 0 && tr[1]!=NULL) {
        aux_deallocate_shared(atoi(tr[1])); 
    } else if (strcmp(tr[0], "-delkey") == 0 && tr[1]!=NULL) {
        aux_deallocate_delkey(atoi(tr[1]));
    } else {
        void *addr = (void *)strtol(tr[0], NULL, 16);
        aux_deallocate_addr(&MEMLIST, addr);
    }   
    
}

void Cmd_memfill(char *tr[]){
    if(tr[0]==NULL||tr[1]==NULL||tr[2]==NULL){
        fprintf(stderr, "use: memfill [addr] [cont] [ch]\n");
        return;
    }
    
    aux_mem(tr, false);
}

void Cmd_memdump(char *tr[]){
    if(tr[0]==NULL||tr[1]==NULL){
        fprintf(stderr, "use: memdummp [addr] [cont]\n");
        return;
    }
    aux_mem(tr, true);
}

void Cmd_memory(char *tr[]){
    if (tr[0] == NULL) {
        aux_memory_all(); 
        return;
    } else if ((strcmp(tr[0], "-?") == 0)){
        printf("use: memory [-funcs] [-vars] [-blocks], [-all], [-pmamp]\n");
        return;
    } else if (strcmp(tr[0], "-funcs") == 0) {
        aux_memory_funcs();
    } else if (strcmp(tr[0], "-vars") == 0) { 
        aux_memory_vars(); 
    } else if (strcmp(tr[0], "-blocks") == 0) {
        aux_memory_blocks(&MEMLIST); 
    } else if (strcmp(tr[0], "-all") == 0) {
        aux_memory_all(); 
    } else if (strcmp(tr[0], "-pmap") == 0) {
        aux_memory_pmap();
    }
}

void Cmd_read(char *tr[]){
    if(tr[0]==NULL || tr[1]==NULL || tr[2]==NULL || (strcmp(tr[0], "-?") == 0)){
        fprintf(stderr, "use: read [df][memAddrs][nBytes]\n");
        return;
    }
    aux_read(tr);
}

void Cmd_readfile(char *tr[]) {
    if (tr[0] == NULL || tr[1] == NULL || tr[2] == NULL || (strcmp(tr[0], "-?") == 0)) {
        printf("use: readfile [file name][memAddrs][nBytes]\n");
        return;
    }
    aux_readFile(tr);
}

void Cmd_write(char *tr[]){
    if (tr[0] == NULL || tr[1] == NULL || tr[2] == NULL
        ||(strcmp(tr[0],"-o")==0 && tr[3]==NULL)){
         fprintf(stderr, "use: write[-o][fd][memAddrs][size]\n");
        return;
    }
    aux_write(tr);
}

void Cmd_writefile(char *tr[]) {
    if (tr[0] == NULL || tr[1] == NULL || tr[2] == NULL
        ||(strcmp(tr[0],"-o")==0 && tr[3]==NULL)){
         fprintf(stderr, "use: writefile[-o][file][memAddrs][size]\n");
        return;
    }


    int overWrite=0;
    int argIndex=0;

    if(strcmp(tr[0],"-o")==0){
        overWrite=1;
        argIndex=1;
    }

    const char *file=tr[argIndex];

    if(file==NULL){
        fprintf(stderr, "error: file not specified.\n");
        return;
    }
    
    char *endPtr;
    void *addr=(void *)strtoul(tr[1+argIndex],&endPtr,16);
    if(*endPtr!='\0'){
        fprintf(stderr, "error: memory address '%s' not valid.\n", tr[argIndex + 1]);
        return;
    }

    size_t cont= (size_t)strtoul(tr[2+argIndex],&endPtr,10);
    if (*endPtr != '\0' || cont == 0) {
        fprintf(stderr, "error: size '%s' not valid.\n", tr[argIndex + 2]);
        return;
    }

    aux_writefile(file, addr, cont, overWrite);
}

void Cmd_recurse(char *tr[]){
    if(tr[0]==NULL || (strcmp(tr[0], "-?") == 0)){
        fprintf(stderr, "use: recurse [n]\n");
        return;
    }

    int n=atoi(tr[0]);
    if(n<=0){
        fprintf(stderr, "error: n should be bigger than 0.\n");
        return;
    }

    static int arr_static=0;
    aux_recurse(n, &arr_static);
}

void Cmd_clear(char *tr[]){
    system("clear");
}

void Cmd_getuid(char *tr[]) {
    printf("user UID: %d\n", getuid());
    printf("user eUID: %d\n", geteuid());
    printf("group UID: %d\n", getgid());
}

void Cmd_setuid(char *tr[]) {
    if (tr[0] == NULL || strcmp(tr[0], "-?") == 0) {
        fprintf(stderr, "use: setuid [-l][uid]\n");
        return;
    }
    if (tr[1] != NULL && (strcmp(tr[1], "-l") == 0 || strcmp(tr[1], "-L") == 0)) {
        printf("Login mode detected (-l).\n");
        aux_setuid(tr +1 , 1);
    } else {
        aux_setuid(tr, 0);
    }
}

void Cmd_showvar(char *tr[]) {
    int i;
    
    if (tr[0] == NULL) {
        fprintf(stderr, "use: showvar [var1][var2] ...\n");
        return;
    }
    
    for (i = 0; tr[i] != NULL; i++) {
        aux_showvar(tr[i]);
    }
}

void Cmd_changevar(char *tr[]) {
    char *var, *val;
    int opt = 0; // 0: setenv, 1: putenv

    if (tr[0] == NULL || tr[1] == NULL || (strcmp(tr[0], "-p") == 0 && tr[2] == NULL)) {
        fprintf(stderr, "use: changevar [-a|-e|-p][var][value]\n");
        return;
    }

    if (strcmp(tr[0], "-a") == 0 || strcmp(tr[0], "-e") == 0) {
        var = tr[1];
        val = tr[2];
    } else if (strcmp(tr[0], "-p") == 0) {
        opt = 1;
        var = tr[1];
        val = tr[2];
    } else {
        var = tr[0];
        val = tr[1];
    }

    if (aux_changevar(var, val, opt) != 0) {
        perror("error trying to change the environ var");
    }
}

void Cmd_subsvar(char *tr[]) {
    char *var1, *var2, *val;

    if (tr[0] == NULL || tr[1] == NULL || tr[2] == NULL) {
        fprintf(stderr, "use: subsvar [-a|-e][var1][var2][value]\n");
        return;
    }

    if (strcmp(tr[0], "-a") == 0 || strcmp(tr[0], "-e") == 0) {
        var1 = tr[1];
        var2 = tr[2];
        val = tr[3];
    } else {
        var1 = tr[0];
        var2 = tr[1];
        val = tr[2];
    }

    if (aux_subsvar(var1, var2, val) != 0) {
        perror("error trying to substitute the environ var");
        return;
    }

    printf("var %s deleted\n", var1);
    printf("var %s created with value %s\n", var2, val);
}

void Cmd_environ(char *tr[]) {
    if (tr[0] != NULL && strcmp(tr[0], "-environ") == 0) {
        aux_environ_addr(0);
    } else if (tr[0] != NULL && strcmp(tr[0], "-addr") == 0) {
        aux_environ_addr(1);
    } else {
        aux_environ_print();
    }
}

void Cmd_fork(char *tr[]){
    pid_t pid;

    if((pid=fork())==0){
        printf("Executing process %d...\n", getpid());
    } else if(pid != -1){
        waitpid(pid, NULL, 0);
    }
}

void Cmd_search(char *tr[]){
    if(tr[0]==NULL){
        showDirList(DIRLIST);
    }else if (strcmp(tr[0], "-add") == 0 && tr[1] != NULL) {
        addDir(&DIRLIST, tr[1]);
    } else if (strcmp(tr[0], "-del") == 0 && tr[1] != NULL) {
        if (deleteDir(&DIRLIST, tr[1])) {
            printf("Directory succesfully deleted: %s\n", tr[1]);
        } else {
            printf("Directory %s is not on the list.\n", tr[1]);
        }
    } else if (strcmp(tr[0], "-clear") == 0) {
        clearDirList(&DIRLIST);
        printf("Search list successfully emptied.\n");
    } else if (strcmp(tr[0], "-path") == 0) {
        importPath(&DIRLIST);
        printf("Directories imported from PATH\n");
    } else {
        printf("use: search [-add dir][-del dir][-clear][-path]\n");
    }
}

void Cmd_exec(char *tr[]){
    if (tr[0] == NULL) {
        fprintf(stderr, "error: a program has not been selected\n");
        return;
    }

    if (tr[0][0] == '-') {
        fprintf(stderr, "use: exec progspec [args...]\n");
        return;
    }
    char *program=NULL;
    char **newEnv=NULL;
    int envCount=0;

    for(int i=0; tr[i]!=NULL; i++){
        if(strchr(tr[i], '=')==NULL && getenv(tr[i])!=NULL){
            envCount++;
        }else{
            program=tr[i];
            break;
        }
    }

    if(program==NULL){
        fprintf(stderr,"error: a program has not been selected for execution\n");
        return;
    }
    if(envCount>0){
        newEnv=malloc((envCount+1)*sizeof(char *));
        if (newEnv == NULL) {
            perror("Address memory assignation error. ");
            return;
        }
        int index=0;
        for(int i=0; tr[i]!=NULL && index<envCount;i++){
            if(getenv(tr[i])!=NULL)
                newEnv[index++]=strdup(tr[i]);
        }
        newEnv[index]=NULL;
    }
    aux_execProgram(program, tr, newEnv);
}

void Cmd_execpri(char *tr[]){
    if (tr[0] == NULL || tr[1] == NULL) {
        fprintf(stderr, "use: execpri prio progspec [args...]\n");
        return;
    }
    char *endptr;
    int priority = strtol(tr[0], &endptr, 10); // b10
    if (*endptr != '\0') {
        fprintf(stderr, "error: priority has to be an integer\n");
        return;
    }

    if (priority < -20 || priority > 19) {
        fprintf(stderr, "error: priority has to be an integer from [-20,19]\n");
        return;
    }

    if (priority < 0 && getuid() != 0) {
        fprintf(stderr, "error: admin privileges are needed for negative priorities\n");
        return;
    }

    // change prio
    if (setpriority(PRIO_PROCESS, getpid(), priority) == -1) {
        fprintf(stderr, "Error changing priority: %s\n", strerror(errno));
        return;
    }

    // identify environ var and program
    char *program = NULL;
    char **newEnv = NULL;
    int envCount = 0;

    for (int i = 1; tr[i] != NULL; i++) {
        if (strchr(tr[i], '=') == NULL && getenv(tr[i]) != NULL) {
            envCount++;
        } else {
            program = tr[i];
            break;
        }
    }

    if (program == NULL) {
        fprintf(stderr, "Error: A process to run has not been specified.\n");
        return;
    }

    // create env if necessary 
    if (envCount > 0) {
        newEnv = malloc((envCount + 1) * sizeof(char *));
        if (newEnv == NULL) {
            perror("Error: couldn't assign memory to the process\n");
            return;
        }
        int index = 0;
        for (int i = 1; tr[i] != NULL && index < envCount; i++) {
            if (getenv(tr[i]) != NULL)
                newEnv[index++] = strdup(tr[i]);
        }
        newEnv[index] = NULL;
    }

    // exec process
    aux_execProgram(program, &tr[1], newEnv);
}

void Cmd_fg(char *tr[]){
    if(tr[0]==NULL){
        fprintf(stderr, "Error: A process to run has not been specified.\n");
        return;
    }

    pid_t pid=fork();
    if(pid==-1){
        perror("Error: couldn't create child process.\n");
        return;
    }
    if(pid==0){
        //child process
        aux_execProgram(tr[0], tr, NULL);
    }else{
        //process
        aux_waitForChild(pid);
    }
}

void Cmd_fgpri(char *tr[]){
    if (tr[0] == NULL || tr[1] == NULL) {
        fprintf(stderr, "use: fgpri prio prog args...\n");
        return;
    }

    char *endptr;
    int priority = strtol(tr[0], &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stderr, "Error: Priority can only be an integer value.\n");
        return;
    }

    if (priority < -20 || priority > 19) {
        fprintf(stderr, "Error: Priority can only be between [-20,19].\n");
        return;
    }
    pid_t pid=fork();
    if(pid==-1){
        perror("Error: couldn't create child process.\n");
        return;
    }

    if(pid==0){
        //child
        if (aux_setChildPriority(priority) == -1) {
            exit(EXIT_FAILURE); //failure if prio changes
        }
        aux_execProgram(tr[1], &tr[1], NULL);
    }else{
        //process
        aux_waitForChild(pid);
    }
}

void Cmd_back(char *tr[]) {
   if (tr[0] == NULL) {
        fprintf(stderr, "use: back progspec [args...]\n");
        return;
    }

    pid_t pid = fork();
    if (pid == 0){
        execvp(tr[0], tr);
        perror("Couldn't execute the process");
        exit(EXIT_FAILURE);
    } else if(pid > 0){
        addProcess(&PROCLIST, pid, tr[0], ACTIVE, 0);
        printf("Process %d executing in the background.\n", pid);
    } else{
        perror("Couldn't create child process.\n");
    }
}

void Cmd_backpri(char *tr[]) {
    if (tr[0]== NULL || tr[1]==NULL){
        fprintf(stderr, "use: backpri prio progspec [args...]\n");
        return;
    }

    char *endptr;
    int prio = strtol(tr[0], &endptr, 10);

    aux_isPrioOkay(endptr, prio);

    pid_t pid = fork();
    if (pid == 0){
        if (setpriority(PRIO_PROCESS, getpid(), prio) == -1){
            perror("Error al establecer prioridad");
            exit(EXIT_FAILURE);
        }
        execvp(tr[1], &tr[1]);
        perror("Couldn't execute the process");
        exit(EXIT_FAILURE);
        
    } else if (pid > 0){
        addProcess(&PROCLIST, pid, tr[1], ACTIVE, prio);
        printf("Proceso %d ejecutandose en segundo plano con prioridad %d\n", pid, prio);
    } else{
        perror("Error al crear el proceso hijo");
    }

}


void Cmd_listjobs() {
    showProcessList(PROCLIST);
}


void Cmd_deljobs(char *tr[]) {
    if (tr[0] == NULL) {
        fprintf(stderr, "Uso: deljobs -term|-sig\n");
        return;
    }

    if (strcmp(tr[0], "-term") == 0) {
        deleteProcessByState(&PROCLIST, FINISHED);
        printf("Procesos terminados eliminados.\n");
    } else if (strcmp(tr[0], "-sig") == 0) {
        deleteProcessByState(&PROCLIST, SIGNALED);
        printf("Procesos señalizados eliminados.\n");
    } else {
        fprintf(stderr, "Error: opción no válida. Usa -term o -sig.\n");
    }
}


void Cmd_external(char *tr[]) {
    pid_t pid = fork();
    if (pid == 0) {
        // Proceso hijo
        execvp(tr[0], tr);
        perror("Error al ejecutar el programa");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Proceso padre
        waitpid(pid, NULL, 0);
    } else {
        perror("Error al crear el proceso hijo");
    }
}

void clearLine(int len) {
    printf("\r%*s\r", len, "");
}


// definición del array de comandos
// si ponemos lo que hay en "", vamos a la funcion correspondiente
struct CMD C[] = {
    {"whoisme", Cmd_whoisme},
    {"pid", Cmd_pid},
    {"ppid", Cmd_ppid},
    {"cd", Cmd_chdir},
    {"exit", Cmd_exit},
    {"bye", Cmd_exit},
    {"quit", Cmd_exit},
    {"date", Cmd_date},
    {"help", Cmd_help},
    {"historic", Cmd_history},
    {"open", Cmd_open},
    {"close", Cmd_close},
    {"dup", Cmd_dup},
    {"mkdir", Cmd_mkdir},
    {"mkfile", Cmd_mkdir},
    {"listfile", Cmd_listfile},
    {"listdir", Cmd_listdir},
    {"reclist", Cmd_reclist},
    {"revlist", Cmd_revlist},
    {"erase", Cmd_erase},
    {"delrec", Cmd_delrec},
    {"allocate", Cmd_allocate},
    {"deallocate", Cmd_deallocate},
    {"read", Cmd_read},
    {"write", Cmd_write},
    {"memfill", Cmd_memfill},
    {"memdump", Cmd_memdump},
    {"memory", Cmd_memory},
    {"readfile", Cmd_readfile},
    {"writefile",Cmd_writefile},
    {"recurse", Cmd_recurse},
    {"clear", Cmd_clear},
    {"getuid", Cmd_getuid},
    {"setuid", Cmd_setuid},
    {"showvar", Cmd_showvar},
    {"changevar", Cmd_changevar},
    {"subsvar", Cmd_subsvar},
    {"environ", Cmd_environ},
    {"fork", Cmd_fork},
    {"search", Cmd_search},
    {"exec", Cmd_exec},
    {"execpri", Cmd_execpri},
    {"fg", Cmd_fg},
    {"fgpri", Cmd_fgpri},
    {"back", Cmd_back},
    {"backpri", Cmd_backpri},
    {"listjobs", Cmd_listjobs},
    {"deljobs", Cmd_deljobs},
    {"*****", Cmd_external},
    {NULL, NULL}
};

// permite leer y ejecutar el comando
void DoCommand(char *tr[]) {
    int i;
    char command[MAX_COMM] = "";

    if (tr[0] == NULL) { // solo el comando, sin parametros
        return;
    }
    for (i = 0; tr[i] != NULL; i++) { // concatena comando con parametros
        strcat(command, tr[i]);
        if (tr[i + 1] != NULL) {
            strcat(command, " ");
        }
    }
    insertHistoricItem(&HIS, command); // inserta en el historial el comando
    for (i = 0; C[i].name != NULL; i++) { // comprueba el nombre con lo que tenemos en la funcion del CMD
        if (!strcmp(tr[0], C[i].name)) {
            (*C[i].function)(tr + 1);
            return;
        } 
    }
    printf("sso: command not found: %s \n", tr[0]);//
}

int funcBreakLine(char *line, char *pz[]) {
    int i = 1;
    if ((pz[0] = strtok(line, " \n\t")) == NULL)
        return 0;
    while ((pz[i] = strtok(NULL, " \n\t")) != NULL)
        i++;
    return i;
}

int main(int argc, char *argv[]) {
    char line[MAX_COMM];
    char *pcs[MAX_COMM / 2];

    // initializing shell lists 
    initHistoricList(&HIS); 
    initFileList(&FILELIST); 
    initMemList(&MEMLIST);
    initDirList(&DIRLIST);
    initProcessList(&PROCLIST); 
    
    const char *username = getUsername();
    const char *osname = getOsName();
    while (1) {
        printf("%s@%s-%s:~ ", username, username, osname);
        fgets(line, MAX_COMM, stdin);
        if (funcBreakLine(line, pcs) == 0) { 
            continue;
        }
        DoCommand(pcs);
    }
}