#include <BladeFileClient.h>

#include <map>
#include <cstdio>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

sirius::BladeFileClient client;

std::map<int, size_t> fd_to_offset;
std::map<std::string, int> path_to_fd;
std::map<std::string, sirius::FileAllocRec> path_to_rec;
std::map<int, sirius::FileAllocRec> fd_to_rec;

int opened_files = 100;

#define GB (1024*1024*1024)
#define FILE_SIZE (1 * GB)

static const char* IP   = "10.10.49.83";
static const char* PORT = "99999";

extern "C"
void dummy() {
    puts("dummy");
}

extern "C"
void blade_init() {
    puts("Blade init2");
    client.connect(IP, PORT);
    puts("Blade connected");
}

extern "C"
ssize_t blade_write(int fd, void* data, size_t count) {
    std::cout << "Blade write fd:" << fd << std::endl;

    //off_t offset = lseek(fd, 0, SEEK_CUR);
    off_t offset = fd_to_offset[fd];
    if (offset + count > 1 * FILE_SIZE) {
        std::cout << "Blade write fd:" << fd << std::endl;
        exit(-1);
    }

    //lseek(fd, count, SEEK_CUR);
    fd_to_offset[fd] += count;

    client.write_sync(fd_to_rec[fd], offset, count, data);
    return count;
}

extern "C"
ssize_t blade_pwrite(int fd, void* data, size_t size, off_t offset) {
    std::cout << "Blade pwrite fd:" << fd << std::endl;

    if (offset + size > 1 * FILE_SIZE) {
        std::cout << "Blade pwrite fd:" << fd << std::endl;
        exit(-1);
    }

    client.write_sync(fd_to_rec[fd], offset, size, data);
    return size;
}

extern "C"
ssize_t blade_read(int fd, void *buf, size_t count) {
    printf("Blade read fd: %d\n", fd);
    return 0;

    //off_t offset = lseek(fd, 0, SEEK_CUR);
    off_t offset = fd_to_offset[fd];
    if (offset + count > 1 * FILE_SIZE) {
        std::cout << "Blade read fd:" << fd << std::endl;
        exit(-1);
    }

    client.read_sync(fd_to_rec[fd], offset, count, buf);

    //lseek(fd, count, SEEK_CUR);
    fd_to_offset[fd] += count;

    return count;
}

//ssize_t (*_pread)(int fd, void *buf, size_t count, off_t offset);
extern "C"
ssize_t blade_pread(int fd, void *buf, size_t count, off_t offset) {
    std::cout << "Blade pread fd:" << fd << std::endl;

    if (offset + count > 1 * FILE_SIZE) {
        std::cout << "Blade pread fd:" << fd << std::endl;
        exit(-1);
    }

    client.read_sync(fd_to_rec[fd], offset, count, buf);
    return count;
}

extern "C"
int blade_open(const char * pathname, int flags, mode_t mode) {
    puts("Blade opening");
    sirius::FileAllocRec alloc = client.allocate(pathname, FILE_SIZE);

    path_to_rec[pathname] = alloc;
    path_to_fd[pathname] = opened_files;
    
    //fd_to_offset[opened_files] = 0;
    fd_to_rec[opened_files] = alloc;

    return opened_files++;
}

extern "C"
void blade_open64(const char * pathname, int flags, mode_t mode) {
    exit(-1);
}

extern "C"
void blade_close(int fd) {

}

extern "C"
void blade_lseek(int fd, off_t offset, int whence) {
    puts("Blade lseek");
    //lseek(fd, offset, whence);
    if (whence == SEEK_CUR) { 
        fd_to_offset[fd] += offset;
    } else if (whence == SEEK_SET) {
        fd_to_offset[fd] = offset;
    } else if (whence == SEEK_END) {
        exit(-1);
    }
}

extern "C"
char *blade_fgets(char *s, int size, FILE *stream) {
    dummy();
    int fd = fileno(stream);
    printf("Blade read fd: %d\n", fd);
    blade_read(fd, s, size);
    puts("Blade read done");
    return s;
}

extern "C"
int blade_ftruncate(int fd, off_t length) {
    return 0;
}
