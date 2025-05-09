#include "globals.h"


// Variables globales no inicializadas
int global_uninit_var1;
int global_uninit_var2;
int global_uninit_var3;

// Variables estáticas no inicializadas 
static int static_uninit_var1;
static int static_uninit_var2;
static int static_uninit_var3;


const char* getUsername(){
    uid_t uid = getuid();
    struct passwd* pw = getpwuid(uid);
    if (pw){
        return pw->pw_name;
    }
    return "??";
}

const char* getOsName(){
    return (PLATFORM_NAME == NULL) ? "" : PLATFORM_NAME;
}

//checks type of process
void typeOfProcess(char *tr[], char *directories[], int *dirCount, int *showHidden, int *longFormat, int *showAccessTime, int *showLink){
    int i = 0;
    for (; tr[i] != NULL; i++) {
        if (tr[i][0] == '-') {
            if (strcmp(tr[i], "-h") == 0) { if (showHidden != NULL) *showHidden = 1;
            } else if (strcmp(tr[i], "-l") == 0) { if (longFormat != NULL) *longFormat = 1;
            } else if (strcmp(tr[i], "-a") == 0) { if (showAccessTime != NULL) *showAccessTime = 1;
            } else if (strcmp(tr[i], "-ln") == 0) { if (showLink != NULL) *showLink = 1;
            } else {
                return;
            }
        } else {
            if (directories != NULL && dirCount != NULL && *dirCount < 100) { 
                directories[*dirCount] = tr[i];
                (*dirCount)++;
            }
        }
    }
    if (directories != NULL && dirCount != NULL && *dirCount == 0) {
        directories[0] = ".";
        *dirCount = 1;
    }
}

void processAndGetDirectory(char *tr[], char *directories[], int *dirCount, int *showHidden, int *longFormat, int *showAccessTime, int *showLink) {
    if (dirCount != NULL) *dirCount = 0;
    if (showHidden != NULL) *showHidden = 0;
    if (longFormat != NULL) *longFormat = 0;
    if (showAccessTime != NULL) *showAccessTime = 0;
    if (showLink != NULL) *showLink = 0;
    typeOfProcess(tr, directories, dirCount, showHidden, longFormat, showAccessTime, showLink);
}

void getAcc(struct stat *st, int acc){
    if (acc) {
        char timebuf[80];
        strftime(timebuf, sizeof(timebuf), "%Y/%m/%d-%H:%M", localtime(&st->st_atime));
        printf("%s\t", timebuf);
    }
    printf("%ld ", (long)st->st_nlink);
    printf("(%ld)\t", (long)st->st_ino);
}

void getLong(struct stat *st, int lng){
    if (lng) {
        printf("%c%c%c%c%c%c%c%c%c%c\t", 
        (S_ISDIR(st->st_mode)) ? 'd' : '-', 
        (st->st_mode & S_IRUSR) ? 'r' : '-', 
        (st->st_mode & S_IWUSR) ? 'w' : '-', 
        (st->st_mode & S_IXUSR) ? 'x' : '-', 
        (st->st_mode & S_IRGRP) ? 'r' : '-', 
        (st->st_mode & S_IWGRP) ? 'w' : '-', 
        (st->st_mode & S_IXGRP) ? 'x' : '-', 
        (st->st_mode & S_IROTH) ? 'r' : '-', 
        (st->st_mode & S_IWOTH) ? 'w' : '-', 
        (st->st_mode & S_IXOTH) ? 'x' : '-'
        );
    }
}

void getLink(struct dirent *file, struct stat *st, int lnk){
    if (lnk && S_ISLNK(st->st_mode)) {
        char link_target[512];
        ssize_t len = readlink(file->d_name, link_target, sizeof(link_target) - 1);
        if (len != -1) { 
            link_target[len] = '\0'; 
            printf(" -> %s\n", link_target); 
        }
    }
}

void getOwner(struct stat *st){
    struct passwd *pwd = getpwuid(st->st_uid);
    printf("%s\t", pwd ? pwd->pw_name : "???");
}

