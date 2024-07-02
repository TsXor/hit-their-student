#include "core.hpp"
#include "simple_vector.hpp"

struct StudentManagerUI {
    utils::Vector<StudentInfo> db;
    static int read_int();
    static std::string read_string();
    static void listops();
    StudentManagerUI() {}
    ~StudentManagerUI() {}
    void dispatch(int code);
};
