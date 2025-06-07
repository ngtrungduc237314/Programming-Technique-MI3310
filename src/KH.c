#include "KH.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"

#define MAX_CUSTOMERS 1000000

// Goi ham nay duy nhat 1 lan de khoi tao file KH.BIN
int open_KH(void) {
    FILE *KH = fopen("KH.BIN", "wb");
    if (KH == NULL) {
        printf("Khong mo duoc KH.BIN\n");
        return -1;
    }
    fclose(KH);
    return 0;
}

// Them khach hang moi vao mang va file
int add_KH(void) {
    struct customer new_customer;
    char buffer[100];

    // Lần 1: Kiểm tra input an toàn
    printf("Nhap ma khach hang: ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap ma khach hang!\n");
        return -1;
    }

    if (!isValidCustomerId(buffer)) {
        printf("Ma khach hang khong hop le!\n");
        return -1;
    }

    // Kiểm tra trùng lặp
    int exists = isCustomerIdExists(buffer);
    if (exists == 1) {
        printf("Ma khach hang da ton tai!\n");
        return -1;
    } else if (exists == -1) {
        printf("Loi kiem tra ma khach hang!\n");
        return -1;
    }

    //Copy ma khach hang vao mang
    strcpy(new_customer.ID, buffer);

    //Kiem tra ten khach hang nhap vao co hop le hay khong
    printf("Nhap ten khach hang: ");
    if (!safeInput(buffer, sizeof(buffer)) || !isValidLength(buffer, sizeof(new_customer.Name))) {
        printf("Ten khach hang khong hop le!\n");
        return -1;
    }
    strcpy(new_customer.Name, buffer);

    printf("Nhap dia chi: ");
    if (!safeInput(buffer, sizeof(buffer)) || !isValidLength(buffer, sizeof(new_customer.Address))) {
        printf("Dia chi khong hop le!\n");
        return -1;
    }
    strcpy(new_customer.Address, buffer);

    printf("Nhap ma cong to: ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap ma cong to!\n");
        return -1;
    }

    if (!isValidMeterNumber(buffer)) {
        printf("Ma cong to khong dung dinh dang (CTXXXXXX)!\n");
        return -1;
    }

    // Kiểm tra trùng mã công tơ
    int meterExists = isMeterNumberExists(buffer);
    if (meterExists == 1) {
        printf("Ma cong to da duoc su dung!\n");
        return -1;
    } else if (meterExists == -1) {
        printf("Loi kiem tra ma cong to!\n");
        return -1;
    }

    strcpy(new_customer.Meter, buffer);

    FILE* fp = fopen("KH.BIN", "ab");
    if (!fp) {
        printf("Khong the mo file KH.BIN!\n");
        return -1;
    }

    if (fwrite(&new_customer, sizeof(struct customer), 1, fp) != 1) {
        printf("Loi ghi file!\n");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    printf("Them khach hang thanh cong!\n");
    return 0;
}

