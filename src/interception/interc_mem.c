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
#include "log.h"

#define DEBUG
#define NS_IN_SEC (1000000000L)
#define US_IN_SEC (1000000L)
#define FILE_SIZE (1000000000)

// set to 1 to intercept calls to specific file
#define ENABLE 0
#define INFO 0

#define NOT_IMPLEMENTED(str) { printf("Not implemented %s\n", str); exit(-1); }


pthread_mutex_t mutex;

typedef unsigned long long int ull;

//static void init() __attribute__((constructor));
static void my_exit() __attribute__((destructor));

char is_good_fd[10000] = {0};
int fd_counter = 100;

char* file_data[1000];
int file_ptr[1000];
int special_fd = 0;
int special_fd_size = 0;

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

    //pthread_mutexattr_t attr;
    //pthread_mutexattr_init(&attr);
    //pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    //pthread_mutex_init(&mutex, &attr);
    
    _open = (int (*)(const char * pathname, int flags, ...))
        dlsym(RTLD_NEXT, "open");
    _open64 = (int (*)(const char * pathname, int flags, mode_t))
        dlsym(RTLD_NEXT, "open64");
    _close = (int (*)(int fd))
        dlsym(RTLD_NEXT, "close");
    _write = (ssize_t (*)(int fd, const void* bug, size_t count))
        dlsym(RTLD_NEXT, "write");
    _pread = (ssize_t (*)(int fd, void *buf, size_t count, off_t offset))
        dlsym(RTLD_NEXT, "pread");
    _pwrite = (ssize_t (*)(int fd, const void *buf, size_t count, off_t offset))
        dlsym(RTLD_NEXT, "pwrite");
    _read = (size_t (*)(int fd, void *buf, size_t count))
        dlsym(RTLD_NEXT, "read");
    _lseek = (off_t (*)(int fd, off_t offset, int whence))
        dlsym(RTLD_NEXT, "lseek");

    _ungetc = (int (*)(int c, FILE*))
        dlsym(RTLD_NEXT, "ungetc");

    _fopen = (FILE* (*)(const char *path, const char *mode))
        dlsym(RTLD_NEXT, "fopen");
    _fwrite = (size_t (*)(const void*, size_t, size_t, FILE*))
        dlsym(RTLD_NEXT, "fwrite");
    _fread = (size_t (*)(void*, size_t, size_t, FILE*))
        dlsym(RTLD_NEXT, "fread");
    _fgets = (char* (*)(char*, int, FILE*))
        dlsym(RTLD_NEXT, "fgets");
    _fprintf = (int (*)(FILE*, const char*, ...))
        dlsym(RTLD_NEXT, "fprintf");
    _fclose = (int (*)(FILE*))
        dlsym(RTLD_NEXT, "fclose");
    _ftruncate = (int (*)(int, off_t))
        dlsym(RTLD_NEXT, "ftruncate");
}

void print_flags(int flags) {
    LOG(INFO, ("flags: O_APPEND: %d O_ASYNC: %d O_CREAT: %d O_DIRECT: %d O_NONBLOCK: %d O_SYNC: %d\n", 
                    flags & O_APPEND, flags & O_ASYNC, flags & O_CREAT, flags&O_DIRECT, flags&O_NONBLOCK,
                    flags & O_SYNC));
}

char first = 0;
int open(const char * pathname, int flags, ...) {
    init_mapping();
    LOG(INFO, ("open fname: %s\n", pathname));
    print_flags(flags);
   
    pthread_mutex_lock(&mutex);
    if (ENABLE && strcmp("/data/joao/ligra/utils/my_edge_1M", pathname) == 0) {
        int fd = fd_counter++;
        is_good_fd[fd] = 1;
        file_data[fd] = (char*)malloc(FILE_SIZE);
        pthread_mutex_unlock(&mutex);
        LOG(INFO, ("blade open returning fd: %d\n", fd));
        return fd;
    } else {
        LOG(INFO, ("regular open: %s\n", pathname));
        int ret = _open(pathname, flags);
        LOG(INFO, ("ret fd: %d\n", ret));
        pthread_mutex_unlock(&mutex);
        return ret;

    }
}

int close(int fd) {
    pthread_mutex_lock(&mutex);

    if (is_good_fd[fd]) {
#ifdef DEBUG
        LOG(INFO, ("blade close fd: %d\n", fd));
#endif
        pthread_mutex_unlock(&mutex);
        return 0;
    } else {
#ifdef DEBUG
        LOG(INFO, ("normal close fd: %d\n", fd));
#endif
        pthread_mutex_unlock(&mutex);
        return _close(fd);
    }
}