void getGroup(struct stat *st){
    struct group *grp = getgrgid(st->st_gid);
    printf("%s\t", grp ? grp->gr_name : "??????");
}

void getBlockSize(struct stat *st){
    printf("%8lld ", (long long)st->st_size);
}

void aux_printFileInfo(struct dirent *file, struct stat *st, int lng, int acc, int lnk){
    getAcc(st, acc);
    getOwner(st);
    getGroup(st);
    getLong(st, lng);
    getBlockSize(st);
    getLink(file, st, lnk);
    printf("\n");
}

void aux_listFilesInDir(const char *DIRname, int lng, int acc, int lnk){
    DIR *cdir = opendir(DIRname);
    struct dirent *cfile;
    struct stat cstat;
    char buffer[512];

    if (cdir == NULL) {
        fprintf(stderr, "Couldn't open dir.");
        return;
    }
    while ((cfile = readdir(cdir)) != NULL) {
        snprintf(buffer, sizeof(buffer), "%s/%s", DIRname, cfile->d_name);
        if (lstat(buffer, &cstat) == 0) {
            aux_printFileInfo(cfile, &cstat, lng, acc, lnk);
        }
    }
    closedir(cdir);
}

void aux_printDir(const char *dir, struct dirent *entry, int hid, int lng, int acc, int lnk) {
    if (!hid && entry->d_name[0] == '.') {
        return;
    }
    // printing
    if (!lng) {
        printf("%s\n", entry->d_name);
    } else {
        struct stat st;
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, entry->d_name);
        if (stat(fullpath, &st) == -1) {
            perror("stat");
            return;
        }
        char timebuf[80];
        struct tm *tm_info;
        
        // acc
        if (acc) {
            tm_info = localtime(&st.st_atime);
        } else {
            tm_info = localtime(&st.st_mtime);
        }
        strftime(timebuf, sizeof(timebuf), "%Y/%m/%d %H:%M:%S", tm_info);

        // name, group
        getOwner(&st);
        getGroup(&st);
        // nlink
        printf("%s   %u (%8ld)", timebuf, (unsigned)st.st_nlink, (long)st.st_ino);
        //long
        getLong(&st, lng);
        //size
        getBlockSize(&st);
        // na,e
        printf(" %s", entry->d_name);
        // link
        getLink(entry, &st, lnk);
        printf("\n");
    }
}

void recList(const char *dirPath, int hid, int lng, int acc, int lnk,
            DIR *auxdir, struct dirent *entry){
    
    printf("************%s\n", dirPath);
    // inside dir
    while (entry != NULL) {
        char size[1024];
        snprintf(size, sizeof(size), "%s/%s", dirPath, entry->d_name);
        aux_printDir(dirPath, entry, hid, lng, acc, lnk);
        // recursive
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            recList(size, hid, lng, acc, lnk, auxdir, entry);
        }
    }
}

void revList(const char *dirPath, int hid, int lng, int acc, int lnk,
            DIR *auxdir, struct dirent *entry){
    rewinddir(auxdir);
        printf("************%s\n", dirPath);
        while (entry != NULL) {
            revList(dirPath, hid, lng, acc, lnk, auxdir, entry);
        }
}


void aux_listDir(const char *dirPath, int hid, int lng, int acc, int lnk, int rev) {
    DIR *auxdir;
    struct dirent *entry;

    if ((auxdir = opendir(dirPath)) == NULL) {  
        perror("opendir");
        return;
    }   
    entry = readdir(auxdir);

    if (!rev){
        recList(dirPath, hid, lng, acc, lnk, auxdir, entry);
    } else
        revList(dirPath, hid, lng, acc, lnk, auxdir, entry);

    closedir(auxdir);
}

void removeArchive(char *tr[],struct stat *st){
    for(int i=0; tr[i]!=NULL;i++){
        if(stat(tr[i], st)==-1){
            perror(tr[i]);
            continue;
        }
        if(unlink(tr[i])==0){
            printf("File with nlink %s successfully deleted\n",tr[i]);
        }else{
            perror("Couldn't delete file.");
        }
    }
}

