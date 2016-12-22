

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
int (*_ftruncate)(int, off_t);
size_t (*_fread)(void*, size_t, size_t, FILE*);
size_t (*_fwrite)(const void*, size_t, size_t, FILE*);
char* (*_fgets)(char *s, int size, FILE *stream);
int (*_fileno)(FILE*);
int (*_fflush)(FILE*);
int (*_puts)(const char*);
int (*_fputs)(const char *s, FILE *stream);
int (*_fprintf)(FILE*, const char*, ...);
int (*_printf)(const char *format, ...);
int (*_vfprintf)(FILE *stream, const char *format, va_list ap);
int (*_dup)(int);
int (*_fclose)(FILE *);

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
    _fflush = (int (*)(FILE*))
        dlsym(RTLD_NEXT, "fflush");
    _puts = (int (*)(const char*))
        dlsym(RTLD_NEXT, "puts");
    _printf = (int (*)(const char*, ...))
        dlsym(RTLD_NEXT, "printf");
    _vfprintf = (int (*)(FILE*, const char*, va_list))
        dlsym(RTLD_NEXT, "vfprintf");
    _fputs = (int (*)(const char *s, FILE *stream))
        dlsym(RTLD_NEXT, "fputs");
    _dup = (int (*)(int))
        dlsym(RTLD_NEXT, "dup");
}

