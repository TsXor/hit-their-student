#include <stdint.h>

enum Subject {
    S_MATH = 0,
    S_CHINESE,
    S_ENGLISH,
    S_PHYSICS,
    S_CHEMISTRY,
    S_BIOLOGY,
    S_POLITICS,
    S_HISTORY,
    S_GEOGRAPHY,
    SUBJECT_MAX,
};

const char* subject_name(int code);

enum Evaluation {
    EV_SUPREME = 0, // 优秀
    EV_GREAT,       // 良好
    EV_MIDDLE,      // 中等
    EV_PASSING,     // 及格
    EV_FAILING,     // 不及格
    EVALUATION_MAX
};

const char* evaluation_name(int code);
int evaluate_score(int score);

typedef struct {
    int id;
    char name[32];
    int scores[SUBJECT_MAX];
} StudentInfo;

int stu_score_sum(const StudentInfo* self);

typedef struct {
    StudentInfo* data;
    size_t size, capacity;
} StudentDatabase;

typedef StudentInfo* StudentPtr;
typedef StudentPtr* StudentIndex;
typedef int (*StudentIndexCompare)(const StudentPtr*, const StudentPtr*);

void studb_create(StudentDatabase* self);
void studb_destroy(StudentDatabase* self);
void studb_preserve(StudentDatabase* self, size_t new_size);
StudentInfo* studb_push(StudentDatabase* self);
void studb_pop(StudentDatabase* self);
void studb_index(const StudentDatabase* self, StudentIndex index);
void studb_sortidx(const StudentDatabase* self, StudentIndex index, StudentIndexCompare cmp);
int stucmp_score(const StudentPtr* a, const StudentPtr* b);
int stucmp_id(const StudentPtr* a, const StudentPtr* b);
int stucmp_name(const StudentPtr* a, const StudentPtr* b);
