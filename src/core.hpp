#include <stdint.h>
#include <string>

enum class Subject {
    MATH = 0,
    CHINESE,
    ENGLISH,
    PHYSICS,
    CHEMISTRY,
    BIOLOGY,
    POLITICS,
    HISTORY,
    GEOGRAPHY,
    MAX,
};

const char* subject_name(int code);

enum class Evaluation {
    UNKNOWN = -1,
    SUPREME = 0, // 优秀
    GREAT,       // 良好
    MIDDLE,      // 中等
    PASSING,     // 及格
    FAILING,     // 不及格
    MAX
};

const char* evaluation_name(int code);
Evaluation evaluate_score(int score);

struct StudentInfo {
    int id;
    std::string name;
    int scores[size_t(Subject::MAX)];
    
    int score_sum() const { 
        int sum = 0;
        for (auto s : scores) { sum += s; }
        return sum;
    }
};
