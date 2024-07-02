#include <stdlib.h>
#include <string.h>
#include "core.h"

const char* subject_name(int code) {
    static const char* name_table[SUBJECT_MAX] = {
        "math",
        "chinese",
        "english",
        "physics",
        "chemistry",
        "biology",
        "politics",
        "history",
        "geography",
    };
    return name_table[code];
}

const char* evaluation_name(int code) {
    static const char* name_table[EVALUATION_MAX] = {
        "supreme",  // 优秀
        "great",    // 良好
        "middle",   // 中等
        "passing",  // 及格
        "failing",  // 不及格
    };
    return name_table[code];
}

int evaluate_score(int score) {
    if (score > 100) { return -1; }
    if (score >= 90) { return EV_SUPREME; }
    if (score >= 80) { return EV_GREAT; }
    if (score >= 70) { return EV_MIDDLE; }
    if (score >= 60) { return EV_PASSING; }
    if (score >= 0) { return EV_FAILING; }
    return -1;
}

int stu_score_sum(const StudentInfo* self) {
    int sum = 0;
    for (size_t i = 0; i < SUBJECT_MAX; ++i) {
        sum += self->scores[i];
    }
    return sum;
}

static const size_t cap_block = 256;

void studb_create(StudentDatabase* self) {
    self->size = 0; self->capacity = cap_block;
    self->data = malloc(cap_block * sizeof(StudentInfo));
}
void studb_destroy(StudentDatabase* self) {
    self->size = 0; self->capacity = 0;
    free(self->data);
}
void studb_preserve(StudentDatabase* self, size_t new_size) {
    if (new_size <= self->capacity) { return; }
    size_t n_blocks = new_size ? ((new_size - 1) / cap_block + 1) : 1;
    StudentInfo* new_data = malloc(n_blocks * cap_block * sizeof(StudentInfo));
    memcpy(new_data, self->data, self->size * sizeof(StudentInfo));
    free(self->data); self->data = new_data;
    self->capacity = n_blocks * cap_block;
}
StudentInfo* studb_push(StudentDatabase* self) {
    ++self->size;
    studb_preserve(self, self->size);
    return self->data + self->size - 1;
}
void studb_pop(StudentDatabase* self) {
    --self->size;
    studb_preserve(self, self->size);
}
void studb_index(const StudentDatabase* self, StudentIndex index) {
    for (size_t i = 0; i < self->size; ++i) {
        index[i] = self->data + i;
    }
}
void studb_sortidx(const StudentDatabase* self, StudentIndex index, StudentIndexCompare cmp) {
    qsort(index, self->size, sizeof(StudentPtr), (int (*)(const void*, const void*))cmp);
}
int stucmp_score(const StudentPtr* a, const StudentPtr* b) {
    return stu_score_sum(*a) - stu_score_sum(*b);
}
int stucmp_id(const StudentPtr* a, const StudentPtr* b) {
    return (*a)->id - (*b)->id;
}
int stucmp_name(const StudentPtr* a, const StudentPtr* b) {
    return strcmp((*a)->name, (*b)->name);
}
