#include <iostream>
#include "ui.hpp"

int main() {
    StudentManagerUI ui;
    while (true) {
        ui.listops();
        std::cout << "Please enter your choice: ";
        int opcode = ui.read_int();
        if (opcode == 0) {
            std::cout << "See you next time!" << '\n'; break;
        } else {
            std::cout << "=========================" << '\n';
            ui.dispatch(opcode);
            std::cout << "=========================" << '\n';
        }
    }
}
