#define _GNU_SOURCE
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include "posix_api.h"
#include <stdarg.h>
#include "log.h"

#define DEBUG
#define NS_IN_SEC (1000000000L)
#define US_IN_SEC (1000000L)
#define FILE_SIZE (1000000000)

// set to 1 to intercept calls to specific file
#define ENABLE 1
#define INFO 0
#define FD_SIZE 10000

#define LOCK
#define UNLOCK

#define NOT_IMPLEMENTED(str) { printf("Not implemented %s\n", str); exit(-1); }
ssize_t open_file_copy(char* fname, char* ptr);


pthread_mutex_t mutex;

typedef unsigned long long int ull;

//static void init() __attribute__((constructor));
static void my_exit() __attribute__((destructor));

char is_special_fd[FD_SIZE] = {0};
int special_fd_size[FD_SIZE] = {0};
int fd_counter = 100;

char* file_data[1000];
int file_ptr[1000];

void my_exit() {}
    
unsigned long long int get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec*US_IN_SEC+ts.tv_nsec/1000;
}

char mapping_inited = 0;
void init_mapping() {
    if (mapping_inited)
        return;
    mapping_inited = 1;

    load_posix();

    //pthread_mutexattr_t attr;
    //pthread_mutexattr_init(&attr);
    //pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    //pthread_mutex_init(&mutex, &attr);
}

void print_flags(int flags) {
    LOG(INFO, ("flags (%d): O_APPEND: %d O_ASYNC: %d O_CREAT: %d O_DIRECT: %d O_NONBLOCK: %d O_SYNC: %d O_RDWR: %d O_RDONLY: %d O_WRONLY: %d\n", 
                    flags,
                    flags & O_APPEND, flags & O_ASYNC, flags & O_CREAT, 
                    flags&O_DIRECT, flags&O_NONBLOCK,
                    flags & O_SYNC, flags & O_RDWR, flags&O_RDONLY, flags&O_WRONLY));
}

char first = 0;
int open(const char * pathname, int flags, ...) {
    LOG(INFO, ("open fname: %s\n", pathname));
    print_flags(flags);
    init_mapping();
   
    LOCK;
    if (ENABLE && strcmp("/data/joao/ligra/utils/my_edge_1M", pathname) == 0) {
        // XXX we need to associate this filename to this fd
        int fd = fd_counter++;
        is_special_fd[fd] = 1;
        file_ptr[fd] = 0;
        file_data[fd] = (char*)malloc(FILE_SIZE);
        ssize_t size = open_file_copy("/data/joao/ligra/utils/my_edge_1M", file_data[fd]);
        special_fd_size[fd] = size;
        LOG(INFO, ("special file returning fd: %d\n", fd));
        UNLOCK;
        return fd;
    } else {
        int ret = _open(pathname, flags);
        LOG(INFO, ("regular open: %s. returning fd: %d\n", pathname, ret));
        UNLOCK;
        return ret;

    }
}

int close(int fd) {
    LOCK;

    if (is_special_fd[fd]) {
#ifdef DEBUG
        LOG(INFO, ("blade close fd: %d\n", fd));
#endif
        UNLOCK;
        return 0;
    } else {
#ifdef DEBUG
        LOG(INFO, ("normal close fd: %d\n", fd));
#endif
        UNLOCK;
        return _close(fd);
    }
}

ssize_t write(int fd, const void *buf, size_t count) {
    LOG(INFO, ("write fd: %d\n", fd));
    init_mapping();
    
    LOCK;
    if (is_special_fd[fd]) {
        LOG(INFO, ("blade write fd:%d\n", fd));
        UNLOCK;
        memcpy(file_data[fd] + file_ptr[fd], buf, count);
        file_ptr[fd] += count;
        return count;
    } else {
        UNLOCK;
        return _write(fd, buf, count);
    }
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
    init_mapping();
    if (is_special_fd[fd]) {
#ifdef DEBUG
        LOG(INFO, ("blade pread. fd: %d count: %lu offset: %lu\n", fd, count, offset));
#endif
        memcpy(buf, file_data[fd] + offset, count);
        return count;
    } else {
#ifdef DEBUG
        LOG(INFO, ("normal pread. fd: %d count: %lu offset: %lu\n", fd, count, offset));
#endif
        return _pread(fd, buf, count, offset);
    }
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
    LOG(INFO, ("pwrite"));
    init_mapping();
    if (is_special_fd[fd]) {
        LOG(INFO, ("blade pwrite"));
        memcpy(file_data[fd] + offset, buf, count);
        return count;
    }
    else {
        return _pwrite(fd, buf, count, offset);
    }
}

