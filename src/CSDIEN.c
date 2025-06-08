#include "CSDIEN.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "date.h"


// Khởi tạo file chỉ số điện (tạo file rỗng)
ErrorCode open_CSDIEN(void) {
    FILE *fp = fopen("CSDIEN.BIN", "wb");
    if (fp == NULL) {
        printf("Cannot open CSDIEN.BIN\n");
        return ERR_FILE_OPEN;
    }
    fclose(fp);
    return SUCCESS;
}

// Thêm chỉ số điện mới
ErrorCode add_CSDIEN(void) {
    struct eindex new_eindex;
    char buffer[100];
    int day, month, year;
    Date prev_date;
    int prev_index;
    ErrorCode error;

    printf("Nhap ma so khach hang: ");
    // Kiểm tra độ dài của ID
    if (!safeInput(new_eindex.ID, sizeof(new_eindex.ID))) {
        printf("Loi nhap ID!\n");
        return ERR_INPUT_BUFFER;
    }

    // Kiểm tra ID có tồn tại trong KH.BIN
    error = isCustomerIdExists(new_eindex.ID);
    if (error == ERR_FILE_OPEN) {
        printf("Loi mo file KH.BIN!\n");
        return error;
    } else if (error == ERR_DATA_INVALID) {
        printf("Du lieu dau vao khong hop le!\n");
        return error;
    } else if (error == SUCCESS) {  // Không tìm thấy ID
        printf("Ma khach hang khong ton tai trong he thong!\n");
        return ERR_DATA_NOTFOUND;
    }

    printf("Nhap chi so dien: ");
    char input_buffer[100];
    if (!safeInput(input_buffer, sizeof(input_buffer))) {
        printf("Loi nhap chi so dien!\n");
        return ERR_INPUT_BUFFER;
    }
    if (sscanf(input_buffer, "%d", &new_eindex.index) != 1) {
        printf("Chi so dien khong hop le!\n");
        return ERR_INPUT_FORMAT;
    }

    // Kiểm tra giá trị chỉ số điện ngay sau khi nhập
    if (new_eindex.index < 0) {
        printf("Chi so dien khong duoc am!\n");
        return CSDIEN_ERR_INVALID_INDEX;
    }

    printf("Nhap ngay chot chi so dien(dd/mm/yyyy): ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap ngay!\n");
        return ERR_INPUT_BUFFER;
    }
    if (sscanf(buffer, "%d/%d/%d", &day, &month, &year) != 3) {
        printf("Dinh dang ngay khong hop le!\n");
        return ERR_INPUT_FORMAT;
    }
    new_eindex.closing_date.day = day;
    new_eindex.closing_date.month = month;
    new_eindex.closing_date.year = year;

    // Kiểm tra ngày hợp lệ
    if (!isValidDate(&new_eindex.closing_date)) {
        printf("Ngay khong hop le!\n");
        return ERR_DATA_INVALID;
    }

    printf("Nhap ky (1-12): ");
    if (!safeInput(input_buffer, sizeof(input_buffer))) {
        printf("Loi nhap ky!\n");
        return ERR_INPUT_BUFFER;
    }

    // Kiểm tra xem input có chứa ký tự không phải số không
    for(int i = 0; input_buffer[i] != '\0'; i++) {
        if(input_buffer[i] != ' ' && (input_buffer[i] < '0' || input_buffer[i] > '9')) {
            printf("Ky chi duoc chua cac chu so (1-12)!\n");
            return ERR_INPUT_FORMAT;
        }
    }

    if (sscanf(input_buffer, "%d", &new_eindex.term) != 1) {
        printf("Ky khong hop le!\n");
        return ERR_INPUT_FORMAT;
    }

    // Kiểm tra kỳ thu phí hợp lệ
    error = isValidTerm(new_eindex.term);
    if (error != SUCCESS) {
        return error;
    }

    // Kiểm tra chỉ số điện với kỳ trước
    error = getPreviousIndex(new_eindex.ID, new_eindex.term, &prev_index, &prev_date);
    if (error == SUCCESS) {
        // Chỉ kiểm tra với chỉ số kỳ trước nếu tìm thấy kỳ trước
        error = isValidIndex(new_eindex.index, prev_index);
        if (error != SUCCESS) {
            return error;
        }
        
        // Nếu có kỳ trước, kiểm tra ngày phải sau ngày kỳ trước
        if (compareDate(&new_eindex.closing_date, &prev_date) <= 0) {
            printf("Ngay chot chi so phai sau ngay chot ky truoc!\n");
            return ERR_DATA_INVALID;
        }
    } else if (error == ERR_DATA_NOTFOUND) {
        // Kỳ đầu tiên, chỉ cần kiểm tra chỉ số điện không âm
        if (new_eindex.index < 0) {
            printf("Chi so dien khong duoc am!\n");
            return CSDIEN_ERR_INVALID_INDEX;
        }
    } else {
        // Các lỗi khác
        return error;
    }

    // Kiểm tra trùng ID và kỳ
    error = isElectricIndexExists(new_eindex.ID, new_eindex.term);
    if (error == ERR_DATA_FOUND) {
        printf("Chi so dien cho ID va ky nay da ton tai!\n");
        return error;
    } else if (error != SUCCESS) {
        return error;
    }

    FILE *fp = fopen("CSDIEN.BIN", "ab");
    if (fp == NULL) {
        printf("Cannot open CSDIEN.BIN\n");
        return ERR_FILE_OPEN;
    }
    if (fwrite(&new_eindex, sizeof(struct eindex), 1, fp) != 1) {
        printf("Loi ghi file!\n");
        fclose(fp);
        return ERR_FILE_WRITE;
    }
    fclose(fp);

    printf("Them chi so dien thanh cong!\n");
    return SUCCESS;
}

