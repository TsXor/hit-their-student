#include "core.h"

typedef struct {
    StudentDatabase db;
} StudentManagerUI;

void ui_read_int(int* target);
void ui_read_string(char* target, int limit);
void ui_listops(void);
void ui_create(StudentManagerUI* self);
void ui_destroy(StudentManagerUI* self);
void ui_dispatch(StudentManagerUI* self, int code);