ssize_t read(int fd, void *buf, size_t count) {
    LOG(INFO, ("read"));
    init_mapping();
    if (is_special_fd[fd]) {
        NOT_IMPLEMENTED("blade read");
        memcpy(buf, file_data[fd] + file_ptr[fd], count);
        file_ptr[fd] += count;
        return count;
    } else {
        return _read(fd, buf, count);
    }
}

int ftruncate(int fd, off_t length) {
    LOG(INFO, ("ftruncate. fd: %d length: %lu\n", fd, length));
    init_mapping();

    if (is_special_fd[fd]) {
#ifdef DEBUG
        LOG(INFO, ("blade ftruncate. fd: %d length: %lu\n", fd, length));
#endif
    exit(-1);
        return 0; // XXX fix
    } else {
#ifdef DEBUG
        LOG(INFO, ("normal ftruncate. fd: %d length: %lu\n", fd, length));
#endif
        return _ftruncate(fd, length);
    }

    return 0;
}

off_t lseek(int fd, off_t offset, int whence) {
    LOG(INFO, ("lseek. fd: %d\n", fd));

    init_mapping();
    if (is_special_fd[fd]) {
        LOG(INFO, ("blade lseek. fd: %d\n", fd));
        exit(-1);
#ifdef DEBUG
#endif
        return 0; // XXX fix
    } else {
#ifdef DEBUG
        LOG(INFO, ("normal lseek. fd: %d\n", fd));
#endif
        return _lseek(fd, offset, whence);
    }
}

// *****************************

ssize_t open_file_copy(char* fname, char* ptr) {
    LOG(INFO, ("Opening file %s\n", fname));
    int fd = _open(fname, 0);
    if (!fd)
        exit(-1);

    ssize_t size = _read(fd, ptr, FILE_SIZE);

    _printf ("Read: %.20s\n", ptr);

    LOG(INFO, ("Opened %s size: %lu fd: %d errno: %d\n", fname, size, fd, errno));

    _close(fd);
    return size;
}

// ***************************************
// FILE* interface
// *************************************
FILE *fopen(const char *path, const char *mode) {
    init_mapping();
#ifdef DEBUG
    LOG(INFO, ("fopen path: %s\n", path));
#endif

    // We open a file just to make sure we return a good fd
    // let's hope we capture all accesses to this fd
    if (ENABLE && strcmp(path, "/data/joao/ligra/utils/my_edge_1M") == 0) {
        LOG(INFO, ("special file: %s mode: %s\n", path, mode));

        ssize_t fd = fd_counter++;
        is_special_fd[fd] = 1;
        file_data[fd] = (char*)malloc(FILE_SIZE);
        file_ptr[fd] = 0;

        ssize_t size = open_file_copy("/data/joao/ligra/utils/my_edge_1M", file_data[fd]);
        special_fd_size[fd] = size;

        LOG(INFO, ("returning fake fd: %d\n", fd));
        return (FILE*)fd;
    } else {
        FILE* ret = _fopen(path, mode);
#ifdef DEBUG
        LOG(INFO, ("normal ret: %llu\n", (unsigned long long int) ret));
        if (ret)
            LOG(INFO, ("normal fileno ret: %d\n", _fileno(ret)));
#endif
        return ret;
    }
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    LOG(INFO, ("fread. size: %lu nmemb: %lu\n", size, nmemb));
    NOT_IMPLEMENTED("fread");
    int fd = _fileno(stream);
    if (is_special_fd[fd]) {
#ifdef DEBUG
        LOG(INFO, ("blade fread. size: %lu nmemb: %lu\n", size, nmemb));
#endif
        return 0; // XXX fix
    } else {
#ifdef DEBUG
        LOG(INFO, ("normal fread. size: %lu nmemb: %lu\n", size, nmemb));
#endif
        return _fread(ptr, size, nmemb, stream);
    }
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb,
                                    FILE *stream) {
#ifdef DEBUG
    //printf("fwrite\n");
#endif
    int fd = _fileno(stream);
    if (is_special_fd[fd]) {
        NOT_IMPLEMENTED("blade fwrite");
        return 0; // XXX fix
    } else {
        return _fwrite(ptr, size, nmemb, stream);
    }
}

int is_special_stream(FILE* stream) {
    ssize_t fd = (ssize_t)stream;
    return fd < FD_SIZE && is_special_fd[fd];
}