// Sửa chỉ số điện theo ID và kỳ
ErrorCode edit_CSDIEN(void) {
    char edit_ID[20];
    int edit_term;
    char buffer[100];
    int day, month, year;
    struct eindex temp;
    long pos;
    Date prev_date;
    ErrorCode error;

    printf("Nhap ma so khach hang can sua: ");
    if (!safeInput(edit_ID, sizeof(edit_ID))) {
        printf("Loi nhap ID!\n");
        return ERR_INPUT_BUFFER;
    }

    // Kiểm tra ID có tồn tại trong KH.BIN
    error = isCustomerIdExists(edit_ID);
    if (error != ERR_DATA_FOUND) {
        printf("Ma khach hang khong ton tai trong he thong!\n");
        return ERR_DATA_NOTFOUND;
    }

    printf("Nhap ky can sua (1-12): ");
    if (scanf("%d", &edit_term) != 1) {
        printf("Loi nhap ky!\n");
        while (getchar() != '\n');
        return ERR_INPUT_FORMAT;
    }
    while (getchar() != '\n');

    // Kiểm tra kỳ thu phí hợp lệ
    error = isValidTerm(edit_term);
    if (error != SUCCESS) {
        return error;
    }

    FILE *fp = fopen("CSDIEN.BIN", "r+b");
    if (fp == NULL) {
        printf("Cannot open CSDIEN.BIN\n");
        return ERR_FILE_OPEN;
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
        return ERR_DATA_NOTFOUND;
    }

    printf("Nhap chi so dien moi: ");
    if (scanf("%d", &temp.index) != 1) {
        printf("Loi nhap chi so dien!\n");
        while (getchar() != '\n');
        fclose(fp);
        return ERR_INPUT_FORMAT;
    }
    while (getchar() != '\n');

    // Kiểm tra chỉ số điện với kỳ trước
    int prev_index;
    error = getPreviousIndex(edit_ID, edit_term, &prev_index, &prev_date);
    if (error != ERR_DATA_NOTFOUND && error != SUCCESS) {
        fclose(fp);
        return error;
    }
    
    error = isValidIndex(temp.index, prev_index);
    if (error != SUCCESS) {
        fclose(fp);
        return error;
    }

    printf("Nhap ngay chot chi so dien moi (dd/mm/yyyy): ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap ngay!\n");
        fclose(fp);
        return ERR_INPUT_BUFFER;
    }
    if (sscanf(buffer, "%d/%d/%d", &day, &month, &year) != 3) {
        printf("Dinh dang ngay khong hop le!\n");
        fclose(fp);
        return ERR_INPUT_FORMAT;
    }
    temp.closing_date.day = day;
    temp.closing_date.month = month;
    temp.closing_date.year = year;

    // Kiểm tra ngày hợp lệ
    if (!isValidDate(&temp.closing_date)) {
        printf("Ngay khong hop le!\n");
        fclose(fp);
        return ERR_DATA_INVALID;
    }

    // Nếu có kỳ trước, kiểm tra ngày phải sau ngày kỳ trước
    if (error == SUCCESS && compareDate(&temp.closing_date, &prev_date) <= 0) {
        printf("Ngay chot chi so phai sau ngay chot ky truoc!\n");
        fclose(fp);
        return ERR_DATA_INVALID;
    }

    fseek(fp, pos, SEEK_SET);
    if (fwrite(&temp, sizeof(struct eindex), 1, fp) != 1) {
        printf("Loi ghi file!\n");
        fclose(fp);
        return ERR_FILE_WRITE;
    }

    fclose(fp);
    printf("Sua chi so dien thanh cong!\n");
    return SUCCESS;
}

