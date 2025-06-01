#include "../file_CSDIEN/CSDIEN.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/input.h"
#include "../utils/date.h"

// Kiểm tra kỳ thu phí hợp lệ
static int isValidTerm(int term) {
    if (term < 1 || term > 12) {
        printf("Ky thu phi phai tu 1 den 12!\n");
        return 0;
    }
    return 1;
}

// Kiểm tra chỉ số điện hợp lệ
static int isValidIndex(int new_index, int prev_index) {
    if (new_index < 0) {
        printf("Chi so dien khong duoc am!\n");
        return 0;
    }
    if (prev_index >= 0 && new_index < prev_index) {
        printf("Chi so dien moi phai lon hon hoac bang chi so ky truoc (%d)!\n", prev_index);
        return 0;
    }
    return 1;
}

// Khởi tạo file chỉ số điện (tạo file rỗng)
int open_CSDIEN(void) {
    FILE *fp = fopen("CSDIEN.BIN", "wb");
    if (fp == NULL) {
        printf("Cannot open CSDIEN.BIN\n");
        return -1;
    }
    fclose(fp);
    return 0;
}

// Kiểm tra trùng lặp chỉ số điện (mã KH + kỳ)
int isElectricIndexExists(const char* id, int term) {
    struct eindex temp;
    FILE* fp = fopen("CSDIEN.BIN", "rb");
    if (fp == NULL) {
        return -1; // Lỗi mở file
    }

    while (fread(&temp, sizeof(struct eindex), 1, fp) == 1) {
        if (strcmp(temp.ID, id) == 0 && temp.term == term) {
            fclose(fp);
            return 1; // Tìm thấy
        }
    }

    fclose(fp);
    return 0; // Không tìm thấy
}

// Lấy chỉ số điện của kỳ trước
int getPreviousIndex(const char* id, int term, Date* prev_date) {
    struct eindex temp;
    int prev_term = term - 1;
    if (prev_term < 1) {
        prev_term = 12;  // Quay lại tháng 12 năm trước
    }
    
    FILE* fp = fopen("CSDIEN.BIN", "rb");
    if (fp == NULL) {
        return -1; // Lỗi mở file
    }

    while (fread(&temp, sizeof(struct eindex), 1, fp) == 1) {
        if (strcmp(temp.ID, id) == 0 && temp.term == prev_term) {
            if (prev_date != NULL) {
                *prev_date = temp.closing_date;
            }
            fclose(fp);
            return temp.index;
        }
    }

    fclose(fp);
    return -1; // Không tìm thấy kỳ trước
}

// Thêm chỉ số điện mới
int add_CSDIEN(void) {
    struct eindex new_eindex;
    char buffer[100];
    int day, month, year;
    Date prev_date;

    printf("Nhap ID: ");
    if (!safeInput(new_eindex.ID, sizeof(new_eindex.ID))) {
        printf("Loi nhap ID!\n");
        return -1;
    }

    // Kiểm tra ID có tồn tại trong KH.BIN
    if (!isCustomerIdExists(new_eindex.ID)) {
        printf("Ma khach hang khong ton tai trong he thong!\n");
        return -1;
    }

    printf("Nhap ky (1-12): ");
    if (scanf("%d", &new_eindex.term) != 1) {
        printf("Loi nhap ky!\n");
        while (getchar() != '\n');
        return -1;
    }
    while (getchar() != '\n');

    // Kiểm tra kỳ hợp lệ
    if (!isValidTerm(new_eindex.term)) {
        return -1;
    }

    printf("Nhap chi so: ");
    if (scanf("%d", &new_eindex.index) != 1) {
        printf("Loi nhap chi so dien!\n");
        while (getchar() != '\n');
        return -1;
    }
    while (getchar() != '\n');

    // Kiểm tra chỉ số điện với kỳ trước
    int prev_index = getPreviousIndex(new_eindex.ID, new_eindex.term, &prev_date);
    if (!isValidIndex(new_eindex.index, prev_index)) {
        return -1;
    }

    printf("Nhap ngay chot chi so (dd/mm/yyyy): ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap ngay!\n");
        return -1;
    }
    if (sscanf(buffer, "%d/%d/%d", &day, &month, &year) != 3) {
        printf("Dinh dang ngay khong hop le!\n");
        return -1;
    }
    new_eindex.closing_date.day = day;
    new_eindex.closing_date.month = month;
    new_eindex.closing_date.year = year;

    // Kiểm tra ngày hợp lệ
    if (!isValidDate(&new_eindex.closing_date)) {
        printf("Ngay khong hop le!\n");
        return -1;
    }

    // Nếu có kỳ trước, kiểm tra ngày phải sau ngày kỳ trước
    if (prev_index >= 0) {
        if (compareDate(&new_eindex.closing_date, &prev_date) <= 0) {
            printf("Ngay chot chi so phai sau ngay chot ky truoc!\n");
            return -1;
        }
    }

    // Kiểm tra trùng ID và kỳ
    if (isElectricIndexExists(new_eindex.ID, new_eindex.term)) {
        printf("Chi so dien cho ID va ky nay da ton tai!\n");
        return -1;
    }

    FILE *fp = fopen("CSDIEN.BIN", "ab");
    if (fp == NULL) {
        printf("Cannot open CSDIEN.BIN\n");
        return -1;
    }
    if (fwrite(&new_eindex, sizeof(struct eindex), 1, fp) != 1) {
        printf("Loi ghi file!\n");
        fclose(fp);
        return -1;
    }
    fclose(fp);

    printf("Them chi so dien thanh cong!\n");
    return 0;
}

