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

//#define DISK

FILE* (*_fopen)(const char *path, const char *mode);
size_t (*_fwrite)(const void*, size_t, size_t, FILE*);
size_t (*_read)(int fd, void *buf, size_t count);
ssize_t (*_pread)(int fd, void *buf, size_t count, off_t offset);
ssize_t (*_pwrite)(int fd, const void *buf, size_t count, off_t offset);
int (*_close)(int fd);
int (*_fclose)(FILE*);
int (*_open64)(const char * pathname, int flags, mode_t mode);
int (*_open)(const char * pathname, int flags, ...);
int (*_open2)(const char * pathname, int flags);
ssize_t (*_write)(int fd, const void* bug, size_t count);
off_t (*lseek_)(int fd, off_t offset, int whence);

//int open(const char * pathname, int flags, mode_t mode);

typedef ssize_t (*bwrite_f)(int, const void*, size_t);
typedef ssize_t (*bpwrite_f)(int, const void*, size_t, off_t);
typedef ssize_t (*bread_f)(int, void*, size_t);
typedef ssize_t (*bpread_f)(int, void*, size_t, off_t);
typedef int (*bopen_f)(const char*, int, mode_t);
typedef FILE (*bfopen_f)(const char*, const char*);
typedef int (*bopen2_f)(const char*, int);
typedef int (*bclose_f)(int);
typedef int (*bftruncate_f)(int);
typedef off_t (*blseek_f)(int, off_t, int);
typedef void (*binit_f)();

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
binit_f blade_init_;
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
   
   blade_init_ = (binit_f)dlsym(handle, "blade_init");
   blade_write = (bwrite_f)dlsym(handle, "blade_write");
   blade_pwrite = (bpwrite_f)dlsym(handle, "blade_pwrite");
   blade_read = (bread_f)dlsym(handle, "blade_read");
   blade_pread = (bpread_f)dlsym(handle, "blade_pread");
   blade_open = (bopen_f)dlsym(handle, "blade_open");
   blade_open64 = (bopen_f)dlsym(handle, "blade_open64");
   blade_lseek = (blseek_f)dlsym(handle, "blade_lseek");
   blade_close = (bclose_f)dlsym(handle, "blade_close");
   
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
    _fopen = (FILE* (*)(const char *path, const char *mode))
        dlsym(RTLD_NEXT, "fopen");
    _write = (ssize_t (*)(int fd, const void* bug, size_t count))
        dlsym(RTLD_NEXT, "write");
    _pread = (ssize_t (*)(int fd, void *buf, size_t count, off_t offset))
        dlsym(RTLD_NEXT, "pread");
    _pwrite = (ssize_t (*)(int fd, const void *buf, size_t count, off_t offset))
        dlsym(RTLD_NEXT, "pwrite");
    //_fwrite = (size_t (*)(const void*, size_t, size_t, FILE*))
    //    dlsym(RTLD_NEXT, "fwrite");
    _read = (size_t (*)(int fd, void *buf, size_t count))
        dlsym(RTLD_NEXT, "read");
    //_fclose = (int (*)(FILE*))
    //    dlsym(RTLD_NEXT, "fclose");
    lseek_ = (off_t (*)(int fd, off_t offset, int whence))
        dlsym(RTLD_NEXT, "lseek");
}

char first = 0;
int open(const char * pathname, int flags, ...)
{
    printf("open fname: %s\n", pathname);
    init_mapping();
   
    if (first == 0) { 
        first = 1;
        puts("Calling blade init");
        blade_init_();
        puts("Blade init done");
    }

#ifdef DISK
    return _open(pathname, flags);
    //return _open(pathname, flags, mode);
#else
    if (strncmp("/sys", pathname, 4) == 0 ||
            strncmp("/dev", pathname, 4) == 0) {
        printf("regular open: %s\n", pathname);
        return _open(pathname, flags);
        //return _open(pathname, flags, mode);
    }
    else {
        printf("blade_open\n");
        int ret = blade_open(pathname, flags, 0);
        is_good_fd[ret] = 1;
        return ret;
    }
#endif
}


int open64(const char * pathname, int flags, mode_t mode)
{
    puts("open64");
    exit(-1);
    init_mapping();
}

