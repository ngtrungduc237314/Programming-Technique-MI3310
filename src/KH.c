#include "KH.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"

#define MAX_CUSTOMERS 1000000

// Goi ham nay duy nhat 1 lan de khoi tao file KH.BIN
ErrorCode open_KH(void) {
    FILE *KH = fopen("KH.BIN", "wb");
    if (KH == NULL) {
        printf("Khong mo duoc KH.BIN\n");
        return ERR_FILE_OPEN;
    }
    fclose(KH);
    return SUCCESS;
}

// Them khach hang moi vao mang va file
ErrorCode add_KH(void) {
    struct customer new_customer;
    char buffer[100];

    printf("Nhap ma khach hang: ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap ma khach hang!\n");
        return ERR_INPUT_BUFFER;
    }

    if (!isValidCustomerId(buffer)) {
        printf("Ma khach hang khong hop le!\n");
        return ERR_INPUT_FORMAT;
    }

    // Kiểm tra trùng lặp
    ErrorCode exists = isCustomerIdExists(buffer);
    if (exists == ERR_DATA_DUPLICATE) {
        printf("Ma khach hang da ton tai!\n");
        return ERR_DATA_DUPLICATE;
    } else if (exists != SUCCESS) {
        printf("Loi kiem tra ma khach hang!\n");
        return exists;
    }

    strcpy(new_customer.ID, buffer);

    printf("Nhap ten khach hang: ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap ten!\n");
        return ERR_INPUT_BUFFER;
    }
    if (!isValidLength(buffer, sizeof(new_customer.Name))) {
        printf("Ten khach hang qua dai!\n");
        return ERR_INPUT_LENGTH;
    }
    strcpy(new_customer.Name, buffer);

    printf("Nhap dia chi: ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap dia chi!\n");
        return ERR_INPUT_BUFFER;
    }
    if (!isValidLength(buffer, sizeof(new_customer.Address))) {
        printf("Dia chi qua dai!\n");
        return ERR_INPUT_LENGTH;
    }
    strcpy(new_customer.Address, buffer);

    printf("Nhap ma cong to: ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap ma cong to!\n");
        return ERR_INPUT_BUFFER;
    }

    if (!isValidMeterNumber(buffer)) {
        printf("Ma cong to khong dung dinh dang (CTXXXXXX)!\n");
        return ERR_INPUT_FORMAT;
    }

    // Kiểm tra trùng mã công tơ
    ErrorCode meterExists = isMeterNumberExists(buffer);
    if (meterExists == ERR_DATA_DUPLICATE) {
        printf("Ma cong to da duoc su dung!\n");
        return ERR_DATA_DUPLICATE;
    } else if (meterExists != SUCCESS) {
        printf("Loi kiem tra ma cong to!\n");
        return meterExists;
    }

    strcpy(new_customer.Meter, buffer);

    FILE* fp = fopen("KH.BIN", "ab");
    if (!fp) {
        printf("Khong the mo file KH.BIN!\n");
        return ERR_FILE_OPEN;
    }

    if (fwrite(&new_customer, sizeof(struct customer), 1, fp) != 1) {
        printf("Loi ghi file!\n");
        fclose(fp);
        return ERR_FILE_WRITE;
    }

    fclose(fp);
    printf("Them khach hang thanh cong!\n");
    return SUCCESS;
}

// Sua thong tin khach hang theo ID
ErrorCode edit_KH() {
    char edit_ID[20];
    char buffer[100];
    struct customer temp;

    printf("Nhap ID khach hang can sua: ");
    if (!safeInput(edit_ID, sizeof(edit_ID))) {
        printf("Loi nhap ma khach hang!\n");
        return ERR_INPUT_BUFFER;
    }

    if (!isValidCustomerId(edit_ID)) {
        printf("Ma khach hang khong hop le!\n");
        return ERR_INPUT_FORMAT;
    }

    FILE* fp = fopen("KH.BIN", "r+b");
    if (fp == NULL) {
        printf("Khong mo duoc file KH.BIN\n");
        return ERR_FILE_OPEN;
    }

    int found = 0;
    while (fread(&temp, sizeof(struct customer), 1, fp) == 1) {
        if (strcmp(temp.ID, edit_ID) == 0) {
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Khong tim thay khach hang!\n");
        fclose(fp);
        return ERR_DATA_NOTFOUND;
    }

    // Lưu lại thông tin cũ
    char old_id[20], old_name[50], old_address[100], old_meter[20];
    strcpy(old_id, temp.ID);
    strcpy(old_name, temp.Name);
    strcpy(old_address, temp.Address);
    strcpy(old_meter, temp.Meter);

    printf("Nhap ma khach hang moi (Enter de giu nguyen): ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap ma khach hang!\n");
        fclose(fp);
        return ERR_INPUT_BUFFER;
    }

    // Nếu nhập mã mới (không để trống)
    if (strlen(buffer) > 0) {
        // Kiểm tra format mã khách hàng mới
        if (!isValidCustomerId(buffer)) {
            printf("Ma khach hang khong hop le!\n");
            fclose(fp);
            return ERR_INPUT_FORMAT;
        }

        // Kiểm tra trùng mã khách hàng mới (trừ trường hợp giữ nguyên mã cũ)
        if (strcmp(buffer, old_id) != 0) {
            ErrorCode exists = isCustomerIdExists(buffer);
            if (exists == ERR_DATA_DUPLICATE) {
                printf("Ma khach hang da ton tai!\n");
                fclose(fp);
                return ERR_DATA_DUPLICATE;
            } else if (exists != SUCCESS) {
                printf("Loi kiem tra ma khach hang!\n");
                fclose(fp);
                return exists;
            }
        }
        strcpy(temp.ID, buffer);
    }

    printf("Nhap ten moi (Enter de giu nguyen): ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap ten!\n");
        fclose(fp);
        return ERR_INPUT_BUFFER;
    }
    if (strlen(buffer) > 0) {
        if (!isValidLength(buffer, sizeof(temp.Name))) {
            printf("Ten khach hang qua dai!\n");
            fclose(fp);
            return ERR_INPUT_LENGTH;
        }
        strcpy(temp.Name, buffer);
    }

    printf("Nhap dia chi moi (Enter de giu nguyen): ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap dia chi!\n");
        fclose(fp);
        return ERR_INPUT_BUFFER;
    }
    if (strlen(buffer) > 0) {
        if (!isValidLength(buffer, sizeof(temp.Address))) {
            printf("Dia chi qua dai!\n");
            fclose(fp);
            return ERR_INPUT_LENGTH;
        }
        strcpy(temp.Address, buffer);
    }

    printf("Nhap ma cong to moi (Enter de giu nguyen): ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap ma cong to!\n");
        fclose(fp);
        return ERR_INPUT_BUFFER;
    }
    if (strlen(buffer) > 0) {
        // Kiểm tra format mã công tơ
        if (!isValidMeterNumber(buffer)) {
            printf("Ma cong to khong dung dinh dang (CTXXXXXX)!\n");
            fclose(fp);
            return ERR_INPUT_FORMAT;
        }

        // Kiểm tra trùng mã công tơ (trừ trường hợp giữ nguyên mã cũ)
        if (strcmp(buffer, old_meter) != 0) {
            ErrorCode meterExists = isMeterNumberExists(buffer);
            if (meterExists == ERR_DATA_DUPLICATE) {
                printf("Ma cong to da duoc su dung!\n");
                fclose(fp);
                return ERR_DATA_DUPLICATE;
            } else if (meterExists != SUCCESS) {
                printf("Loi kiem tra ma cong to!\n");
                fclose(fp);
                return meterExists;
            }
        }
        strcpy(temp.Meter, buffer);
    }

    // Ghi lại vào file
    fseek(fp, -(long)sizeof(struct customer), SEEK_CUR);
    if (fwrite(&temp, sizeof(struct customer), 1, fp) != 1) {
        printf("Loi cap nhat file!\n");
        fclose(fp);
        return ERR_FILE_WRITE;
    }

    fclose(fp);
    printf("Cap nhat thong tin thanh cong!\n");
    return SUCCESS;
}

