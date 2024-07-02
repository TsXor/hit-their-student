#include "ui.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


void ui_read_int(int* target) {
    while (read_int(target) != 0) {
        printf("Input is not a number, retry: ");
    }
}

void ui_read_string(char* target, int limit) {
    int read = read_string(target, limit, '\n', true);
    if (read > limit - 1) {
        printf("Input exceeded length limit of %d, which is truncated.\n", limit - 1);
    }
}

typedef void(*UISubroutine)(StudentManagerUI*);

void add_student(StudentManagerUI* self) {
    StudentInfo* stu = studb_push(&self->db);
    printf("Input student's ID: "); ui_read_int(&stu->id);
    printf("Input student's name: "); ui_read_string(stu->name, 32);
    for (int sub = 0; sub < SUBJECT_MAX; ++sub) {
        printf("Input student's score of %s: ", subject_name(sub));
        ui_read_int(&stu->scores[sub]);
    }
}

void course_score_info(StudentManagerUI* self) {
    long long int course_sums[SUBJECT_MAX];
    for (int sub = 0; sub < SUBJECT_MAX; ++sub) { course_sums[sub] = 0; }
    for (size_t i = 0; i < self->db.size; ++i) {
        StudentInfo* stu = &self->db.data[i];
        for (int sub = 0; sub < SUBJECT_MAX; ++sub) { course_sums[sub] += stu->scores[sub]; }
    }
    for (int sub = 0; sub < SUBJECT_MAX; ++sub) {
        double avg = (double)course_sums[sub] / self->db.size;
        printf("Score of %s:\ttotal=%d, average=%lf\n", subject_name(sub), course_sums[sub], avg);
    }
}

void student_score_info(StudentManagerUI* self) {
    for (size_t i = 0; i < self->db.size; ++i) {
        StudentInfo* stu = &self->db.data[i];
        int blend_sum = stu_score_sum(stu);
        double avg = (double)blend_sum / SUBJECT_MAX;
        printf("Score of %s:\ttotal=%d, average=%lf\n", stu->name, blend_sum, avg);
    }
}

void print_student_detail(StudentInfo* stu) {
    int blend_sum = stu_score_sum(stu);
    printf("<id=%d>%s:\ttotal=%d", stu->id, stu->name, blend_sum);
    for (int sub = 0; sub < SUBJECT_MAX; ++sub) {
        printf(", %s=%d", subject_name(sub), stu->scores[sub]);
    }
}

void student_sort(StudentManagerUI* self, StudentIndexCompare cmp, bool desc) {
    StudentIndex index = malloc(self->db.size * sizeof(StudentPtr));
    studb_index(&self->db, index);
    studb_sortidx(&self->db, index, cmp);
    for (size_t i = 0; i < self->db.size; ++i) {
        StudentInfo* stu = desc ? index[self->db.size - i - 1] : index[i];
        int blend_sum = stu_score_sum(stu);
        printf("Rank(%d)\t", i + 1);
        print_student_detail(stu);
        printf("\n");
    }
    free(index);
}

void student_sort_score_desc(StudentManagerUI* self) {
    student_sort(self, stucmp_score, true);
}

void student_sort_score_asc(StudentManagerUI* self) {
    student_sort(self, stucmp_score, false);
}

void student_sort_id_asc(StudentManagerUI* self) {
    student_sort(self, stucmp_id, false);
}

void student_sort_name_asc(StudentManagerUI* self) {
    student_sort(self, stucmp_name, false);
}

void find_by_id(StudentManagerUI* self) {
    int target_id;
    printf("Input student's ID: "); ui_read_int(&target_id);
    StudentInfo* found = NULL;
    for (size_t i = 0; i < self->db.size; ++i) {
        StudentInfo* stu = &self->db.data[i];
        if (stu->id == target_id) {
            found = stu; break;
        }
    }
    if (found) {
        printf("Found student: ");
        print_student_detail(found);
        printf("\n");
    } else {
        printf("Specified student not found!\n");
    }
}

void find_by_name(StudentManagerUI* self) {
    char target_name[32];
    printf("Input student's name: "); ui_read_string(target_name, 32);
    StudentInfo* found = NULL;
    for (size_t i = 0; i < self->db.size; ++i) {
        StudentInfo* stu = &self->db.data[i];
        if (strcmp(stu->name, target_name) == 0) {
            found = stu; break;
        }
    }
    if (found) {
        printf("Found student: ");
        print_student_detail(found);
        printf("\n");
    } else {
        printf("Specified student not found!\n");
    }
}

