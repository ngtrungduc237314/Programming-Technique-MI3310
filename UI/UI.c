#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include "UI.h"
#include "../file_KH/KH.h"
#include "../file_CSDIEN/CSDIEN.h"
#include "../file_GIADIEN/GIADIEN.h"
#include "../cons/cons.h"
#include "../search/search.h"
#include "../expense/expense.h"
#include "../invoice/invoice.h"

void clearScreen() {
    printf("\033[H\033[J");  // ANSI escape code để xóa màn hình
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
    const char *submenu_items[8][3] = {
        {"1.1. Ghi vao KH.BIN", "1.2. Ghi vao CSDIEN.BIN", "1.3. Ghi vao GIADIEN.BIN"},
        {"2.1. Them", "2.2. Sua", "2.3. Xoa"},
        {"3.1. Them", "3.2. Sua", "3.3. Xoa"},
        {"4.1. Them", "4.2. Sua", "4.3. Xoa"},
        {"5.1. Tinh theo ky", "5.2. Tinh theo thang", "5.3. Tinh theo nam"},
        {"6.1. Tim theo ma KH", "6.2. Tim theo ten", "6.3. Tim theo dia chi"},
        {"7.1. Tinh tien", "7.2. Xem lich su", "7.3. Thong ke"},
        {"8.1. In man hinh", "8.2. In file", "8.3. In day du"}
    };

    clearScreen();
    printf("===== %s =====\n\n", submenu_items[menuId-1][0]);
    for (int i = 0; i < MAX_SUB_OPTIONS; i++) {
        if (i == selected) {
            printf("--> %s\n", submenu_items[menuId-1][i]);
        } else {
            printf("    %s\n", submenu_items[menuId-1][i]);
        }
    }
}

void processUserInput(void) {
    int selected = 0;
    int subSelected = 0;
    char key;
    char customer_id[20];
    int inSubmenu = 0;
    int currentMenu = 0;

    while (1) {
        do {
            if (!inSubmenu) {
                displayMenu(selected);
            } else {
                displaySubMenu(currentMenu, subSelected);
            }

            key = _getch();
            if (key == 72) { // Up arrow
                if (!inSubmenu && selected > 0) {
                    selected--;
                } else if (inSubmenu && subSelected > 0) {
                    subSelected--;
                }
            } 
            else if (key == 80) { // Down arrow
                if (!inSubmenu && selected < MAX_OPTIONS - 1) {
                    selected++;
                } else if (inSubmenu && subSelected < MAX_SUB_OPTIONS - 1) {
                    subSelected++;
                }
            }
            else if (key == 27) { // ESC
                if (inSubmenu) {
                    inSubmenu = 0;
                    subSelected = 0;
                    continue;
                }
            }
            else if (key == 13) { // Enter
                break;
            }
        } while (1);

        if (!inSubmenu) {
            if (selected == MAX_OPTIONS - 1) { // Thoát
                printf("Ban co chac muon thoat? (Y/N): ");
                char confirm = _getch();
                if (confirm == 'Y' || confirm == 'y') {
                    clearScreen();
                    printf("Cam on ban da su dung chuong trinh!\n");
                    return;
                }
                continue;
            }
            currentMenu = selected + 1;
            inSubmenu = 1;
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
            if (tinh_dien_nang_tieu_thu() == 0) 
                printf("\nDa tinh xong dien nang tieu thu\n");
            break;

        case 6: // Tìm kiếm & tra cứu
            printf("Nhap ma khach hang: ");
            scanf("%s", customer_id);
            search(customer_id);
            break;

        case 7: // Tính tiền điện
            if (cal_expense() == 0)
                printf("\nDa tinh xong tien dien\n");
            break;

        case 8: // In hóa đơn
            printf("Nhap ma khach hang: ");
            scanf("%s", customer_id);
            print_invoice(customer_id);
            break;
        }

        printf("\nNhan phim bat ky de tiep tuc...");
        _getch();
        inSubmenu = 0;
        subSelected = 0;
    }
}