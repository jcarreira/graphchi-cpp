#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define NS_IN_SEC (1000000000L)

unsigned long long int get_time()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec*NS_IN_SEC+ts.tv_nsec;
}

int open(const char *pathname, int flags) {
    puts("open not implemented");
    exit(-1);
    return 0;
}

int open(const char *pathname, int flags, mode_t mode) {
    puts("open not implemented");
    exit(-1);
    return 0;
}

int creat(const char *pathname, mode_t mode) {
    puts("creat not implemented");
    exit(-1);
    return 0;
}

ssize_t write(int fd, const void *buf, size_t count) {
    puts("write not implemented");
    exit(-1);
    return 0;
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
    puts("pread not implemented");
    exit(-1);
    return 0;
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
    puts("pwrite not implemented");
    exit(-1);
    return 0;
}

ssize_t read(int fd, void *buf, size_t count) {
    puts("read not implemented");
    exit(-1);
    return 0;
}

