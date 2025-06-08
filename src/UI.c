#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include "UI.h"
#include "KH.h"
#include "CSDIEN.h"
#include "GIADIEN.h"
#include "cons.h"
#include "search.h"
#include "expense.h"
#include "invoice.h"

void clearScreen(void) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(hConsole, ' ', cellCount, (COORD){0, 0}, &count);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, (COORD){0, 0}, &count);
    SetConsoleCursorPosition(hConsole, (COORD){0, 0});
}

void displayMenu(int selected) {
    const char *options[] = {
        "1. Khoi tao file",
        "2. Thao tac tren file KH.BIN",
        "3. Thao tac tren file CSDIEN.BIN",
        "4. Thao tac tren file GIADIEN.BIN",
        "5. Tinh dien nang tieu thu",
        "6. Tim kiem & tra cuu",
        "7. Tinh tien dien theo ky",
        "8. In hoa don",
        "0. Thoat"
    };

    clearScreen();
    printf("===== CHUONG TRINH QUAN LY DIEN NANG =====\n\n");
    for (int i = 0; i < MAX_OPTIONS; i++) {
        if (i == selected) {
            printf("--> %s\n", options[i]);
        } else {
            printf("    %s\n", options[i]);
        }

    }
}

void displaySubMenu(int menuId, int selected) {
    const char *submenu_items[8][MAX_SUB_OPTIONS] = {
        {"1. Ghi vao KH.BIN", "2. Ghi vao CSDIEN.BIN", "3. Ghi vao GIADIEN.BIN"},
        {"1. Them thong tin khach hang", "2. Sua thong tin khach hang", "3. Xoa thong tin khach hang"},
        {"1. Them thong tin chi so dien", "2. Sua thong tin chi so dien", "3. Xoa thong tin chi so dien"},
        {"1. Them thong tin gia dien", "2. Sua thong tin gia dien", "3. Xoa thong tin gia dien"},
        {"", "", ""},
        {"", "", ""},
        {"", "", ""},
        {"", "", ""}
    };

    clearScreen();
    const char *titles[] = {
        "KHOI TAO FILE",
        "THAO TAC TREN FILE KH.BIN",
        "THAO TAC TREN FILE CSDIEN.BIN",
        "THAO TAC TREN FILE GIADIEN.BIN"
    };
    
    if (menuId >= 1 && menuId <= 4) {
        printf("===== %s =====\n\n", titles[menuId-1]);
        for (int i = 0; i < MAX_SUB_OPTIONS; i++) {
            if (submenu_items[menuId-1][i][0] != '\0') {
                if (i == selected) {
                    printf("--> %s\n", submenu_items[menuId-1][i]);
                } else {
                    printf("    %s\n", submenu_items[menuId-1][i]);
                }
            }
        }
        printf("\nNhan ESC de quay lai menu chinh");
    }
}

void processUserInput(void) {
    int selected = 0;
    int subSelected = 0;
    char key;
    char customer_id[20];
    int inSubmenu = 0;
    int currentMenu = 0;

    // Hiển thị menu lần đầu
    if (!inSubmenu) {
        displayMenu(selected);
    }

    while (1) {
        // Chờ phím nhấn
        key = _getch();

        // Xử lý phím mũi tên lên
        if (key == 72) { // Up arrow
            if (!inSubmenu && selected > 0) {
                selected--;
                displayMenu(selected);
                Sleep(50); // Thêm độ trễ nhỏ để menu mượt hơn
            } else if (inSubmenu && subSelected > 0) {
                subSelected--;
                displaySubMenu(currentMenu, subSelected);
                Sleep(50);
            }
            continue;
        } 

        // Xử lý phím mũi tên xuống
        if (key == 80) { // Down arrow
            if (!inSubmenu && selected < MAX_OPTIONS - 1) {
                selected++;
                displayMenu(selected);
                Sleep(50);
            } else if (inSubmenu && subSelected < MAX_SUB_OPTIONS - 1) {
                subSelected++;
                displaySubMenu(currentMenu, subSelected);
                Sleep(50);
            }
            continue;
        }

        // Xử lý phím ESC
        if (key == 27) { // ESC
            if (inSubmenu) {
                inSubmenu = 0;
                subSelected = 0;
                displayMenu(selected);
            }
            continue;
        }

        // Xử lý phím Enter
        if (key == 13) { // Enter
            if (!inSubmenu) {
                if (selected == MAX_OPTIONS - 1) { // Thoát
                    clearScreen();
                    printf("Ban co chac muon thoat? (Y/N): ");
                    char confirm = _getch();
                    if (confirm == 'Y' || confirm == 'y') {
                        clearScreen();
                        printf("Cam on ban da su dung chuong trinh!\n");
                        printf("Nhan phim bat ky de thoat...");
                        _getch();
                        return;
                    }
                    displayMenu(selected);
                    continue;
                }
                currentMenu = selected + 1;
                inSubmenu = 1;
                subSelected = 0;
                displaySubMenu(currentMenu, subSelected);
                continue;
            }

            // Xử lý chức năng
            clearScreen();
            switch (currentMenu) {
            case 1: // Khởi tạo file
                switch (subSelected) {
                case 0:
                    if (open_KH() == 0) printf("\nDa tao file KH.BIN\n");
                    break;
                case 1:
                    if (open_CSDIEN() == 0) printf("\nDa tao file CSDIEN.BIN\n");
                    break;
                case 2:
                    if (open_GIADIEN() == 0) printf("\nDa tao file GIADIEN.BIN\n");
                    break;
                }
                break;

            case 2: // Thao tác KH.BIN
                switch (subSelected) {
                case 0:
                    if (add_KH() == 0) printf("\nDa them khach hang\n");
                    break;
                case 1:
                    if (edit_KH() == 0) printf("\nDa sua thong tin khach hang\n");
                    break;
                case 2:
                    if (remove_KH() == 0) printf("\nDa xoa khach hang\n");
                    break;
                }
                break;

            case 3: // Thao tác CSDIEN.BIN
                switch (subSelected) {
                case 0:
                    if (add_CSDIEN() == 0) printf("\nDa them chi so dien\n");
                    break;
                case 1:
                    if (edit_CSDIEN() == 0) printf("\nDa sua chi so dien\n");
                    break;
                case 2:
                    if (remove_CSDIEN() == 0) printf("\nDa xoa chi so dien\n");
                    break;
                }
                break;

            case 4: // Thao tác GIADIEN.BIN
                switch (subSelected) {
                case 0:
                    if (add_GIADIEN() == 0) printf("\nDa them gia dien\n");
                    break;
                case 1:
                    if (edit_GIADIEN() == 0) printf("\nDa sua gia dien\n");
                    break;
                case 2:
                    if (remove_GIADIEN() == 0) printf("\nDa xoa gia dien\n");
                    break;
                }
                break;

            case 5: // Tính điện năng tiêu thụ
                if (cal_cons() == 0) 
                    printf("\nDa tinh xong dien nang tieu thu\n");
                break;

            case 6: // Tìm kiếm & tra cứu
                printf("Nhap ma khach hang: ");
                scanf("%s", customer_id);
                search_by_id(customer_id);
                break;

            case 7: // Tính tiền điện
                if (process_bills() == 0)
                    printf("\nDa tinh xong tien dien\n");
                break;

            case 8: // In hóa đơn
                full_invoice();
                break;
            }

            printf("\nNhan phim bat ky de tiep tuc...");
            _getch();
            
            // Quay lại menu chính
            inSubmenu = 0;
            subSelected = 0;
            displayMenu(selected);
        }

        Sleep(10); // Giảm tải CPU
    }
}