int close(int fd) {
    puts("close");
#ifdef DISK
    int ret = _close(fd);
    return ret;
#else
    if (is_good_fd[fd])
        return blade_close(fd);
    else
        return _close(fd);
#endif
}

int creat(const char *pathname, mode_t mode) {
    puts("creat");
    exit(-1);
    return 0;
}

FILE *fopen(const char *path, const char *mode) {
    printf("fopen path: %s\n", path);
    init_mapping();
    FILE* ret = _fopen(path, mode);
    return ret;
}

//FILE *fdopen(int fd, const char *mode) {
//    puts("fdopen");
//    exit(-1);
//    return 0;
//}
//
//FILE *freopen(const char *path, const char *mode, FILE *stream) {
//    puts("freopen1");
//    exit(-1);
//    return 0;
//}

ssize_t write(int fd, const void *buf, size_t count) {
    puts("write");
    init_mapping();
#ifdef DISK
    ssize_t ret = _write(fd, buf, count);
    return ret;
#else
    if (is_good_fd[fd])
        return blade_write(fd, buf, count);
    else
        return _write(fd, buf, count);
#endif
}

ssize_t pread(int fd, void *buf, size_t count, off_t offset) {
    puts("pread");
    ull start = get_time();
    init_mapping();
#ifdef DISK
    ssize_t ret = _pread(fd, buf, count, offset);
    printf("size: %lu %llu\n", count, get_time() - start);
    return ret;
#else
    if (is_good_fd[fd])
        return blade_pread(fd, buf, count, offset);
    else
        return _pread(fd, buf, count, offset);
#endif
}

ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) {
    puts("pwrite");
    //ull start = get_time();
    init_mapping();
#ifdef DISK
    ssize_t ret = _pwrite(fd, buf, count, offset);
    return ret;
#else
    if (is_good_fd[fd])
        return blade_pwrite(fd, buf, count, offset);
    else
        return _pwrite(fd, buf, count, offset);
#endif
}

ssize_t read(int fd, void *buf, size_t count) {
    puts("read");
    ull start = get_time();
    init_mapping();
#ifdef DISK
    ssize_t ret = _read(fd, buf, count);
    return ret;
#else
    if (is_good_fd[fd])
        return blade_read(fd, buf, count);
    else
        return _read(fd, buf, count);
#endif
}

//size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
//    puts("fread");
//    exit(-1);
//    return 0;
//}
//
//size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
//    //puts("fwrite");
//    //ull start = get_time();
//    init_mapping();
//    size_t ret = _fwrite(ptr, size, nmemb, stream);
//    //printf("%llu\n", get_time() - start);
//    return ret;
//}
//
//int fclose(FILE *fp) {
//    //ull start = get_time();
//    init_mapping();
//    int ret = _fclose(fp);
//    //printf("%llu\n", get_time() - start);
//    return ret;
//}

int stat(const char *path, struct stat *buf) {
    puts("stat");
    exit(-1);
}

int fstat(int fd, struct stat *buf) {
    puts("fstat");
    exit(-1);
}

int lstat(const char *path, struct stat *buf) {
    puts("lstat");
    exit(-1);
}

int truncate(const char *path, off_t length) {
    puts("truncate");
    exit(-1);
}

int ftruncate(int fd, off_t length) {
    printf("ftruncate. fd: %d length: %lu\n", fd, length);
    init_mapping();

    return 0;
}

off_t lseek(int fd, off_t offset, int whence) {
    puts("lseek");
    init_mapping();
#ifdef DISK
    return lseek_(fd, offset, whence);
#else
    if (is_good_fd[fd])
        return blade_lseek(fd, offset, whence);
    else
        return lseek_(fd, offset, whence);
#endif
}

off64_t lseek64(int fd, off64_t offset, int whence) {
    puts("lseek64");
    exit(-1);
}

int fseek(FILE *stream, long offset, int whence) {
    puts("fseek");
    exit(-1);
}

long ftell(FILE *stream) {
    puts("ftell");
    exit(-1);
}

void rewind(FILE *stream) {
    puts("rewind");
    exit(-1);
}

int fgetpos(FILE *stream, fpos_t *pos) {
    puts("fgetpos");
    exit(-1);
}