void removeDir(char *tr[], struct stat *st){
    for(int i=0; tr[i]!=NULL;i++){
        if(stat(tr[i], st)==-1){
            perror(tr[i]);
            continue;
        }
        if(rmdir(tr[i])==0){
            printf("Dir %s successfully deleted.\n",tr[i]);
        }else{
            perror("Couldn't delete dir.");
        }
    }
}

void aux_erase(char *tr[]){
    struct stat st;   
    
    for (int i = 0; tr[i] != NULL; i++) {
        if (stat(tr[i], &st) == -1) { 
            perror(tr[i]);
            continue; 
        }
    }
    if((S_ISREG(st.st_mode))){
        removeArchive(tr, &st);
    }

    if((S_ISDIR(st.st_mode))){
        removeDir(tr, &st);
    }
}

bool isDir(const char *dirPath) {
    struct stat file_stat;
    return (stat(dirPath, &file_stat) == 0 && S_ISDIR(file_stat.st_mode));
}

int aux_delrec(const char *dirPath) {
    if (!isDir(dirPath)) {
        return unlink(dirPath);
    }
    DIR *directorio = opendir(dirPath);
    struct dirent *entry;
    char fullPath[1024];

    while ((entry = readdir(directorio)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);
        aux_delrec(fullPath);
    }

    closedir(directorio);
    return rmdir(dirPath);
}
void aux_allocate_malloc(size_t size) {
    void *mem = malloc(size);
    if (mem == NULL) {
        perror("Error creating a memory address.");
        return;
    }
    printf("Asigned %zu bytes in %p address\n", size, mem);
    
    MemoryBlock block = {
        .address = mem,
        .size = size,
        .alloc_time = time(NULL),
        .type = MALLOC
    };
    addMemBlock(&MEMLIST, block);
}

void *mapearFichero (char *archivo, int protection)
{
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;

    if (protection & PROT_WRITE)
          modo=O_RDWR;
    if (stat(archivo,&s)==-1 || (df=open(archivo, modo))==-1)
          return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
           return NULL;
    return p;
}

int tipoApertura(char *tipo_apertura, int protection){
    if (strchr(tipo_apertura, 'r') != NULL) protection |= PROT_READ;
    if (strchr(tipo_apertura, 'w') != NULL) protection |= PROT_WRITE;
    if (strchr(tipo_apertura, 'x') != NULL) protection |= PROT_EXEC;
    return protection;
}

size_t sizeBlock(char *archivo, struct stat s){
    if (stat(archivo, &s) == -1) {
        perror("Error al obtener información del archivo");
        return -1;
    }
    return s.st_size;
}

void aux_allocate_nmap(char *archivo, char *tipo_apertura) {
    int protection = 0;
    size_t tam;
    struct stat s;
    void *dir;

    tipoApertura(tipo_apertura, protection);
    memset(&s, 0, sizeof(struct stat));
    tam = sizeBlock(archivo, s);

    int modo = (protection & PROT_WRITE) ? O_RDWR : O_RDONLY;
    int fd = open(archivo, modo);
    if (fd == -1) {
        perror("Couldn't open the file");
        return;
    }
    dir = mapearFichero(archivo, protection);

    printf("Filed %s mapped in %p\n", archivo, dir);

    MemoryBlock block = {
        .address = dir,
        .size = tam,
        .alloc_time = time(NULL),
        .type = MMAP
    };
    strncpy(block.extra_info.mmap_info.filename, archivo, sizeof(block.extra_info.mmap_info.filename) - 1);
    block.extra_info.mmap_info.filename[sizeof(block.extra_info.mmap_info.filename) - 1] = '\0';
    block.extra_info.mmap_info.fd = fd;
    addMemBlock(&MEMLIST, block);
}

