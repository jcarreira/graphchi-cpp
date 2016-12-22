

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
int (*_fprintf)(FILE*, const char*, ...);
size_t (*_fwrite)(const void*, size_t, size_t, FILE*);
char* (*_fgets)(char *s, int size, FILE *stream);
int (*_fileno)(FILE*);
int (*_fflush)(FILE*);

void load_posix() {
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
    _fileno = (int (*)(FILE*))
        dlsym(RTLD_NEXT, "fileno");
}