// Xóa chỉ số điện theo ID và kỳ
ErrorCode remove_CSDIEN(void) {
    char remove_ID[20];
    int remove_term;
    struct eindex temp;

    printf("Nhap ma so khach hang can xoa: ");
    if (!safeInput(remove_ID, sizeof(remove_ID))) {
        printf("Loi nhap ID!\n");
        return ERR_INPUT_BUFFER;
    }

    // Kiểm tra ID có tồn tại trong KH.BIN
    ErrorCode error = isCustomerIdExists(remove_ID);
    if (error != ERR_DATA_FOUND) {
        printf("Ma khach hang khong ton tai trong he thong!\n");
        return ERR_DATA_NOTFOUND;
    }

    printf("Nhap ky can xoa (1-12): ");
    if (scanf("%d", &remove_term) != 1) {
        printf("Loi nhap ky!\n");
        while (getchar() != '\n');
        return ERR_INPUT_FORMAT;
    }
    while (getchar() != '\n');

    // Kiểm tra kỳ thu phí hợp lệ
    ErrorCode error_term = isValidTerm(remove_term);
    if (error_term != SUCCESS) {
        return error_term;
    }

    // Mở file để đọc
    FILE* fp = fopen("CSDIEN.BIN", "rb");
    if (fp == NULL) {
        printf("Khong the mo file CSDIEN.BIN de doc!\n");
        return ERR_FILE_OPEN;
    }

    // Tạo file tạm
    FILE* temp_fp = fopen("temp.bin", "wb");
    if (temp_fp == NULL) {
        printf("Loi tao file tam!\n");
        fclose(fp);
        return ERR_FILE_OPEN;
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
            return ERR_FILE_WRITE;
        }
    }

    fclose(fp);
    fclose(temp_fp);

    if (!found) {
        printf("Khong tim thay chi so dien!\n");
        remove("temp.bin");
        return ERR_DATA_NOTFOUND;
    }

    // Xóa file cũ và đổi tên file tạm
    if (remove("CSDIEN.BIN") != 0) {
        printf("Loi xoa file cu!\n");
        remove("temp.bin");
        return ERR_FILE_REMOVE;
    }
    if (rename("temp.bin", "CSDIEN.BIN") != 0) {
        printf("Loi doi ten file!\n");
        return ERR_FILE_RENAME;
    }

    printf("Xoa chi so dien thanh cong!\n");
    return SUCCESS;
}

