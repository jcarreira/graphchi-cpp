//#define _GNU_SOURCE
#include <dlfcn.h>
//#define _FCNTL_H
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <time.h>


extern int errorno;

int (*_open)(const char * pathname, int flags, ...);
int (*_open64)(const char * pathname, int flags, ...);
FILE* (*_fopen)(const char *path, const char *mode);
size_t (*_fwrite)(const void*, size_t, size_t, FILE*);
size_t (*_read)(int fd, void *buf, size_t count);
ssize_t (*_pread)(int fd, void *buf, size_t count, off_t offset);
ssize_t (*_pwrite)(int fd, const void *buf, size_t count, off_t offset);
int (*_close)(int fd);
int (*_fclose)(FILE*);

#define NS_IN_SEC (1000000000L)
#define US_IN_SEC (1000000L)

typedef unsigned long long int ull;

unsigned long long int get_time()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec*US_IN_SEC+ts.tv_nsec/1000;
}

int open(const char * pathname, int flags, mode_t mode)
{
    exit(-1);
    _open = (int (*)(const char * pathname, int flags, ...)) dlsym(RTLD_NEXT, "open");
    if(flags & O_CREAT)
        return _open(pathname, flags | O_NOATIME, mode);
    else
        return _open(pathname, flags | O_NOATIME, 0);
}

int open64(const char * pathname, int flags, mode_t mode)
{
    exit(-1);
    _open64 = (int (*)(const char * pathname, int flags, ...)) dlsym(RTLD_NEXT, "open64");
    if(flags & O_CREAT)
        return _open64(pathname, flags | O_NOATIME, mode);
    else
        return _open64(pathname, flags | O_NOATIME, 0);
}

int open(const char *pathname, int flags) {
    puts("open");
    exit(-1);
    return 0;
}

int close(int fd) {
//    puts("close");
    if (!_close)
        _close = (int (*)(int fd)) dlsym(RTLD_NEXT, "close");
    int ret = _close(fd);
    return ret;
}

int creat(const char *pathname, mode_t mode) {
    puts("creat");
    exit(-1);
    return 0;
}

FILE *fopen(const char *path, const char *mode) {
  //  puts("fopen");
    ull start = get_time();
    if (!_fopen)
        _fopen = (FILE* (*)(const char *path, const char *mode)) dlsym(RTLD_NEXT, "fopen");
    FILE* ret = _fopen(path, mode);
    //printf("%llu\n", get_time() - start);
    return ret;
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

ssize_t write(int fd, const void *buf, size_t count) {
    puts("write");
    exit(-1);
    return 0;
}
ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
    //puts("pread");
    ull start = get_time();
    if (!_pread)
        _pread = (ssize_t (*)(int fd, void *buf, size_t count, off_t offset)) dlsym(RTLD_NEXT, "pread");
    ssize_t ret = _pread(fd, buf, count, offset);
    printf("size: %lu %llu\n", count, get_time() - start);
    return ret;
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
    //puts("write");
    ull start = get_time();
    if (!_pwrite)
        _pwrite = (ssize_t (*)(int fd, const void *buf, size_t count, off_t offset)) dlsym(RTLD_NEXT, "pwrite");
    ssize_t ret = _pwrite(fd, buf, count, offset);
    //printf("%llu\n", get_time() - start);
    return ret;
}

ssize_t read(int fd, void *buf, size_t count) {
    //puts("read");
    ull start = get_time();
    if (!_read)
        _read = (size_t (*)(int fd, void *buf, size_t count)) dlsym(RTLD_NEXT, "read");
    ssize_t ret = _read(fd, buf, count);
    printf("size: %lu %llu\n", count, get_time() - start);
    return ret;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    puts("fread");
    exit(-1);
    return 0;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
    //puts("fwrite");
    ull start = get_time();
    if (!_fwrite)
        _fwrite = (size_t (*)(const void*, size_t, size_t, FILE*)) dlsym(RTLD_NEXT, "fwrite");
    size_t ret = _fwrite(ptr, size, nmemb, stream);
    //printf("%llu\n", get_time() - start);
    return ret;
}

int fclose(FILE *fp) {
    ull start = get_time();
    if (!_fclose)
        _fclose = (int (*)(FILE*)) dlsym(RTLD_NEXT, "fclose");
    int ret = _fclose(fp);
    //printf("%llu\n", get_time() - start);
    return ret;
}