char *fgets(char *s, int size, FILE *stream) {
    LOG(INFO, ("Doing fgets size: %d\n", size));
    init_mapping();

    // fgets reads at most size-1
    if (size <= 1)
        return 0;

    if (is_special_stream(stream)) {
        ssize_t fd = (ssize_t)stream;
        LOG(INFO, ("blade fgets file_ptr: %d fd: %d\n", file_ptr[fd], fd));
        int count = 1;

        _printf ("fgets reading string: %.20s\n", file_data[fd] + file_ptr[fd]);

        while (count < size && file_ptr[fd] < special_fd_size[fd] &&
                *(file_data[fd]+file_ptr[fd]) != '\n' &&
                *(file_data[fd]+file_ptr[fd])) {
            *s = *(file_data[fd]+file_ptr[fd]);
            file_ptr[fd]++;
            ++s;
            count++;
        }
        *s = *(file_data[fd]+file_ptr[fd]);
        file_ptr[fd]++;
        LOG(INFO, ("blade fgets count: %d ret: %lu\n", count, s));
        return s; // fgets returns the string passed
    } else {
        char* ret = _fgets(s, size, stream);
        return ret;
    }
}

int ungetc(int c, FILE *stream) {
    init_mapping();
    int fd = _fileno(stream);
    LOG(INFO, ("ungetc. fd: %d\n", fd));
    return _ungetc(c, stream);
}

int fileno( FILE *stream) { 
    LOG(INFO, ("fileno\n"));
    init_mapping();
    return _fileno(stream);
}
int fflush(FILE* stream) { 
    LOG(INFO, ("fflush\n"));
    init_mapping();
    return _fflush(stream);
}

int puts(const char *s) { 
    LOG(INFO, ("puts\n"));
    init_mapping();
    return _puts(s);
}

int printf(const char *format, ...) {
    LOG(INFO, ("printf\n"));
    init_mapping();
    va_list va;
    int ret;

    va_start(va, format);
    ret = _vfprintf(stderr, format, va);
    va_end(va);
    return ret;
}

int vfprintf(FILE *stream, const char *format, va_list ap) {
    NOT_IMPLEMENTED("vfprintf");
    LOG(INFO, ("vfprintf\n"));
    init_mapping();
    return _vfprintf(stream, format, ap);
}

int fputs(const char *s, FILE *stream) { 
    LOG(INFO, ("fputs\n"));
    init_mapping();
    return _fputs(s, stream);
}

FILE *fdopen(int fd, const char *mode) { NOT_IMPLEMENTED("fdopen"); } 
FILE *freopen(const char *path, const char *mode, FILE *stream) { NOT_IMPLEMENTED("freopen"); } 
int fseek(FILE *stream, long offset, int whence) { NOT_IMPLEMENTED("fseek"); } 
long ftell(FILE *stream) { NOT_IMPLEMENTED("ftell"); } 
int fgetpos(FILE *stream, fpos_t *pos) { NOT_IMPLEMENTED("fgetpos"); }
int fsetpos(FILE *stream, const fpos_t *pos) { NOT_IMPLEMENTED("fsetpos"); }
int getc(FILE *stream) { NOT_IMPLEMENTED("getc"); }
int getchar(void) { NOT_IMPLEMENTED("getchar"); }
char *gets(char *s) { NOT_IMPLEMENTED("gets"); }
int fgetc(FILE *stream) { NOT_IMPLEMENTED("fgetc"); }
off64_t lseek64(int fd, off64_t offset, int whence) { NOT_IMPLEMENTED("lseek64"); } 
void rewind(FILE *stream) { NOT_IMPLEMENTED("rewind"); }
int creat(const char *pathname, mode_t mode) { NOT_IMPLEMENTED("creat"); } 
int stat(const char *path, struct stat *buf) { NOT_IMPLEMENTED("stat"); }
int fstat(int fd, struct stat *buf) { NOT_IMPLEMENTED("fstat"); } 
int lstat(const char *path, struct stat *buf) { NOT_IMPLEMENTED("lstat"); } 
int truncate(const char *path, off_t length) { NOT_IMPLEMENTED("truncate"); } 
int open64(const char * pathname, int flags, ...) { NOT_IMPLEMENTED("open64"); }
int access(const char *pathname, int mode) { NOT_IMPLEMENTED("access"); }
int fcntl(int fd, int cmd, ...) { NOT_IMPLEMENTED("fcntl"); }
int dup(int oldfd) { NOT_IMPLEMENTED("dup"); }
int dup2(int oldfd, int newfd) { NOT_IMPLEMENTED("dup2"); }
int pipe(int filedes[2]) { NOT_IMPLEMENTED("pipe"); }
int mkfifo( const char *pathname, mode_t mode ) { NOT_IMPLEMENTED("mkfifo"); }
mode_t umask(mode_t mask) { NOT_IMPLEMENTED("umask"); }
int fputc(int c, FILE *stream) { NOT_IMPLEMENTED("fputc"); }
int putc(int c, FILE *stream) { NOT_IMPLEMENTED("putc"); }
int putchar(int c) { NOT_IMPLEMENTED("putchar"); }
