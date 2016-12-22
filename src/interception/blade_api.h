
typedef ssize_t (*bwrite_f)(int, const void*, size_t);
typedef ssize_t (*bpwrite_f)(int, const void*, size_t, off_t);
typedef ssize_t (*bread_f)(int, void*, size_t);
typedef ssize_t (*bpread_f)(int, void*, size_t, off_t);
typedef int (*bopen_f)(int, const char*, int, mode_t);
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
bopen_f blade_open64;
blseek_f blade_lseek;
bclose_f blade_close;
bftruncate_f blade_ftruncate;
binit_f blade_init;
bfgets_f blade_fgets;
