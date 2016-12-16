#define _GNU_SOURCE
#include <dlfcn.h>
//#define _FCNTL_H
//#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DEBUG

size_t (*_read)(int, void*, size_t);
ssize_t (*_pread)(int, void*, size_t, off_t);
ssize_t (*_pwrite)(int, const void*, size_t, off_t);
int (*_close)(int);
int (*_open64)(const char*, int, mode_t);
int (*_open)(const char*, int, ...);
int (*_open2)(const char*, int);
ssize_t (*_write)(int, const void*, size_t);
off_t (*_lseek)(int, off_t, int);

int (*_ungetc)(int c, FILE *stream);

FILE* (*_fopen)(const char*, const char*);
int (*_fclose)(FILE*);
int (*_ftruncate)(int, off_t);
size_t (*_fread)(void*, size_t, size_t, FILE*);
size_t (*_fwrite)(const void*, size_t, size_t, FILE*);
char* (*_fgets)(char *s, int size, FILE *stream);

//int open(const char * pathname, int flags, mode_t mode);

typedef ssize_t (*bwrite_f)(int, const void*, size_t);
typedef ssize_t (*bpwrite_f)(int, const void*, size_t, off_t);
typedef ssize_t (*bread_f)(int, void*, size_t);
typedef ssize_t (*bpread_f)(int, void*, size_t, off_t);
typedef int (*bopen_f)(const char*, int, mode_t);
typedef FILE (*bfopen_f)(const char*, const char*);
typedef int (*bopen2_f)(const char*, int);
typedef int (*bclose_f)(int);
typedef int (*bftruncate_f)(int, off_t);
typedef off_t (*blseek_f)(int, off_t, int);
typedef void (*binit_f)();
typedef char* (*bfgets_f)(char*, int, FILE*);

bwrite_f blade_write;
bpwrite_f blade_pwrite;
bread_f blade_read;
bpread_f blade_pread;
bopen_f blade_open;
bopen2_f blade_open2;
bopen_f blade_open64;
blseek_f blade_lseek;
bclose_f blade_close;
bftruncate_f blade_ftruncate;
binit_f blade_init;
bfgets_f blade_fgets;
void* handle;

#define NS_IN_SEC (1000000000L)
#define US_IN_SEC (1000000L)

typedef unsigned long long int ull;

static void init() __attribute__((constructor));
static void my_exit() __attribute__((destructor));

char is_good_fd[10000] = {0};

void my_exit() {
   dlclose(handle);
}
    
char inited = 0;
void init() {
    if (inited != 0) {
        return;
    }

   printf("Starting interc.so %d", inited);
   handle = dlopen("blade_layer.so", RTLD_LAZY);
   if (!handle) {
       puts(dlerror());
       puts("Error in dlopen");
       exit(-1);
   }
   
   blade_init = (binit_f)dlsym(handle, "blade_init");
   blade_write = (bwrite_f)dlsym(handle, "blade_write");
   blade_pwrite = (bpwrite_f)dlsym(handle, "blade_pwrite");
   blade_read = (bread_f)dlsym(handle, "blade_read");
   blade_pread = (bpread_f)dlsym(handle, "blade_pread");
   blade_open = (bopen_f)dlsym(handle, "blade_open");
   blade_open64 = (bopen_f)dlsym(handle, "blade_open64");
   blade_lseek = (blseek_f)dlsym(handle, "blade_lseek");
   blade_close = (bclose_f)dlsym(handle, "blade_close");
   blade_fgets = (bfgets_f)dlsym(handle, "blade_fgets");
   blade_ftruncate = (bftruncate_f)dlsym(handle, "blade_ftruncate");
   
   puts("init() done");
   inited++;
}

unsigned long long int get_time()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec*US_IN_SEC+ts.tv_nsec/1000;
}

char mapping_inited = 0;
void init_mapping() {
    if (mapping_inited)
        return;
    mapping_inited = 1;
        
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
    _fclose = (int (*)(FILE*))
        dlsym(RTLD_NEXT, "fclose");
    _ftruncate = (int (*)(int, off_t))
        dlsym(RTLD_NEXT, "ftruncate");
}

char first = 0;
int open(const char * pathname, int flags, ...)
{
    printf("open fname: %s\n", pathname);
    init_mapping();
   
    if (first == 0) { 
        first = 1;
        puts("Calling blade init");
        blade_init();
        puts("Blade init done");
    }

    if (0 && strncmp("/data/joao/ligra/utils/", pathname, 
                strlen("/data/joao/ligra/utils/")) == 0) {
        printf("calling blade_open\n");
        int ret = blade_open(pathname, flags, 0);
        printf("blade_open fd: %d\n", ret);
        is_good_fd[ret] = 1;
        return ret;
    } else {
        printf("regular open: %s\n", pathname);
        int ret = _open(pathname, flags);
        printf("ret fd: %d\n", ret);
        return ret;

    }
}

int open64(const char * pathname, int flags, mode_t mode)
{
    puts("open64");
    exit(-1);
    init_mapping();
}

int close(int fd) {
    printf("close fd: %d\n", fd);
    if (is_good_fd[fd])
        return blade_close(fd);
    else
        return _close(fd);
}

int creat(const char *pathname, mode_t mode) {
    puts("creat");
    exit(-1);
    return 0;
}

