#include <stdio.h>
#include "ui.h"

int main() {
    StudentManagerUI ui;
    ui_create(&ui);
    int opcode;
    while (1) {
        ui_listops();
        printf("0.Exit\n");
        printf("Please enter your choice: "); ui_read_int(&opcode);
        if (opcode == 0) {
            printf("See you next time!\n");
            break;
        } else {
            printf("=========================\n");
            ui_dispatch(&ui, opcode);
            printf("=========================\n");
        }
    }
    ui_destroy(&ui);
}
