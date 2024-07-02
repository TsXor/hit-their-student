#include "ui.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>


int StudentManagerUI::read_int() {
    while (true) {
        std::string line; std::getline(std::cin, line);
        std::stringstream ls; ls << line;
        int result; ls >> result;
        if (!ls.fail()) { return result; }
        std::cout << "Input(" << line << ") is not a number, retry: ";
    }
}

std::string StudentManagerUI::read_string() {
    std::string line;
    std::getline(std::cin >> std::ws, line);
    return line;
}

struct consume_string {
    size_t n_read; std::string_view target;
    template <typename... ArgTs>
    consume_string(ArgTs... args) : target(std::forward<ArgTs>(args)...) {}
    bool success() { return n_read == target.length(); }
    char next() { return target[n_read]; }
    void reset() { n_read = 0; }
};
std::istream& operator>>(std::istream& is, consume_string& manip) {
    is >> std::ws;
    while (is) {
        if (manip.next() != is.peek()) { break; }
        ++manip.n_read; is.get();
    }
    return is;
}

std::ostream& operator<<(std::ostream& os, const StudentInfo& stu) {
    std::cout << "<id=" << stu.id << ">" << stu.name << ":" << '\t' << "total=" << stu.score_sum();
    for (int sub = 0; sub < size_t(Subject::MAX); ++sub) {
        std::cout << ", " << subject_name(sub) << "=" << stu.scores[sub];
    }
    return os;
}

using UISubroutine = void(*)(StudentManagerUI&);

void add_student(StudentManagerUI& self) {
    auto& stu = self.db.push_back();
    std::cout << "Input student's ID: ";
    stu.id = self.read_int();
    std::cout << "Input student's name: ";
    stu.name = self.read_string();
    for (size_t sub = 0; sub < size_t(Subject::MAX); ++sub) {
        std::cout << "Input student's score of " << subject_name(sub) << ": ";
        stu.scores[sub] = self.read_int();
    }
}

void course_score_info(StudentManagerUI& self) {
    ssize_t course_sums[size_t(Subject::MAX)];
    for (auto& c : course_sums) { c = 0; }
    for (size_t i = 0; i < self.db.size(); ++i)
        { for (size_t sub = 0; sub < size_t(Subject::MAX); ++sub) { course_sums[sub] += self.db[i].scores[sub]; } }
    for (size_t sub = 0; sub < size_t(Subject::MAX); ++sub) {
        double avg = double(course_sums[sub]) / self.db.size();
        std::cout << "Score of " << subject_name(sub) << ":" << '\t' << "total=" << course_sums[sub] << ", average=" << avg << '\n';
    }
}

void student_score_info(StudentManagerUI& self) {
    for (size_t i = 0; i < self.db.size(); ++i) {
        auto& stu = self.db[i]; int blend_sum = stu.score_sum();
        double avg = (double)blend_sum / size_t(Subject::MAX);
        std::cout << "Score of " << stu.name << ":" << '\t' << "total=" << blend_sum << ", average=" << avg << '\n';
    }
}

template <typename Compare>
void student_sort(StudentManagerUI& self, Compare&& cmp) {
    using StudentPtr = StudentInfo*;
    utils::Vector<StudentPtr> index; index.preserve(self.db.size());
    for (size_t i = 0; i < self.db.size(); ++i) { index[i] = &self.db[i]; }
    auto index_begin = &index[0]; auto index_end = index_begin + index.size();
    std::sort(index_begin, index_end, [&](const StudentPtr& a, const StudentPtr& b) { return cmp(*a, *b); });
    for (size_t i = 0; i < self.db.size(); ++i) {
        std::cout << "Rank(" << i + 1 << ")" << '\t' << *index[i] << '\n';
    }
}

void student_sort_score_desc(StudentManagerUI& self) {
    student_sort(self, [&](const StudentInfo& a, const StudentInfo& b) { return a.score_sum() > b.score_sum(); });
}

void student_sort_score_asc(StudentManagerUI& self) {
    student_sort(self, [&](const StudentInfo& a, const StudentInfo& b) { return a.score_sum() < b.score_sum(); });
}

void student_sort_id_asc(StudentManagerUI& self) {
    student_sort(self, [&](const StudentInfo& a, const StudentInfo& b) { return a.id < b.id; });
}

void student_sort_name_asc(StudentManagerUI& self) {
    student_sort(self, [&](const StudentInfo& a, const StudentInfo& b) { return a.name < b.name; });
}