ssize_t write(int fd, const void *buf, size_t count) {
    printf("write fd:%d\n", fd);
    init_mapping();
    if (is_good_fd[fd])
        return blade_write(fd, buf, count);
    else
        return _write(fd, buf, count);
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
    puts("pread");
    init_mapping();
    if (is_good_fd[fd])
        return blade_pread(fd, buf, count, offset);
    else
        return _pread(fd, buf, count, offset);
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
    puts("pwrite");
    init_mapping();
    if (is_good_fd[fd])
        return blade_pwrite(fd, buf, count, offset);
    else
        return _pwrite(fd, buf, count, offset);
}

ssize_t read(int fd, void *buf, size_t count) {
    puts("read");
    init_mapping();
    if (is_good_fd[fd])
        return blade_read(fd, buf, count);
    else
        return _read(fd, buf, count);
}


int stat(const char *path, struct stat *buf) {
    puts("stat");
    exit(-1);
    return -1;
}

int fstat(int fd, struct stat *buf) {
    puts("fstat");
    exit(-1);
    return -1;
}

int lstat(const char *path, struct stat *buf) {
    puts("lstat");
    exit(-1);
    return -1;
}

int truncate(const char *path, off_t length) {
    puts("truncate");
    exit(-1);
    return -1;
}

int ftruncate(int fd, off_t length) {
    printf("ftruncate. fd: %d length: %lu\n", fd, length);
    init_mapping();

    if (is_good_fd[fd]) {
#ifdef DEBUG
        printf("blade ftruncate. fd: %d length: %lu\n", fd, length);
#endif
        return blade_ftruncate(fd, length);
    } else {
#ifdef DEBUG
        printf("normal ftruncate. fd: %d length: %lu\n", fd, length);
#endif
        return _ftruncate(fd, length);
    }

    return 0;
}

off_t lseek(int fd, off_t offset, int whence) {

    init_mapping();
    if (is_good_fd[fd]) {
#ifdef DEBUG
        printf("blade lseek. fd: %d\n", fd);
#endif
        return blade_lseek(fd, offset, whence);
    } else {
#ifdef DEBUG
        printf("normal lseek. fd: %d\n", fd);
#endif
        return _lseek(fd, offset, whence);
    }
}

off64_t lseek64(int fd, off64_t offset, int whence) {
    puts("lseek64");
    exit(-1);
}

void rewind(FILE *stream) {
    puts("rewind");
    exit(-1);
}

// FILE* interface


FILE *fopen(const char *path, const char *mode) {
    init_mapping();
#ifdef DEBUG
    printf("fopen path: %s\n", path);
#endif

    FILE* ret = _fopen(path, mode);
#ifdef DEBUG
    printf("ret: %llu\n", (unsigned long long int) ret);
    if (ret)
        printf("fileno ret: %d\n", fileno(ret));
#endif
    if (0 && strncmp(path, "/data/joao/", strlen("/data/joao")) == 0) {
        // mark this fd so that next read()
        printf("Marking file\n");
        is_good_fd[fileno(ret)] = 1;
        return ret;
    } else {
        return ret;
    }
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    puts("fread");
    int fd = fileno(stream);
    if (is_good_fd[fd]) {
        return blade_read(fd, ptr, size);
    } else {
        return _fread(ptr, size, nmemb, stream);
    }
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb,
                                    FILE *stream) {
    int fd = fileno(stream);
    printf("fwrite fd:%d\n", fd);
    if (is_good_fd[fd]) {
        return blade_write(fd, ptr, size);
    } else {
        return _fwrite(ptr, size, nmemb, stream);
    }
}


FILE *fdopen(int fd, const char *mode) {
    puts("fdopen");
    exit(-1);
    return 0;
}

FILE *freopen(const char *path, const char *mode, FILE *stream) {
    puts("freopen1");
    exit(-1);
    return 0;
}

int fseek(FILE *stream, long offset, int whence) {
    puts("fseek");
    exit(-1);
}

long ftell(FILE *stream) {
    puts("ftell");
    exit(-1);
}

int fgetpos(FILE *stream, fpos_t *pos) {
    puts("fgetpos");
    exit(-1);
}

//int fsetpos(FILE *stream, fpos_t *pos) {
//    puts("fsetpos");
//    exit(-1);
//}


int fgetc(FILE *stream) {
    puts("fgetc");
    exit(-1);
}

char *fgets(char *s, int size, FILE *stream) {
    printf("Computing fgets\n");
    int fd = fileno(stream);
    printf("fgets. fd: %d\n", fd);
    init_mapping();

    if (is_good_fd[fd]) {
        printf("blade fgets\n");
        return blade_fgets(s, size, stream);
    } else {
        char* ret = _fgets(s, size, stream);
        printf("normal fgets ret: %d\n", ret != NULL);
        return ret;
    }
}

int getc(FILE *stream) {
    puts("getc");
    exit(-1);
}

int getchar(void) {
    puts("getchar");
    exit(-1);
}

char *gets(char *s) {
    puts("gets");
    exit(-1);
}

int ungetc(int c, FILE *stream) {
    init_mapping();
    int fd = fileno(stream);
    printf("ungetc. fd: %d\n", fd);
    return _ungetc(c, stream);
}

