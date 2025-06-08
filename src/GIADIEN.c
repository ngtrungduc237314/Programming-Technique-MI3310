#include "GIADIEN.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"


// Khởi tạo file giá điện với các bậc thực tế
ErrorCode open_GIADIEN(void) {
    struct tariff default_tariffs[MAX_TARIFF] = {
        {1, 0,   50,   1.984f},
        {2, 51,  100,  2.050f},
        {3, 101, 200,  2.380f},
        {4, 201, 300,  2.998f},
        {5, 301, 400,  3.350f},
        {6, 401, 1000000, 3.460f}
    };

    // Kiểm tra tính hợp lệ của dữ liệu mặc định
    ErrorCode error;
    for (int i = 0; i < MAX_TARIFF; i++) {
        error = isValidTariff(&default_tariffs[i]);
        if (error != SUCCESS) {
            printf("Du lieu mac dinh khong hop le tai bac %d!\n", i + 1);
            return error;
        }
    }

    // Kiểm tra tính liên tục
    error = checkTariffContinuity(default_tariffs, MAX_TARIFF);
    if (error != SUCCESS) {
        return error;
    }

    FILE *fp = fopen("GIADIEN.BIN", "wb");
    if (fp == NULL) {
        printf("Khong mo duoc GIADIEN.BIN\n");
        return ERR_FILE_OPEN;
    }

    if (fwrite(default_tariffs, sizeof(struct tariff), MAX_TARIFF, fp) != MAX_TARIFF) {
        printf("Loi ghi file!\n");
        fclose(fp);
        return ERR_FILE_WRITE;
    }

    fclose(fp);
    //printf("Da khoi tao file GIADIEN.BIN voi cac bac gia dien thuc te.\n");
    return SUCCESS;
}

// Thêm bậc giá điện mới
ErrorCode add_GIADIEN(void) {
    struct tariff tariffs[MAX_TARIFF];
    struct tariff new_tariff;
    int count = 0;

    // Đọc dữ liệu hiện có
    FILE *fp = fopen("GIADIEN.BIN", "rb");
    if (fp == NULL) {
        printf("Khong mo duoc GIADIEN.BIN\n");
        return ERR_FILE_OPEN;
    }

    count = fread(tariffs, sizeof(struct tariff), MAX_TARIFF, fp);
    fclose(fp);

    if (count >= MAX_TARIFF) {
        printf("Da dat so bac gia dien toi da (%d bac)!\n", MAX_TARIFF);
        return GIADIEN_ERR_DATA_FULL;
    }

    // Nhập thông tin bậc giá điện mới
    printf("Nhap bac gia dien moi (1-%d): ", MAX_TARIFF);
    if (scanf("%d", &new_tariff.level) != 1) {
        printf("Loi nhap bac!\n");
        while (getchar() != '\n');
        return ERR_INPUT_FORMAT;
    }
    while (getchar() != '\n');

    // Kiểm tra bậc đã tồn tại chưa
    for (int i = 0; i < count; i++) {
        if (tariffs[i].level == new_tariff.level) {
            printf("Bac gia dien da ton tai!\n");
            return GIADIEN_ERR_DATA_DUPLICATE;
        }
    }

    printf("Nhap chi so duoi: ");
    if (scanf("%d", &new_tariff.usage_bot) != 1) {
        printf("Loi nhap chi so duoi!\n");
        while (getchar() != '\n');
        return ERR_INPUT_FORMAT;
    }
    while (getchar() != '\n');

    printf("Nhap chi so tren: ");
    if (scanf("%d", &new_tariff.usage_top) != 1) {
        printf("Loi nhap chi so tren!\n");
        while (getchar() != '\n');
        return ERR_INPUT_FORMAT;
    }
    while (getchar() != '\n');

    printf("Nhap gia: ");
    if (scanf("%f", &new_tariff.price) != 1) {
        printf("Loi nhap gia!\n");
        while (getchar() != '\n');
        return ERR_INPUT_FORMAT;
    }
    while (getchar() != '\n');

    // Kiểm tra tính hợp lệ của dữ liệu mới
    ErrorCode error = isValidTariff(&new_tariff);
    if (error != SUCCESS) {
        return error;
    }

    // Thêm bậc mới vào mảng và sắp xếp theo level
    tariffs[count] = new_tariff;
    count++;
    
    // Sắp xếp các bậc theo thứ tự tăng dần
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (tariffs[i].level > tariffs[j].level) {
                struct tariff temp = tariffs[i];
                tariffs[i] = tariffs[j];
                tariffs[j] = temp;
            }
        }
    }

    // Kiểm tra tính liên tục sau khi thêm
    error = checkTariffContinuity(tariffs, count);
    if (error != SUCCESS) {
        return error;
    }

    // Ghi lại toàn bộ dữ liệu
    fp = fopen("GIADIEN.BIN", "wb");
    if (fp == NULL) {
        printf("Khong mo duoc GIADIEN.BIN\n");
        return ERR_FILE_OPEN;
    }

    if (fwrite(tariffs, sizeof(struct tariff), count, fp) != count) {
        printf("Loi ghi file!\n");
        fclose(fp);
        return ERR_FILE_WRITE;
    }

    fclose(fp);
    printf("Them bac gia dien thanh cong!\n");
    return SUCCESS;
}

