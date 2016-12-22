#define LOG_LEVEL 10

FILE* flog = NULL;
int flag = 0;

void log_init() {
    flog = _fopen("/tmp/log.txt", "rw");
}

#define LOG3(LEVEL, STR) do {} while(0);
#define LOG2(LEVEL, STR) do {\
        if (!flag) { flag = 1; log_init(); } \
        if ((LEVEL) < LOG_LEVEL) {\
                    printf("%-20s | %3d |  ",__FUNCTION__,__LINE__); \
                    _fprintf(flog, "%-20s | %3d |  ",__FUNCTION__,__LINE__); \
                    printf STR; \
                } \
} while(0);
#define LOG(LEVEL, STR) do {\
        if ((LEVEL) < LOG_LEVEL && _printf != NULL) {\
                    _printf("%-20s | %3d |  ",__FUNCTION__,__LINE__); \
                    _printf STR; \
                } \
} while(0);