ssize_t write(int fd, const void *buf, size_t count) {
    LOG(INFO, ("write fd: %d\n", fd));
    init_mapping();
    
    pthread_mutex_lock(&mutex);
    if (is_good_fd[fd]) {
        LOG(INFO, ("blade write fd:%d\n", fd));
        pthread_mutex_unlock(&mutex);
        memcpy(file_data[fd] + file_ptr[fd], buf, count);
        file_ptr[fd] += count;
        return count;
    } else {
        pthread_mutex_unlock(&mutex);
        return _write(fd, buf, count);
    }
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
    init_mapping();
    if (is_good_fd[fd]) {
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
    if (is_good_fd[fd]) {
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
    if (is_good_fd[fd]) {
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

    if (is_good_fd[fd]) {
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
    printf("lseek. fd: %d\n", fd);

    init_mapping();
    if (is_good_fd[fd]) {
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

void open_file_copy(char* fname, char* ptr) {
    LOG(INFO, ("Opening file %s\n", fname));
    int fd = _open(fname, 0);
    if (!fd)
        exit(-1);

    int size = _read(fd, ptr, FILE_SIZE);
    special_fd_size = size;

    LOG(INFO, ("Opened %s size: %d fd: %lu errno: %d\n", fname, size, fd, errno));

    _close(fd);
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
        // mark this fd so that next read()
        //char filename[100];
        int fd = fd_counter++;
        special_fd = fd;
        file_ptr[fd] = 0;
        //FILE* ret = _fopen(tmpnam_r(filename), "ab+");
#ifdef DEBUG
        LOG(INFO, ("Marking file\n"));
#endif

        open_file_copy("/data/joao/ligra/utils/my_edge_1M", file_data[fd]);

        //int fd = fileno(ret);
        file_data[fd] = (char*)malloc(FILE_SIZE);
        is_good_fd[fd] = 1;

        LOG(INFO, ("returning file our fd: %d\n", fd));
        return (FILE*)fd;
    } else {
        FILE* ret = _fopen(path, mode);
#ifdef DEBUG
        LOG(INFO, ("normal ret: %llu\n", (unsigned long long int) ret));
        if (ret)
            LOG(INFO, ("normal fileno ret: %d\n", fileno(ret)));
#endif
        return ret;
    }
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    printf("fread. size: %lu nmemb: %lu\n", size, nmemb);
    exit(-1);
    int fd = fileno(stream);
    if (is_good_fd[fd]) {
#ifdef DEBUG
        printf("blade fread. size: %lu nmemb: %lu\n", size, nmemb);
#endif
        return 0; // XXX fix
    } else {
#ifdef DEBUG
        printf("normal fread. size: %lu nmemb: %lu\n", size, nmemb);
#endif
        return _fread(ptr, size, nmemb, stream);
    }
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb,
                                    FILE *stream) {
#ifdef DEBUG
    //printf("fwrite\n");
#endif
    int fd = fileno(stream);
#ifdef DEBUG
    //if (fd != 1)
    //    printf("fwrite fd:%d\n", fd);
#endif
    if (is_good_fd[fd]) {
        NOT_IMPLEMENTED("blade fwrite");
        return 0; // XXX fix
    } else {
        return _fwrite(ptr, size, nmemb, stream);
    }
}

char *fgets(char *s, int size, FILE *stream) {
    init_mapping();

    if (stream == special_fd) {
    //if (is_good_fd[fd]) {
        LOG(INFO, ("blade fgets\n"));
        LOG(INFO, ("%s\n", file_data[file_ptr[special_fd]]));
        int count = 1;
        while (file_ptr[special_fd] < special_fd_size &&
                file_data[file_ptr[special_fd]] != '\n' &&
                file_data[file_ptr[special_fd]]) {
            *s = file_data[file_ptr[special_fd]++];
            ++s;
            count++;
        }
        *s = file_data[file_ptr[special_fd]++];
        LOG(INFO, ("blade fgets ret: %d\n", count));
        return count;
    } else {
        char* ret = _fgets(s, size, stream);
        //printf("normal fgets ret: %d\n", ret != NULL);
        return ret;
    }
}

int ungetc(int c, FILE *stream) {
    init_mapping();
    int fd = fileno(stream);
    LOG(INFO, ("ungetc. fd: %d\n", fd));
    return _ungetc(c, stream);
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
