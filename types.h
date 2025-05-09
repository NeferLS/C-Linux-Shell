#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdint.h>
#include <ctype.h>
#include <sys/wait.h>
#include <limits.h>
#include <sys/resource.h>
#include <termios.h>

#define MAX_COMM 2040 
#define MAX_SIZE 2040 
#define MAX_OPEN_FILES 10 //can change

#if defined(_WIN32)
    #define PLATFORM_NAME "Windows" // Windows
#elif defined(_WIN64)
    #define PLATFORM_NAME "Windows" // Windows
#elif defined(__CYGWIN__) && !defined(_WIN32)
    #define PLATFORM_NAME "Windows" // Windows (Cygwin POSIX under Microsoft Window)
#elif defined(__ANDROID__)
    #define PLATFORM_NAME "Android??" // Android (implies Linux, so it must come first)
#elif defined(__linux__)
    #define PLATFORM_NAME "Linux" // Debian, Ubuntu, Gentoo, Fedora, openSUSE, RedHat, Centos and other
#elif defined(__unix__) || !defined(__APPLE__) && defined(__MACH__)
    #include <sys/param.h>
    #if defined(BSD)
        #define PLATFORM_NAME "BSD" // FreeBSD, NetBSD, OpenBSD, DragonFly BSD
    #endif
#elif defined(__hpux)
    #define PLATFORM_NAME "HP-UX" // HP-UX
#elif defined(_AIX)
    #define PLATFORM_NAME "AIX" // IBM AIX
#elif defined(__APPLE__) && defined(__MACH__) // Apple OSX and iOS (Darwin)
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR == 1
        #define PLATFORM_NAME " IOS" // Apple iOS
    #elif TARGET_OS_IPHONE == 1
        #define PLATFORM_NAME "IOS" // Apple iOS
    #elif TARGET_OS_MAC == 1
        #define PLATFORM_NAME "MACOSX" // Apple OSX
    #endif
#elif defined(__sun) && defined(__SVR4)
    #define PLATFORM_NAME "SOLARIS" // Oracle Solaris, Open Indiana
#else
    #define PLATFORM_NAME NULL
#endif