void aux_allocate_shared(int key, size_t size) {
    if (findSharedBlock(&MEMLIST, key)) {
        printf("Error: A shared memory block with key %d is already in the list.\n", key);
        return;
    }

    int shared_mem = shmget(key, size, 0666);
    if (shared_mem == -1) {
        perror("Error attaching to shared memory");
        return;
    }

    void *mem = shmat(shared_mem, NULL, 0);
    if (mem == (void*)-1) {
        perror("Error attaching to shared memory");
        return;
    }

    MemoryBlock block = {
        .address = mem,
        .size = size,
        .alloc_time = time(NULL),
        .type = SHMEM,
        .extra_info.key = key
    };
    addMemBlock(&MEMLIST, block);

    printf("Shared memory attached: key %d, size %zu bytes, at address %p\n", key, size, mem);
}

void aux_allocate_createshared(int key, size_t size) {
    int existing_mem = shmget(key, 0, 0666);
    if (existing_mem != -1) {
        printf("Error: Shared memory with key %d already exists.\n", key);
        return;
    }

    int shared_mem = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
    if (shared_mem == -1) {
        perror("Error creating shared memory");
        return;
    }

    void *mem = shmat(shared_mem, NULL, 0);
    if (mem == (void*)-1) {
        perror("Error attaching to shared memory");
        return;
    }

    MemoryBlock block = {
        .address = mem,
        .size = size,
        .alloc_time = time(NULL),
        .type = SHMEM,
        .extra_info.key = key
    };
    addMemBlock(&MEMLIST, block);

    printf("Shared memory created: key %d, size %zu bytes, at address %p\n", key, size, mem);
}




void aux_deallocate_malloc(size_t size){
    Node *current = MEMLIST.h;
    Node *prev = NULL;
    removeMALLOC(current, size, prev, &MEMLIST);
}

void aux_deallocate_mmap(const char *archivo){
    Node *current = MEMLIST.h;
    Node *prev = NULL;
    removeMMAP(current, archivo, prev, &MEMLIST);
}


void aux_deallocate_shared(int clave){
    Node *current = MEMLIST.h;
    Node *prev = NULL;
    removeSHMEM(current, clave, prev, &MEMLIST);
}

void aux_deallocate_delkey(int clave){
    int shmid = shmget(clave, 0, 0666);
    if(shmid == -1) {
        perror("Error al obtener el identificador de memoria compartida");
        return;
    }
    
    if(shmctl(shmid, IPC_RMID, NULL) == -1){
        perror("Error al eliminar el bloque de memoria compartida del sistema");
    } else
        printf("Bloque de memoria compartida con clave %d eliminado del sistema\n", clave);
}

void aux_deallocate_addr(memList *L, void *addr) {
    Node *current = L->h;
    Node *prev = NULL;
    removeADDR(current, addr, prev, &MEMLIST);
}

void print_memory_line(unsigned char *ptr, int start, int max) {
    //dec
    for (int j = 0; j < 25; j++) {
        if (start + j < max) {
            unsigned char c = ptr[start + j];
            printf("%2c ", isprint(c) ? c : '.');
        } else {
            printf("   ");
        }
    }
    printf("\n");
    //hex
    for (int j = 0; j < 25; j++) {
        if (start + j < max) {
            printf("%02x ", ptr[start + j]);
        } else {
            printf("   ");
        }
    }
    printf("\n\n");
}

void dump_memory(void *addr, size_t cont) {
    printf("Volcando memoria desde la direccion 0x%p (%zu bytes):\n", addr, cont);
    unsigned char *ptr = (unsigned char *)addr;
    int max = (int)cont;
    for (int i = 0; i < max; i += 25) {
        print_memory_line(ptr, i, max);
    }
}

int validate_input(void *addr, size_t cont) {
    if (addr == NULL) {
        fprintf(stderr, "Error: direccion de memoria invalida.\n");
        return 0;
    }
    if (cont <= 0) {
        fprintf(stderr, "Error: cont debe ser mayor a 0.\n");
        return 0;
    }
    return 1;
}