// Sua thong tin khach hang theo ID
int edit_KH() {
    char edit_ID[20];
    char buffer[100];
    struct customer temp;

    printf("Nhap ID khach hang can sua: ");
    if (!safeInput(edit_ID, sizeof(edit_ID))) {
        printf("Loi nhap ma khach hang!\n");
        return -1;
    }

    // Kiểm tra format mã khách hàng cần sửa
    if (!isValidCustomerId(edit_ID)) {
        printf("Ma khach hang khong hop le!\n");
        return -1;
    }

    FILE* fp = fopen("KH.BIN", "r+b");
    if (fp == NULL) {
        printf("Khong mo duoc file KH.BIN\n");
        return -1;
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
        return -1;
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
        return -1;
    }

    // Nếu nhập mã mới (không để trống)
    if (strlen(buffer) > 0) {
        // Kiểm tra format mã khách hàng mới
        if (!isValidCustomerId(buffer)) {
            printf("Ma khach hang khong hop le!\n");
            fclose(fp);
            return -1;
        }

        // Kiểm tra trùng mã khách hàng mới (trừ trường hợp giữ nguyên mã cũ)
        if (strcmp(buffer, old_id) != 0) {
            int exists = isCustomerIdExists(buffer);
            if (exists == 1) {
                printf("Ma khach hang da ton tai!\n");
                fclose(fp);
                return -1;
            } else if (exists == -1) {
                printf("Loi kiem tra ma khach hang!\n");
                fclose(fp);
                return -1;
            }
        }
        strcpy(temp.ID, buffer);
    }

    printf("Nhap ten moi (Enter de giu nguyen): ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap ten!\n");
        fclose(fp);
        return -1;
    }
    if (strlen(buffer) > 0) {
        if (!isValidLength(buffer, sizeof(temp.Name))) {
            printf("Ten khach hang qua dai!\n");
            fclose(fp);
            return -1;
        }
        strcpy(temp.Name, buffer);
    }

    printf("Nhap dia chi moi (Enter de giu nguyen): ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap dia chi!\n");
        fclose(fp);
        return -1;
    }
    if (strlen(buffer) > 0) {
        if (!isValidLength(buffer, sizeof(temp.Address))) {
            printf("Dia chi qua dai!\n");
            fclose(fp);
            return -1;
        }
        strcpy(temp.Address, buffer);
    }

    printf("Nhap ma cong to moi (Enter de giu nguyen): ");
    if (!safeInput(buffer, sizeof(buffer))) {
        printf("Loi nhap ma cong to!\n");
        fclose(fp);
        return -1;
    }
    if (strlen(buffer) > 0) {
        // Kiểm tra format mã công tơ
        if (!isValidMeterNumber(buffer)) {
            printf("Ma cong to khong dung dinh dang (CTXXXXXX)!\n");
            fclose(fp);
            return -1;
        }

        // Kiểm tra trùng mã công tơ (trừ trường hợp giữ nguyên mã cũ)
        if (strcmp(buffer, old_meter) != 0) {
            int meterExists = isMeterNumberExists(buffer);
            if (meterExists == 1) {
                printf("Ma cong to da duoc su dung!\n");
                fclose(fp);
                return -1;
            } else if (meterExists == -1) {
                printf("Loi kiem tra ma cong to!\n");
                fclose(fp);
                return -1;
            }
        }
        strcpy(temp.Meter, buffer);
    }

    // Ghi lại vào file
    fseek(fp, -(long)sizeof(struct customer), SEEK_CUR);
    if (fwrite(&temp, sizeof(struct customer), 1, fp) != 1) {
        printf("Loi ghi file!\n");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    printf("Sua thong tin thanh cong!\n");
    return 0;
}

// Xoa khach hang theo ID
int remove_KH() {
    char remove_ID[20];
    struct customer temp;
    long file_size;
    int found = 0;

    printf("Nhap ID khach hang can xoa: ");
    if (!safeInput(remove_ID, sizeof(remove_ID))) {
        printf("Loi nhap ma khach hang!\n");
        return -1;
    }

    // Kiểm tra format mã khách hàng
    if (!isValidCustomerId(remove_ID)) {
        printf("Ma khach hang khong hop le!\n");
        return -1;
    }

    // Mở file để đọc
    FILE* fp = fopen("KH.BIN", "rb");
    if (fp == NULL) {
        printf("Khong mo duoc file KH.BIN\n");
        return -1;
    }

    // Tạo file tạm
    FILE* temp_fp = fopen("temp.bin", "wb");
    if (temp_fp == NULL) {
        printf("Loi tao file tam!\n");
        fclose(fp);
        return -1;
    }

    // Đọc từng bản ghi và copy sang file tạm, bỏ qua bản ghi cần xóa
    while (fread(&temp, sizeof(struct customer), 1, fp) == 1) {
        if (strcmp(temp.ID, remove_ID) == 0) {
            found = 1;
            continue;
        }
        if (fwrite(&temp, sizeof(struct customer), 1, temp_fp) != 1) {
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
        printf("Khong tim thay khach hang!\n");
        remove("temp.bin");
        return -1;
    }

    // Xóa file cũ và đổi tên file tạm
    if (remove("KH.BIN") != 0) {
        printf("Loi xoa file cu!\n");
        remove("temp.bin");
        return -1;
    }
    if (rename("temp.bin", "KH.BIN") != 0) {
        printf("Loi doi ten file!\n");
        return -1;
    }

    printf("Xoa khach hang thanh cong!\n");
    return 0;
}