// Sửa chỉ số điện theo ID và kỳ
int edit_CSDIEN(void) {
    char edit_ID[20];
    int edit_term;
    char buffer[100];
    int day, month, year;
    struct eindex temp;
    long pos;
    Date prev_date;

    printf("Nhap ID can sua: ");
    if (!safeInput(edit_ID, sizeof(edit_ID))) {
        printf("Loi nhap ID!\n");
        return -1;
    }

    // Kiểm tra ID có tồn tại trong KH.BIN
    if (!isCustomerIdExists(edit_ID)) {
        printf("Ma khach hang khong ton tai trong he thong!\n");
        return -1;
    }

    printf("Nhap ky can sua (1-12): ");
    if (scanf("%d", &edit_term) != 1) {
        printf("Loi nhap ky!\n");
        while (getchar() != '\n');
        return -1;
    }
    while (getchar() != '\n');

    // Kiểm tra kỳ hợp lệ
    if (!isValidTerm(edit_term)) {
        return -1;
    }

    FILE *fp = fopen("CSDIEN.BIN", "r+b");
    if (fp == NULL) {
        printf("Cannot open CSDIEN.BIN\n");
        return -1;
    }

    int found = 0;
    while (fread(&temp, sizeof(struct eindex), 1, fp) == 1) {
        if (strcmp(temp.ID, edit_ID) == 0 && temp.term == edit_term) {
            found = 1;
            pos = ftell(fp) - sizeof(struct eindex);
            break;
        }
    }

    if (!found) {
        printf("Khong tim thay chi so dien!\n");
        fclose(fp);
        return -1;
    }

    printf("Nhap chi so moi: ");
    if (scanf("%d", &temp.index) != 1) {
        printf("Loi nhap chi so dien!\n");
        while (getchar() != '\n');
        fclose(fp);
        return -1;
    }
    while (getchar() != '\n');

    // Kiểm tra chỉ số điện với kỳ trước
    int prev_index = getPreviousIndex(edit_ID, edit_term, &prev_date);
    if (!isValidIndex(temp.index, prev_index)) {
        fclose(fp);
        return -1;
    }

    printf("Nhap ngay chot moi (dd/mm/yyyy): ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap ngay!\n");
        fclose(fp);
        return -1;
    }
    if (sscanf(buffer, "%d/%d/%d", &day, &month, &year) != 3) {
        printf("Dinh dang ngay khong hop le!\n");
        fclose(fp);
        return -1;
    }
    temp.closing_date.day = day;
    temp.closing_date.month = month;
    temp.closing_date.year = year;

    // Kiểm tra ngày hợp lệ
    if (!isValidDate(&temp.closing_date)) {
        printf("Ngay khong hop le!\n");
        fclose(fp);
        return -1;
    }

    // Nếu có kỳ trước, kiểm tra ngày phải sau ngày kỳ trước
    if (prev_index >= 0) {
        if (compareDate(&temp.closing_date, &prev_date) <= 0) {
            printf("Ngay chot chi so phai sau ngay chot ky truoc!\n");
            fclose(fp);
            return -1;
        }
    }

    fseek(fp, pos, SEEK_SET);
    if (fwrite(&temp, sizeof(struct eindex), 1, fp) != 1) {
        printf("Loi ghi file!\n");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    printf("Sua chi so dien thanh cong!\n");
    return 0;
}

// Xóa chỉ số điện theo ID và kỳ
int remove_CSDIEN(void) {
    char remove_ID[20];
    int remove_term;
    struct eindex temp;

    printf("Nhap ID can xoa: ");
    if (!safeInput(remove_ID, sizeof(remove_ID))) {
        printf("Loi nhap ID!\n");
        return -1;
    }

    // Kiểm tra ID có tồn tại trong KH.BIN
    if (!isCustomerIdExists(remove_ID)) {
        printf("Ma khach hang khong ton tai trong he thong!\n");
        return -1;
    }

    printf("Nhap ky can xoa (1-12): ");
    if (scanf("%d", &remove_term) != 1) {
        printf("Loi nhap ky!\n");
        while (getchar() != '\n');
        return -1;
    }
    while (getchar() != '\n');

    // Kiểm tra kỳ hợp lệ
    if (!isValidTerm(remove_term)) {
        return -1;
    }

    // Mở file để đọc
    FILE* fp = fopen("CSDIEN.BIN", "rb");
    if (fp == NULL) {
        printf("Cannot open CSDIEN.BIN\n");
        return -1;
    }

    // Tạo file tạm
    FILE* temp_fp = fopen("temp.bin", "wb");
    if (temp_fp == NULL) {
        printf("Loi tao file tam!\n");
        fclose(fp);
        return -1;
    }

    int found = 0;
    // Đọc từng bản ghi và copy sang file tạm, bỏ qua bản ghi cần xóa
    while (fread(&temp, sizeof(struct eindex), 1, fp) == 1) {
        if (strcmp(temp.ID, remove_ID) == 0 && temp.term == remove_term) {
            found = 1;
            continue;
        }
        if (fwrite(&temp, sizeof(struct eindex), 1, temp_fp) != 1) {
            printf("Loi ghi file tam!\n");
            fclose(fp);
            fclose(temp_fp);
            remove("temp.bin");
            return -1;
        }
    }

    fclose(fp);
    fclose(temp_fp);

    if (!found) {
        printf("Khong tim thay chi so dien!\n");
        remove("temp.bin");
        return -1;
    }

    // Xóa file cũ và đổi tên file tạm
    if (remove("CSDIEN.BIN") != 0) {
        printf("Loi xoa file cu!\n");
        remove("temp.bin");
        return -1;
    }
    if (rename("temp.bin", "CSDIEN.BIN") != 0) {
        printf("Loi doi ten file!\n");
        return -1;
    }

    printf("Xoa chi so dien thanh cong!\n");
    return 0;
}
}