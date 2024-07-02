#include <stdio.h>
#include <stdbool.h>

int fread_int(FILE* fp, int* target);
int read_int(int* target);
int fread_string(FILE* fp, char* target, int limit, char end, bool drop_left);
int read_string(char* target, int limit, char end, bool drop_left);