unsigned char parse_char(char *str) {
    if (str[0] == '\'' && str[2] == '\'') {
        return (unsigned char)str[1];
    } else {
        fprintf(stderr, "Formato incorrecto para el caracter %s\n", str);
        return 0;
    }
}

void fill_memory(void *addr, size_t cont, unsigned char ch) {
    if (mlock(addr, cont) == -1) {
        fprintf(stderr, "Error al bloquear la memoria: %s\n", strerror(errno));
        return;
    }
    memset(addr, ch, cont);
    munlock(addr, cont);
    printf("Memoria rellenada en %p con %zu bytes del caracter '%c' (0x%02x).\n", addr, cont, ch, ch);
}

void aux_mem(char *tr[], bool dump) {
    void *addr = (void *)strtoull(tr[0], NULL, 16);
    size_t cont = strtoull(tr[1], NULL, 10);
    
    if (!validate_input(addr, cont)) {
        return;
    }
    
    if (dump) {
        dump_memory(addr, cont);
    } else {
        unsigned char ch = parse_char(tr[2]);
        if (ch != 0) {
            fill_memory(addr, cont, ch);
        }
    }
}

void aux_memory_vars() {
    // Variables locales de la funcions y estaticas
    int local_var1 = 1;
    int local_var2 = 2;
    int local_var3 = 3;
    static int static_init_var1 = 100;
    static int static_init_var2 = 200;
    static int static_init_var3 = 300;

    printf("Variables locales          %p,       %p,       %p\n", 
           (void *)&local_var1, (void *)&local_var2, (void *)&local_var3);
    printf("Variables globales         %p,       %p,       %p\n", 
           &memFile, (void *)&MEMLIST, (void *)&HIS);
    printf("Var (N.I.)globales         %p,       %p,       %p\n", 
           (void *)&global_uninit_var1, (void *)&global_uninit_var2, (void *)&global_uninit_var3);
    printf("Variables estaticas         %p,       %p,       %p\n", 
           (void *)&static_init_var1, (void *)&static_init_var2, (void *)&static_init_var3);
    printf("Var (N.I.)estaticas         %p,       %p,       %p\n", 
           (void *)&static_uninit_var1, (void *)&static_uninit_var2, (void *)&static_uninit_var3);
}

void printMemoryBocks(Node *current){
    printf("*Address\t\t\t *Size\t\t\t *Time\n");
    while (current) {
        printf("%p\t\t\t %zu\t\t\t %s", 
               current->block.address, current->block.size, ctime(&current->block.alloc_time));
        switch(current->block.type) {
            case MALLOC:
                printf("MALLOC");
                break;
            case MMAP:
                printf("MMAP, Archivo: %s, FD: %d", 
                       current->block.extra_info.mmap_info.filename,
                       current->block.extra_info.mmap_info.fd);
                break;
            case SHMEM:
                printf("SHMEM, Key: %d", current->block.extra_info.key);
                break;
        }
        
        printf("\n");
        current = current->next;
    }
}

void aux_memory_blocks(memList *MEMLIST) {
    Node *current = MEMLIST->h;

    printf("******Memory addresses asigned to process %d\n", getpid());
    if (!current) {
        printf("Nothing to see here...\n");
        return;
    }
    printMemoryBlocks(MEMLIST);
}

void aux_memory_funcs() {
    printf("Funciones programa         %p,       %p,       %p\n", 
           (void *)aux_memory_vars, (void *)aux_memory_blocks, (void *)aux_memory_funcs);
    printf("Funciones libreria         %p,       %p,       %p\n", 
           (void *)printf, (void *)malloc, (void *)free);
}

void aux_memory_all() {
    aux_memory_vars();
    aux_memory_funcs();
    aux_memory_blocks(&MEMLIST);
}

void aux_memory_pmap() {
    char command[128];
    printf("Salida del comando pmap:\n");
    snprintf(command, sizeof(command), "pmap %d", getpid());
    system(command);
}

