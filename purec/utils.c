#include "utils.h"
#include <stdlib.h>

int fread_int(FILE* fp, int* target) {
    int n_ok = fscanf(fp, "%d", target);
    if (n_ok == EOF) { printf("INPUT FAILURE, PANICKING!"); exit(1); }
    // 若未成功读入，丢弃缓冲区
    if (n_ok < 1) { while (fgetc(fp) != '\n') {} return -1; }
    return 0;
}

int read_int(int* target) {
    return fread_int(stdin, target);
}

// 注：为了保留结尾\0，实际能读入的字符数为limit - 1
int fread_string(FILE* fp, char* target, int limit, char end, bool drop_left) {
    fscanf(fp, " "); // 去除空白
    int i; bool finish = false;
    for (i = 0; i < limit - 1; ++i) {
        int ch = fgetc(fp);
        if (ch == end) { finish = true; break; }
        target[i] = ch;
    }
    target[i] = 0;
    if (!finish && drop_left) { while (fgetc(fp) != end) { ++i; } }
    return i;
}

int read_string(char* target, int limit, char end, bool drop_left) {
    return fread_string(stdin, target, limit, end, drop_left);
}