// Xem thông tin chỉ số điện
ErrorCode view_CSDIEN(void) {
    struct eindex temp;
    FILE* fp = fopen("CSDIEN.BIN", "rb");
    if (fp == NULL) {
        printf("Khong mo duoc file CSDIEN.BIN\n");
        return ERR_FILE_OPEN;
    }

    FILE* txt_fp = fopen("csdien.txt", "w");
    if (txt_fp == NULL) {
        printf("Khong tao duoc file csdien.txt\n");
        fclose(fp);
        return ERR_FILE_OPEN;
    }

    // Đọc và ghi từng bản ghi
    int count = 0;
    fprintf(txt_fp, "DANH SACH THONG TIN CHI SO DIEN\n");
    while (fread(&temp, sizeof(struct eindex), 1, fp) == 1) {
        fprintf(txt_fp, "Ma khach hang(KHXXXXXX): %s\n", temp.ID);
        fprintf(txt_fp, "Chi so dien(kWh): %d\n", temp.index);
        fprintf(txt_fp, "Ngay chot chi so: %02d/%02d/%04d\n", 
                temp.closing_date.day, temp.closing_date.month, temp.closing_date.year);
        fprintf(txt_fp, "Ky thu phi: %d\n", temp.term);
        fprintf(txt_fp, "\n");  // Thêm dòng trống giữa các bản ghi
        count++;
    }

    // Thêm footer
    fprintf(txt_fp, "Tong so chi so dien: %d\n", count);

    fclose(fp);
    fclose(txt_fp);

    return SUCCESS;
}

// Lấy chỉ số điện của kỳ trước
ErrorCode getPreviousIndex(const char* id, int term, int* prev_index, Date* prev_date) {
    struct eindex temp;
    int prev_term = term - 1;
    if (prev_term < 1) {
        prev_term = 12;  // Quay lại tháng 12 năm trước
    }
    
    FILE* fp = fopen("CSDIEN.BIN", "rb");
    if (fp == NULL) {
        return ERR_FILE_OPEN;
    }

    while (fread(&temp, sizeof(struct eindex), 1, fp) == 1) {
        if (strcmp(temp.ID, id) == 0 && temp.term == prev_term) {
            if (prev_date != NULL) {
                *prev_date = temp.closing_date;
            }
            if (prev_index != NULL) {
                *prev_index = temp.index;
            }
            fclose(fp);
            return SUCCESS;
        }
    }

    fclose(fp);
    return ERR_DATA_NOTFOUND;
}

// Kiểm tra kỳ thu phí hợp lệ
ErrorCode isValidTerm(int term) {
    if (term < 1 || term > 12) {
        printf("Ky thu phi phai tu 1 den 12!\n");
        return CSDIEN_ERR_INVALID_TERM;
    }
    return SUCCESS;
}

// Kiểm tra chỉ số điện hợp lệ
ErrorCode isValidIndex(int new_index, int prev_index) {
    if (prev_index >= 0 && new_index < prev_index) {
        printf("Chi so dien moi phai lon hon hoac bang chi so ky truoc (%d)!\n", prev_index);
        return CSDIEN_ERR_INDEX_SMALLER;
    }
    return SUCCESS;
}

// Kiểm tra trùng lặp chỉ số điện (mã KH + kỳ)
ErrorCode isElectricIndexExists(const char* id, int term) {
    struct eindex temp;
    FILE* fp = fopen("CSDIEN.BIN", "rb");
    if (fp == NULL) {
        return ERR_FILE_OPEN;
    }

    while (fread(&temp, sizeof(struct eindex), 1, fp) == 1) {
        if (strcmp(temp.ID, id) == 0 && temp.term == term) {
            fclose(fp);
            return ERR_DATA_FOUND;
        }
    }

    fclose(fp);
    return SUCCESS;
}