// Xoa khach hang theo ID
ErrorCode remove_KH() {
    char del_ID[20];
    struct customer temp;
    int found = 0;

    printf("Nhap ID khach hang can xoa: ");
    if (!safeInput(del_ID, sizeof(del_ID))) {
        printf("Loi nhap ma khach hang!\n");
        return ERR_INPUT_BUFFER;
    }

    if (!isValidCustomerId(del_ID)) {
        printf("Ma khach hang khong hop le!\n");
        return ERR_INPUT_FORMAT;
    }

    FILE* fp = fopen("KH.BIN", "rb");
    if (fp == NULL) {
        printf("Khong mo duoc file KH.BIN\n");
        return ERR_FILE_OPEN;
    }

    FILE* temp_fp = fopen("KH_temp.BIN", "wb");
    if (temp_fp == NULL) {
        printf("Khong tao duoc file tam!\n");
        fclose(fp);
        return ERR_FILE_OPEN;
    }

    while (fread(&temp, sizeof(struct customer), 1, fp) == 1) {
        if (strcmp(temp.ID, del_ID) == 0) {
            found = 1;
            continue;
        }
        if (fwrite(&temp, sizeof(struct customer), 1, temp_fp) != 1) {
            printf("Loi ghi file!\n");
            fclose(fp);
            fclose(temp_fp);
            remove("KH_temp.BIN");
            return ERR_FILE_WRITE;
        }
    }

    fclose(fp);
    fclose(temp_fp);

    if (!found) {
        remove("KH_temp.BIN");
        printf("Khong tim thay khach hang!\n");
        return ERR_DATA_NOTFOUND;
    }

    if (remove("KH.BIN") != 0) {
        printf("Khong the xoa file cu!\n");
        remove("KH_temp.BIN");
        return ERR_FILE_WRITE;
    }

    if (rename("KH_temp.BIN", "KH.BIN") != 0) {
        printf("Khong the doi ten file!\n");
        return ERR_FILE_WRITE;
    }

    printf("Xoa khach hang thanh cong!\n");
    return SUCCESS;
}

// Xem thông tin khách hàng
ErrorCode view_KH(void) {
    struct customer temp;
    FILE* fp = fopen("KH.BIN", "rb");
    if (fp == NULL) {
        printf("Khong mo duoc file KH.BIN\n");
        return ERR_FILE_OPEN;
    }

    FILE* txt_fp = fopen("kh.txt", "w");
    if (txt_fp == NULL) {
        printf("Khong tao duoc file kh.txt\n");
        fclose(fp);
        return ERR_FILE_OPEN;
    }

    // Đọc và ghi từng bản ghi
    int count = 0;
    while (fread(&temp, sizeof(struct customer), 1, fp) == 1) {
        fprintf(txt_fp, "Ma khach hang: %s\n", temp.ID);
        fprintf(txt_fp, "Ten khach hang: %s\n", temp.Name);
        fprintf(txt_fp, "Dia chi: %s\n", temp.Address);
        fprintf(txt_fp, "Ma cong to: %s\n", temp.Meter);
        fprintf(txt_fp, "\n");  // Thêm dòng trống giữa các khách hàng
        count++;
    }

    // Thêm footer
    fprintf(txt_fp, "Tong so khach hang: %d\n", count);

    fclose(fp);
    fclose(txt_fp);

    return SUCCESS;
}