void aux_readFile(char *tr[]){
    char *fileName=tr[0];

    void *addr = (void *)strtoul(tr[1],NULL,16);
    if(addr==NULL){
        fprintf(stderr, "Error: dirección de memoria '%s' inválida.\n", tr[1]);
        return;
    }

    char *endPtr;
    size_t cont= (size_t)strtoul(tr[2],&endPtr, 10);
    if(*endPtr!='\0'||cont<=0){
        fprintf(stderr, "Error: número de bytes '%s' inválido.\n", tr[2]);
        return;
    }

    int fd=open(fileName, O_RDONLY);
    if(fd==-1){
        perror("Error al abrir el archivo");
        return;
    }

    size_t bytesRead=read(fd, addr, cont);
    if(bytesRead==-1){
        perror("Error durante la lectura del archivo");
        return;
    }

    if((size_t)bytesRead<cont){
        fprintf(stderr, "Advertencia: Se leyeron menos bytes de los solicitados (%zd de %zu).\n", bytesRead, cont);
    } else {
        printf("Leídos %zd bytes desde el descriptor %d a la dirección %p.\n", bytesRead, fd, addr);
    }

    close(fd);
}

void aux_read(char *tr[]){
    char *endPtr;
    int fd=(int)strtol(tr[0], &endPtr,10);
    if(*endPtr!='\0'||fd<0){
        fprintf(stderr, "Error: descriptor de archivo '%s' inválido.\n", tr[0]);
        return;
    }
    void *addr = (void *)strtoul(tr[1],NULL,16);
    if(addr==NULL){
        fprintf(stderr, "Error: dirección de memoria '%s' inválida.\n", tr[1]);
        return;
    }
    size_t cont= (size_t)strtoul(tr[2],&endPtr, 10);
    if(*endPtr!='\0'||cont<=0){
        fprintf(stderr, "Error: número de bytes '%s' inválido.\n", tr[2]);
        return;
    }

    size_t bytesRead=read(fd, addr, cont);
    if(bytesRead==-1){
        perror("Error durante la lectura del archivo");
        return;
    }

    if((size_t)bytesRead<cont){
        fprintf(stderr, "Advertencia: Se leyeron menos bytes de los solicitados (%zd de %zu).\n", bytesRead, cont);
    } else {
        printf("Leídos %zd bytes desde el descriptor %d a la dirección %p.\n", bytesRead, fd, addr);
    }
}

void aux_writefile(const char *file, void *addr, size_t cont, int overwrite){
    const char *mode=overwrite? "wb":"ab";
    FILE *fp = fopen(file, mode);
    
    if (fp == NULL){
        perror("Error al abrir el archivo para escritura");
        return;
    }

    size_t bytesWritten = fwrite(addr, 1, cont, fp);
    if(bytesWritten < cont){   
        if(ferror(fp)){
            perror("Error durante la escritura del archivo");
        }else{
            fprintf(stderr, "Advertencia: Se escribieron menos bytes de los solicitados\n");
        }
    }else{
        printf("Escritos %zu bytes desde la dirección %p al archivo '%s'.\n", bytesWritten, addr, file); 
    }
    fclose(fp);
}

void aux_write(char *tr[]){
    int overWrite=0;
    int argIndex=0;

    if(strcmp(tr[0],"-o")==0){
        overWrite=1;
        argIndex=1;
    }

    const char *fileName=tr[argIndex];
    if(fileName==NULL){
        fprintf(stderr, "Error: nombre de archivo inválido.\n");
        return;
    }

    void *addr=(void *)strtoul(tr[1+argIndex],NULL,16);
    if(addr==NULL){
        fprintf(stderr, "Error: dirección de memoria '%s' inválida.\n", tr[argIndex + 1]);
        return;
    }

    char *endPtr;
    size_t cont=(size_t)strtoul(tr[2+argIndex],&endPtr,10);
    if(*endPtr!='\0' || cont<=0){
        fprintf(stderr, "Error: el numero de bytes '%s' invalido\n",tr[2+argIndex]);
        return;
    }

    int openMode= O_WRONLY|O_CREAT;
    if(overWrite){
        openMode |=O_TRUNC;
    }else{
        openMode |=O_APPEND;
    }

    int fd=open(fileName, openMode, 0644);
    if(fd==-1){
        perror("Error al abrir el archivo");
        return;
    }

    ssize_t bytesWritten= write(fd, addr, cont);

    if((size_t)bytesWritten<cont){
        fprintf(stderr, "Advertencia: solo se escribieron %zd de %zu bytes.\n", bytesWritten, cont);
    }else{
        printf("Escritos %zd bytes desde la dirección %p al archivo '%s'.\n", bytesWritten, addr, fileName);
    }

    close(fd);
}

