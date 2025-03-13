#include "types.h"

typedef char command[MAX_COMM];

struct CMD {
    char *name;
    void (*function)(char **);  // Puntero a función
};

char *comandos[] = {
    "whoisme            - Prints the user and system info",
    "pid                - Prints the PID of the process running the shell.",
    "ppid               - Prints the PPID of the process running the shell.",
    "cd                 - Changes the current working directory to 'dir'. If no arguments are provided, it just prints the current directory.",
    "date               - Prints the current date.\n\t\tdate -t      - Prints the current time in hh:mm:ss format\n\t\tdate -d      - Prints the current date in DD/MM/YYYY format",
    "historic           - Prints the history of commands executed by the shell.\n\t\thistoric N      - Prints command number N\n\t\thistoric -N      - Prints the last N commands",
    "open [file] mode   - Opens a file and adds it to the list of open files. Available modes are:\n\t\tcr      - Creates a file if it does not exist\n\t\tap      - Opens a file to append data\n\t\tex      - Creates a file exclusively\n\t\tro      - Opens in read-only mode\n\t\trw      - Opens in read-write mode\n\t\two      - Opens in write-only mode\n\t\ttr      - Truncates the file to zero size upon opening",
    "close [df]         - Closes the file descriptor 'df'.",
    "dup [df]           - Duplicates the file descriptor 'df'.",
    "help [cmd]         - Displays a list of available commands or usage of a specific one.",
    "quit/exit/bye      - Exits the program.",
    "mkdir              - Creates a directory.",
    "mkfile             - Creates a file.",
    "listfile           - Prints the contents of a file.",
    "listdir            - Prints the contents of a directory.",
    "reclist            - Recursively prints the contents of a file.",
    "revlist            - Recursively prints the contents of a file in reverse order.",
    "erase              - Deletes a file or directory.",
    "delrec             - Recursively deletes a file or directory.",
    "allocate           - Allocates memory space using malloc, shmem, or mmap.",
    "deallocate         - Frees memory space used with malloc, shmem, mmap, or delkey.",
    "memfill            - Fills a memory space with content.",
    "memdump            - Dumps memory content.",
    "read               - Reads from a memory space.",
    "write              - Writes to a memory space.",
    "readfile           - Reads a file.",
    "writefile          - Writes to a file.",
    "recurse            - Recursively processes memory space.",
    "clear              - Cleans the terminal.",
    "getuid             - Gets the process credentials (real & effective).",
    "setuid             - Sets, if permitted, the effective process credential.",
    "showvar            - Shows the value and address of environment variables.",
    "changevar          - Changes, if permitted, the value of environment variables.",
    "subsvar            - Swaps environment variable var1 for var2 and vice versa.",
    "environ            - Shows the process environment.",
    "fork               - Creates a child process.",
    "search             - Shows or modifies the search list.\n\t\tsearch -add dir:  Adds a directory to the search list\n\t\tsearch -del dir:  Deletes a directory from the search list\n\t\tsearch -clear:    Clears the search list\n\t\tsearch -path:     Imports directories from PATH to the search list",
    "exec [progspec]    - Executes the program specified in [progspec] without creating a new process.",
    "exec [p][prog]     - Executes the program specified in [prog] without creating a new process, with priority set by [p].",
    "fg [prog]          - Creates a process with the program [prog] that executes in the foreground.",
    "fgpri [p] [prog]   - Creates a process with the program [prog] that executes in the foreground with priority set by [p].",
    "back [prog]        - Creates a process with the program [prog] that executes in the background.",
    "back [p][prog]     - Creates a process with the program [prog] that executes in the background with priority set by [p].",
    "listjobs           - Lists background processes.",
    "deljobs [-t][-sig] - Deletes background processes.",
    "*****              - For anything that is not a shell command, the shell assumes an external program.",

    NULL
};


void Cmd_pid(char *tr[]);
void Cmd_ppid(char *tr[]);
void Cmd_authors(char *tr[]);
void Cmd_chdir(char *tr[]);
void Cmd_exit(char *tr[]);
void Cmd_date(char *tr[]);
void Cmd_history(char *tr[]);
void Cmd_help(char *tr[]);
void Cmd_infosys(char *tr[]);
void Cmd_open(char *tr[]);
void Cmd_dup(char *tr[]);
void Cmd_close(char *tr[]);
void Cmd_cwd(char *tr[]);
void Cmd_mkdir(char *tr[]);
void Cmd_mkfile (char *tr[]);
void Cmd_listfile(char *tr[]);
void Cmd_listdir(char *tr[]);
void Cmd_reclist(char *tr[]);
void Cmd_revlist(char *tr[]);
void Cmd_erase(char *tr[]);
void Cmd_delrec(char *tr[]);
void Cmd_allocate(char *tr[]);
void Cmd_deallocate(char *tr[]);
void Cmd_memfill(char *tr[]);
void Cmd_memdump(char *tr[]);
void Cmd_readfile(char *tr[]);
void Cmd_writefile(char *tr[]);
void Cmd_recurse(char *tr[]);
void Cmd_write(char *tr[]);
void Cmd_recurse(char *tr[]);
void Cmd_getuid(char *tr[]);
void Cmd_setuid(char *tr[]);
void Cmd_fork(char *tr[]);
void Cmd_search(char *tr[]);
void Cmd_exec(char *tr[]);
void Cmd_execpri(char *tr[]);
void Cmd_fg(char *tr[]);
void Cmd_fgpri(char *tr[]);
void Cmd_back(char *tr[]);
void Cmd_backpri(char *tr[]);
void Cmd_listjobs();
void Cmd_deljobs(char *tr[]);
void clearLine(int len);
void DoCommand(char *tr[]);
int funcBreakLine(char *line, char *pz[]);