// Sửa bậc giá điện theo level
ErrorCode edit_GIADIEN(void) {
    struct tariff tariffs[MAX_TARIFF];
    struct tariff temp;
    int edit_level;
    ErrorCode error;

    // Đọc toàn bộ dữ liệu hiện tại
    FILE *fp = fopen("GIADIEN.BIN", "rb");
    if (fp == NULL) {
        printf("Khong mo duoc GIADIEN.BIN\n");
        return ERR_FILE_OPEN;
    }

    if (fread(tariffs, sizeof(struct tariff), MAX_TARIFF, fp) != MAX_TARIFF) {
        printf("Loi doc file!\n");
        fclose(fp);
        return ERR_FILE_READ;
    }
    fclose(fp);

    printf("Nhap bac can sua (1-%d): ", MAX_TARIFF);
    if (scanf("%d", &edit_level) != 1) {
        printf("Loi nhap bac!\n");
        while (getchar() != '\n');
        return ERR_INPUT_FORMAT;
    }
    while (getchar() != '\n');

    if (edit_level < 1 || edit_level > MAX_TARIFF) {
        printf("Bac gia dien khong hop le!\n");
        return GIADIEN_ERR_INVALID_LEVEL;
    }

    // Tìm và sửa bậc giá điện
    int found = 0;
    for (int i = 0; i < MAX_TARIFF; i++) {
        if (tariffs[i].level == edit_level) {
            temp = tariffs[i];  // Lưu lại để phục hồi nếu dữ liệu mới không hợp lệ

            printf("Nhap chi so duoi moi: ");
            if (scanf("%d", &tariffs[i].usage_bot) != 1) {
                printf("Loi nhap chi so duoi!\n");
                while (getchar() != '\n');
                return ERR_INPUT_FORMAT;
            }
            while (getchar() != '\n');

            printf("Nhap chi so tren moi: ");
            if (scanf("%d", &tariffs[i].usage_top) != 1) {
                printf("Loi nhap chi so tren!\n");
                while (getchar() != '\n');
                return ERR_INPUT_FORMAT;
            }
            while (getchar() != '\n');

            printf("Nhap gia moi: ");
            if (scanf("%f", &tariffs[i].price) != 1) {
                printf("Loi nhap gia!\n");
                while (getchar() != '\n');
                return ERR_INPUT_FORMAT;
            }
            while (getchar() != '\n');

            // Kiểm tra tính hợp lệ của dữ liệu mới
            error = isValidTariff(&tariffs[i]);
            if (error != SUCCESS) {
                tariffs[i] = temp;  // Phục hồi dữ liệu cũ
                return error;
            }

            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Khong tim thay bac gia dien!\n");
        return GIADIEN_ERR_DATA_NOTFOUND;
    }

    // Kiểm tra tính liên tục sau khi sửa
    error = checkTariffContinuity(tariffs, MAX_TARIFF);
    if (error != SUCCESS) {
        return error;
    }

    // Ghi lại toàn bộ dữ liệu
    fp = fopen("GIADIEN.BIN", "wb");
    if (fp == NULL) {
        printf("Khong mo duoc GIADIEN.BIN\n");
        return ERR_FILE_OPEN;
    }

    if (fwrite(tariffs, sizeof(struct tariff), MAX_TARIFF, fp) != MAX_TARIFF) {
        printf("Loi ghi file!\n");
        fclose(fp);
        return ERR_FILE_WRITE;
    }

    fclose(fp);
    printf("Sua bac gia dien thanh cong!\n");
    return SUCCESS;
}

// Xóa bậc giá điện theo level
ErrorCode remove_GIADIEN(void) {
    struct tariff tariffs[MAX_TARIFF];
    int count = 0;
    int remove_level;

    // Đọc dữ liệu hiện có
    FILE *fp = fopen("GIADIEN.BIN", "rb");
    if (fp == NULL) {
        printf("Khong mo duoc GIADIEN.BIN\n");
        return ERR_FILE_OPEN;
    }

    count = fread(tariffs, sizeof(struct tariff), MAX_TARIFF, fp);
    fclose(fp);
    
    /*Cho phep xoa bac gia dien cuoi cung
    if (count <= 1) {
        printf("Khong the xoa bac gia dien cuoi cung!\n");
        return ERR_DATA_INVALID;
    }
    */
    
    printf("Nhap bac can xoa (1-%d): ", MAX_TARIFF);
    if (scanf("%d", &remove_level) != 1) {
        printf("Loi nhap bac!\n");
        while (getchar() != '\n');
        return ERR_INPUT_FORMAT;
    }
    while (getchar() != '\n');

    // Tìm và xóa bậc giá điện
    int found = 0;
    int new_count = 0;
    struct tariff new_tariffs[MAX_TARIFF];

    for (int i = 0; i < count; i++) {
        if (tariffs[i].level != remove_level) {
            new_tariffs[new_count] = tariffs[i];
            new_count++;
        } else {
            found = 1;
        }
    }

    if (!found) {
        printf("Khong tim thay bac gia dien!\n");
        return GIADIEN_ERR_DATA_NOTFOUND;
    }

    // Kiểm tra tính liên tục sau khi xóa
    ErrorCode error = checkTariffContinuity(new_tariffs, new_count);
    if (error != SUCCESS) {
        return error;
    }

    // Ghi lại toàn bộ dữ liệu
    fp = fopen("GIADIEN.BIN", "wb");
    if (fp == NULL) {
        printf("Khong mo duoc GIADIEN.BIN\n");
        return ERR_FILE_OPEN;
    }

    if (fwrite(new_tariffs, sizeof(struct tariff), new_count, fp) != new_count) {
        printf("Loi ghi file!\n");
        fclose(fp);
        return ERR_FILE_WRITE;
    }

    fclose(fp);
    printf("Xoa bac gia dien thanh cong!\n");
    return SUCCESS;
}

// Xem thông tin bảng giá điện
ErrorCode view_GIADIEN(void) {
    struct tariff tariffs[MAX_TARIFF];
    int count = 0;

    // Đọc dữ liệu từ file nhị phân
    FILE *fp = fopen("GIADIEN.BIN", "rb");
    if (fp == NULL) {
        printf("Khong mo duoc GIADIEN.BIN\n");
        return ERR_FILE_OPEN;
    }

    count = fread(tariffs, sizeof(struct tariff), MAX_TARIFF, fp);
    fclose(fp);

    // Mở file text để ghi
    fp = fopen("giadien.txt", "w");
    if (fp == NULL) {
        printf("Khong tao duoc giadien.txt\n");
        return ERR_FILE_OPEN;
    }

    // Ghi tiêu đề
    fprintf(fp, "BANG GIA DIEN THEO BAC THANG\n");

    // Ghi dữ liệu từng bậc
    for (int i = 0; i < count; i++) {
        fprintf(fp, "Bac: %d\n", tariffs[i].level);
        fprintf(fp, "Muc su dung(kWh): %d - %d\n", 
                tariffs[i].usage_bot, 
                tariffs[i].usage_top);
        fprintf(fp, "Don gia(VND): %.3f\n", tariffs[i].price);
    }

    fclose(fp);
    return SUCCESS;
}

// Kiểm tra tính hợp lệ của bậc giá điện
ErrorCode isValidTariff(const struct tariff* t) {
    if (t->level < 1 || t->level > MAX_TARIFF) {
        printf("Bac gia dien phai tu 1 den %d!\n", MAX_TARIFF);
        return GIADIEN_ERR_INVALID_LEVEL;
    }
    if (t->usage_bot < 0 || t->usage_top < 0) {
        printf("Chi so dien nang tieu thu khong duoc am!\n");
        return GIADIEN_ERR_INVALID_RANGE;
    }
    if (t->usage_bot >= t->usage_top) {
        printf("Chi so duoi phai nho hon chi so tren!\n");
        return GIADIEN_ERR_INVALID_RANGE;
    }
    if (t->price <= 0) {
        printf("Don gia dien phai lon hon 0!\n");
        return GIADIEN_ERR_INVALID_PRICE;
    }
    return SUCCESS;
}

// Kiểm tra tính liên tục của các bậc
ErrorCode checkTariffContinuity(const struct tariff* tariffs, int count) {
    for (int i = 0; i < count - 1; i++) {
        if (tariffs[i].usage_top + 1 != tariffs[i + 1].usage_bot) {
            printf("Cac bac gia dien phai lien tuc!\n");
            printf("Bac %d ket thuc tai %d nhung bac %d bat dau tai %d\n",
                   tariffs[i].level, tariffs[i].usage_top,
                   tariffs[i + 1].level, tariffs[i + 1].usage_bot);
            return GIADIEN_ERR_INVALID_RANGE;
        }
    }
    return SUCCESS;
}