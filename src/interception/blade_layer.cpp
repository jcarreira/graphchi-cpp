#include <BladeClient.h>

#include <map>
#include <cstdio>

sirius::BladeClient client;

std::map<int, size_t> fd_to_offset;
std::map<std::string, int> path_to_fd;

extern "C"
void dummy() {
    puts("dummy");
}

extern "C"
void blade_init() {
    puts("Blade init2");
    client.connect("10.10.49.83", "12345");
    puts("Blade connected");
}

extern "C"
void blade_write(int fd, void* data, size_t size) {

}

extern "C"
void blade_pwrite(int fd, void* data, size_t size, off_t) {

}

extern "C"
void blade_read(int fd, void *buf, size_t count) {

}

extern "C"
ssize_t (*_pread)(int fd, void *buf, size_t count, off_t offset);
void blade_pread(int fd, void *buf, size_t count, off_t offset) {
}


extern "C"
int (*_open)(const char * pathname, int flags, mode_t mode);
void blade_open(const char * pathname, int flags, mode_t mode) {

}

extern "C"
void blade_open64(const char * pathname, int flags, mode_t mode) {

}

extern "C"
void blade_close(int fd) {

}