void aux_recurse(int n,int arr_static){
    printf("parametro: %5d(0x%lx) array 0x%lx, arr estatico 0x%lx\n", 
            n, (unsigned long)&n, (unsigned long)&arr_static, (unsigned long)arr_static);
    if(n>0){
        aux_recurse(n-1,arr_static);
    }
}


//p3

int validateLogin(char *tr[], int login_mode){
    int uid_index = login_mode ? 1 : 0;
    if (tr[uid_index] == NULL) {
        fprintf(stderr, "error: debes proporcionar un UID válido.\n");
        return -1;
    }
    return uid_index;
}

void isLoginCoolorNah(int login_mode, uid_t target_uid){
    if (login_mode) {
        printf("Simulando acciones de login para el UID %d...\n", target_uid);
        char home_dir[256];
        snprintf(home_dir, sizeof(home_dir), "/home/%d", target_uid);
        if (chdir(home_dir) == 0) {
            printf("Directorio de trabajo cambiado a %s\n", home_dir);
        } else {
            printf("No se pudo cambiar al directorio de trabajo %s\n", home_dir);
        }
    }
}

void aux_setuid(char *tr[], int login_mode) {
    int uid_index = validateLogin(tr, login_mode);
    if (uid_index == -1) return;

    char *endptr;
    long target_uid_long = strtol(tr[uid_index], &endptr, 10);
    
    if (*endptr != '\0' || target_uid_long < 0 || target_uid_long > UINT_MAX) {
        fprintf(stderr, "error: UID inválido.\n");
        return;
    }

    uid_t target_uid = (uid_t)target_uid_long;

    if (setuid(target_uid) != 0) {
        perror("Error al cambiar el UID efectivo");
        return;
    }

    printf("UID efectivo cambiado a %d.\n", geteuid());
    isLoginCoolorNah(login_mode, target_uid);
}

void aux_showvar(char *var_name) {
    char *value = getenv(var_name);
    if (value != NULL) {
        printf("%s = %s [%p]\n", var_name, value, (void*)value);
    } else {
        printf("%s no está definida\n", var_name);
    }
}

int aux_changevar(char *var, char *val, int opt) {
    switch (opt) {
        case 0:
            return setenv(var, val, 1);
        case 1:
            {
                char *env_string = malloc(strlen(var) + strlen(val) + 2);
                if (env_string == NULL) {
                    perror("Error de asignación de memoria");
                    return -1;
                }
                sprintf(env_string, "%s=%s", var, val);
                return putenv(env_string);
            }
        default:
            return -1;
    }
}

int aux_subsvar(char *var1, char *var2, char *val) {
    if (setenv(var2, val, 1) != 0) {
        return -1;
    }
    return unsetenv(var1);
}

void aux_environ_print() {
    extern char **environ;
    int i = 0;
    while (environ[i] != NULL) {
        printf("%p -> %s\n", (void *)&environ[i], environ[i]);
        i++;
    }
}

void aux_environ_addr(int type) {
    extern char **environ;
    if (type == 0) {
        printf("environ: %p\n", (void *)environ);
    } else {
        printf("environ: %p\n", (void *)&environ);
    }
}