template <typename Condition>
void find_student(StudentManagerUI& self, Condition&& cond) {
    StudentInfo* found = nullptr;
    for (size_t i = 0; i < self.db.size(); ++i) {
        auto& stu = self.db[i];
        if (cond(stu)) { found = &stu; break; }
    }
    if (found) {
        std::cout << "Found student: " << *found << '\n';
    } else {
        std::cout << "Specified student not found!" << '\n';
    }
}

void find_by_id(StudentManagerUI& self) {
    printf("Input student's ID: ");
    int target_id = self.read_int();
    find_student(self, [&](const StudentInfo& stu) { return stu.id == target_id; });
}

void find_by_name(StudentManagerUI& self) {
    printf("Input student's name: ");
    std::string target_name = self.read_string();
    find_student(self, [&](const StudentInfo& stu) { return stu.name == target_name; });
}

void classify_student(StudentManagerUI& self) {
    for (int sub = 0; sub < size_t(Subject::MAX); ++sub) {
        size_t student_counts[size_t(Evaluation::MAX)];
        for (int ev = 0; ev < size_t(Evaluation::MAX); ++ev) { student_counts[ev] = 0; }
        size_t invalid_count = 0;
        for (size_t i = 0; i < self.db.size(); ++i) {
            auto& stu = self.db[i];
            auto ev = evaluate_score(stu.scores[sub]);
            if (ev == Evaluation::UNKNOWN) { ++invalid_count; } else { ++student_counts[size_t(ev)]; }
        }
        double invalid_percent = double(invalid_count) / self.db.size() * 100;
        std::cout << "Subject " << subject_name(sub) << ": ";
        std::cout << "invalid=" << invalid_count << "(" << invalid_percent << "%)";
        for (int ev = 0; ev < size_t(Evaluation::MAX); ++ev) {
            double percent = double(student_counts[ev]) / self.db.size() * 100;
            std::cout << ", " << evaluation_name(ev) << "=" << student_counts[ev] << "(" << percent << "%)";
        }
        std::cout << '\n';
    }
}

// 是的，这个函数就是复制了course_score_info然后加了几句话。
void list_students(StudentManagerUI& self) {
    ssize_t course_sums[size_t(Subject::MAX)];
    for (auto& c : course_sums) { c = 0; }
    std::cout << "Student details:" << '\n';
    for (size_t i = 0; i < self.db.size(); ++i) {
        auto& stu = self.db[i];
        for (size_t sub = 0; sub < size_t(Subject::MAX); ++sub) { course_sums[sub] += stu.scores[sub]; }
        std::cout << stu << '\n';
    }
    std::cout << "Overall statistics:" << '\n';
    for (size_t sub = 0; sub < size_t(Subject::MAX); ++sub) {
        double avg = double(course_sums[sub]) / self.db.size();
        std::cout << "Score of " << subject_name(sub) << ":" << '\t' << "total=" << course_sums[sub] << ", average=" << avg << '\n';
    }
}

void dump_data(StudentManagerUI& self) {
    std::fstream fs("students.txt", std::ios::out);
    if(!fs) { std::cout << "Cannot open file: " << strerror(errno); return; }
    fs << self.db.size() << '\n';
    for (size_t i = 0; i < self.db.size(); ++i) {
        auto& stu = self.db[i];
        fs << stu.id << "," << stu.name;
        for (int sub = 0; sub < size_t(Subject::MAX); ++sub)
            { fs << "," << stu.scores[sub]; }
        fs << '\n';
    }
    std::cout << "Finished writing." << '\n';
}

void load_data(StudentManagerUI& self) {
    std::fstream fs("students.txt", std::ios::in);
    if(!fs) { std::cout << "Cannot open file: " << strerror(errno); return; }
    size_t n_records; fs >> n_records;
    self.db.clear(); self.db.preserve(n_records);
    for (size_t i = 0; i < n_records; ++i) {
        auto& stu = self.db.push_back();
        fs >> stu.id; while (fs) { if (fs.get() == ',') { break; } }
        std::getline(fs, stu.name, ','); fs.unget();
        for (int sub = 0; sub < size_t(Subject::MAX); ++sub) {
            while (fs) { if (fs.get() == ',') { break; } }
            fs >> stu.scores[sub];
        }
    }
    std::cout << "Finished reading." << '\n';
}

const struct {
    UISubroutine fn;
    const char* info;
} dispatch_table[] = {
    /* 0 */ {NULL, "Exit"},
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

void StudentManagerUI::dispatch(int code) {
    if (code <= 0 || code >= max_dispatch_code) {
        printf("Unknown operation code (%d)!\n", code);
    } else {
        dispatch_table[code].fn(*this);
    }
}
void StudentManagerUI::listops() {
    for (int i = 0; i < max_dispatch_code; ++i) {
        std::cout << i << "." << dispatch_table[i].info << '\n';
    }
}