void classify_student(StudentManagerUI* self) {
    for (int sub = 0; sub < SUBJECT_MAX; ++sub) {
        size_t student_counts[EVALUATION_MAX];
        for (int ev = 0; ev < EVALUATION_MAX; ++ev) { student_counts[ev] = 0; }
        size_t invalid_count = 0;
        for (size_t i = 0; i < self->db.size; ++i) {
            StudentInfo* stu = &self->db.data[i];
            int ev = evaluate_score(stu->scores[sub]);
            if (ev == -1) { ++invalid_count; } else { ++student_counts[ev]; }
        }
        double invalid_percent = (double)invalid_count / self->db.size * 100;
        printf("Subject %s: ", subject_name(sub));
        printf("invalid=%d(%lf%%)", invalid_count, invalid_percent);
        for (int ev = 0; ev < EVALUATION_MAX; ++ev) {
            double percent = (double)student_counts[ev] / self->db.size * 100;
            printf(", %s=%d(%lf%%)", evaluation_name(ev), student_counts[ev], percent);
        }
        printf("\n");
    }
}

// 是的，这个函数就是复制了course_score_info然后加了几句话。
void list_students(StudentManagerUI* self) {
    long long int course_sums[SUBJECT_MAX];
    for (int sub = 0; sub < SUBJECT_MAX; ++sub) { course_sums[sub] = 0; }
    printf("Student details:\n");
    for (size_t i = 0; i < self->db.size; ++i) {
        StudentInfo* stu = &self->db.data[i];
        for (int sub = 0; sub < SUBJECT_MAX; ++sub) { course_sums[sub] += stu->scores[sub]; }
        print_student_detail(stu);
        printf("\n");
    }
    printf("Overall statistics:\n");
    for (int sub = 0; sub < SUBJECT_MAX; ++sub) {
        double avg = (double)course_sums[sub] / self->db.size;
        printf("Score of %s:\ttotal=%d, average=%lf\n", subject_name(sub), course_sums[sub], avg);
    }
}

void dump_data(StudentManagerUI* self) {
    FILE* fp = fopen("students.txt", "w");
    if (fp == NULL) {
        printf("Cannot open file: %s", strerror(errno));
        return;
    }
    fprintf(fp, "%zu\n", self->db.size);
    for (size_t i = 0; i < self->db.size; ++i) {
        StudentInfo* stu = &self->db.data[i];
        fprintf(fp, "%d,%s", stu->id, stu->name);
        for (int sub = 0; sub < SUBJECT_MAX; ++sub) {
            fprintf(fp, ",%d", stu->scores[sub]);
        }
        fprintf(fp, "\n");
    }
    fflush(fp);
    fclose(fp);
    printf("Finished writing.\n");
}

void load_data(StudentManagerUI* self) {
    FILE* fp = fopen("students.txt", "r");
    if (fp == NULL) {
        printf("Cannot open file: %s", strerror(errno));
        return;
    }
    fscanf(fp, "%zu", &self->db.size);
    studb_preserve(&self->db, self->db.size);
    for (size_t i = 0; i < self->db.size; ++i) {
        StudentInfo* stu = &self->db.data[i];
        fscanf(fp, "%d,", &stu->id);
        fread_string(fp, stu->name, 32, ',', true);
        ungetc(',', fp);
        for (int sub = 0; sub < SUBJECT_MAX; ++sub) {
            fscanf(fp, ",%d", &stu->scores[sub]);
        }
    }
    fflush(fp);
    fclose(fp);
    printf("Finished reading.\n");
}

const struct {
    UISubroutine fn;
    const char* info;
} dispatch_table[] = {
    /* 0 */ {NULL, NULL},
    /* 1 */ {add_student, "Input record"},
    /* 2 */ {course_score_info, "Calculate total and average score of every course"},
    /* 3 */ {student_score_info, "Calculate total and average score of every student"},
    /* 4 */ {student_sort_score_desc, "Sort in descending order by total score of every student"},
    /* 5 */ {student_sort_score_asc, "Sort in ascending order by total score of every student"},
    /* 6 */ {student_sort_id_asc, "Sort in ascending order by number"},
    /* 7 */ {student_sort_name_asc, "Sort in dictionary order by name"},
    /* 8 */ {find_by_id, "Search by number"},
    /* 9 */ {find_by_name, "Search by name"},
    /* 10 */ {classify_student, "Statistic analysis for every course"},
    /* 11 */ {list_students, "List record"},
    /* 12 */ {dump_data, "Write to a file"},
    /* 13 */ {load_data, "Read from a file"},
};

const int max_dispatch_code = sizeof(dispatch_table) / sizeof(dispatch_table[0]);

void ui_create(StudentManagerUI* self) {
    studb_create(&self->db);
}
void ui_destroy(StudentManagerUI* self) {
    studb_destroy(&self->db);
}
void ui_dispatch(StudentManagerUI* self, int code) {
    if (code <= 0 || code >= max_dispatch_code) {
        printf("Unknown operation code (%d)!\n", code);
    } else {
        dispatch_table[code].fn(self);
    }
}
void ui_listops(void) {
    for (int i = 1; i < max_dispatch_code; ++i) {
        printf("%d.%s\n", i, dispatch_table[i].info);
    }
}
