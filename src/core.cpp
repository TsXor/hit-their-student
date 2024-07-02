#include <stdlib.h>
#include <string.h>
#include "core.hpp"

const char* subject_name(int code) {
    static const char* name_table[size_t(Subject::MAX)] = {
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
    static const char* name_table[size_t(Evaluation::MAX)] = {
        "supreme",  // 优秀
        "great",    // 良好
        "middle",   // 中等
        "passing",  // 及格
        "failing",  // 不及格
    };
    return name_table[code];
}

Evaluation evaluate_score(int score) {
    if (score > 100) { return Evaluation::UNKNOWN; }
    if (score >= 90) { return Evaluation::SUPREME; }
    if (score >= 80) { return Evaluation::GREAT; }
    if (score >= 70) { return Evaluation::MIDDLE; }
    if (score >= 60) { return Evaluation::PASSING; }
    if (score >= 0) { return Evaluation::FAILING; }
    return Evaluation::UNKNOWN;
}
