#ifndef UI_H
#define UI_H

#include <stdio.h>
#include <conio.h>
#include <windows.h>

#define MAX_OPTIONS 9  // 8 chức năng + 1 thoát
#define MAX_SUB_OPTIONS 4  // Tối đa 4 chức năng con

// Hiển thị menu chính với lựa chọn hiện tại
void displayMenu(int selected);

// Hiển thị submenu với lựa chọn hiện tại
void displaySubMenu(int menuId, int selected);

// Xử lý input từ người dùng
void processUserInput(void);

// Xóa màn hình
void clearScreen(void);

#endif
// Quản lý giao diện người dùng