int aux_configurarEntorno(char *tr[], int *envCount){
    char *var, *val;
    int opt=0;
    int i=0;

    while (tr[i]!=NULL && getenv(tr[0])!=NULL){
        if(strcmp(tr[i], "-a")==0 || strcmp(tr[i], "-e")==0){
            var = tr[i + 1];
            val = tr[i + 2];
            // Aquí podemos reutilizar la función changevar si es necesario
            if (strcmp(tr[0], "-a") == 0 || strcmp(tr[0], "-e") == 0) {
                aux_changevar(var, val, opt);
            }
        }else {
            // Si tenemos más variables en tr[], las agregamos al entorno
            envCount++;
            if (getenv(var) != NULL) {
                var = getenv(tr[i]);
            }
        }
    }
    return 0;
}

int back(char *tr[]) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Error al crear el proceso");
        return -1;
    }

    if (pid == 0) {
        if (execvp(tr[0], tr) == -1) {
            perror("Error al ejecutar el programa");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Proceso %d ejecutándose en segundo plano\n", pid);

        return pid;
    }

    return 0;
}

int backpri(int prio, char *tr[]) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Error al crear el proceso");
        return -1;
    }

    if (pid == 0) {
        if (setpriority(PRIO_PROCESS, getpid(), prio) == -1) {
            perror("Error al cambiar la prioridad");
            exit(EXIT_FAILURE);
        }

        if (execvp(tr[0], tr) == -1) {
            perror("Error al ejecutar el programa");
            exit(EXIT_FAILURE);
        }
    } else {
        printf("Proceso %d ejecutándose en segundo plano con prioridad %d\n", pid, prio);

        return pid;
    }

    return 0;
}

void aux_execProgram(char *program, char *args[], char *newEnv[]){
    if(newEnv==NULL){
        if(execvp(program, args)==-1){
            fprintf(stderr, "Error al ejecutar el programa '%s': %s\n", program, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }else{
        if (execve(program, args, newEnv) == -1) {
            fprintf(stderr, "Error al ejecutar el programa con nuevo entorno: %s\n", strerror(errno));
        }
        for (int i = 0; newEnv[i] != NULL; i++) {
            free(newEnv[i]);    //liberar memoria
        }
        free(newEnv);
    }
    exit(EXIT_FAILURE); //falla algo del proceso hijo
}

void aux_waitForChild(pid_t pid){
    int status;
    waitpid(pid, &status, 0);
    if(WIFEXITED(status)){
        printf("El programa terminó con código %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("El programa fue terminado por la señal %d\n", WTERMSIG(status));
    }
}

int aux_setChildPriority(int priority) {
    if (setpriority(PRIO_PROCESS, 0, priority) == -1) {
        fprintf(stderr, "Error al cambiar la prioridad: %s\n", strerror(errno));
        return -1; // error
    }
    return 0; // exito
}

void aux_isPrioOkay(char *endptr, int prio){
    if (*endptr != '\0') {
        fprintf(stderr, "Error: Priority can only be an integer value.\n");
        return;
    }

    if (prio < -20 || prio >19){
        fprintf(stderr, "Error: Priority can only be between [-20,19].\n");
        return;
    }    
}

void aux_IfcreateEnv(char* tr[],char** argv, char*** newEnv, int envCnt){
    for (int i = 1; tr[i] != NULL; i++) {
        if (strchr(tr[i], '=') == NULL && getenv(tr[i]) != NULL) {
            envCnt++;
        } else {
            *argv = tr[i];
            break;
        }
    }

    if (*argv == NULL) {
        fprintf(stderr, "Error: A process to run has not been specified.\n");
        return;
    }

    // create env if necessary 
    if (envCnt > 0) {
        *newEnv = malloc((envCnt + 1) * sizeof(char *));
        if (*newEnv == NULL) {
            perror("Couldn't assign memory to the process\n");
            return;
        }
        int index = 0;
        for (int i = 1; tr[i] != NULL && index < envCnt; i++) {
            if (getenv(tr[i]) != NULL)
                (*newEnv)[index++] = strdup(tr[i]);
        }
        (*newEnv)[index] = NULL;
    }
}