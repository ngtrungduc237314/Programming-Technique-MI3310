#include "GIADIEN.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"

//#define MAX_TARIFF 6

// Kiểm tra tính hợp lệ của bậc giá điện
static int isValidTariff(const struct tariff* t) {
    if (t->level < 1 || t->level > MAX_TARIFF) {
        printf("Bac gia dien phai tu 1 den %d!\n", MAX_TARIFF);
        return 0;
    }
    if (t->usage_bot < 0 || t->usage_top < 0) {
        printf("Chi so dien nang tieu thu khong duoc am!\n");
        return 0;
    }
    if (t->usage_bot >= t->usage_top) {
        printf("Chi so duoi phai nho hon chi so tren!\n");
        return 0;
    }
    if (t->price <= 0) {
        printf("Don gia dien phai lon hon 0!\n");
        return 0;
    }
    return 1;
}

// Kiểm tra tính liên tục của các bậc
static int checkTariffContinuity(const struct tariff* tariffs, int count) {
    for (int i = 0; i < count - 1; i++) {
        if (tariffs[i].usage_top + 1 != tariffs[i + 1].usage_bot) {
            printf("Cac bac gia dien phai lien tuc!\n");
            printf("Bac %d ket thuc tai %d nhung bac %d bat dau tai %d\n",
                   tariffs[i].level, tariffs[i].usage_top,
                   tariffs[i + 1].level, tariffs[i + 1].usage_bot);
            return 0;
        }
    }
    return 1;
}

// Khởi tạo file giá điện với các bậc thực tế
int open_GIADIEN(void) {
    struct tariff default_tariffs[MAX_TARIFF] = {
        {1, 0,   50,   1.984f},
        {2, 51,  100,  2.050f},
        {3, 101, 200,  2.380f},
        {4, 201, 300,  2.998f},
        {5, 301, 400,  3.350f},
        {6, 401, 1000000, 3.460f}
    };

    // Kiểm tra tính hợp lệ của dữ liệu mặc định
    for (int i = 0; i < MAX_TARIFF; i++) {
        if (!isValidTariff(&default_tariffs[i])) {
            printf("Du lieu mac dinh khong hop le tai bac %d!\n", i + 1);
            return -1;
        }
    }

    // Kiểm tra tính liên tục
    if (!checkTariffContinuity(default_tariffs, MAX_TARIFF)) {
        return -1;
    }

    FILE *fp = fopen("GIADIEN.BIN", "wb");
    if (fp == NULL) {
        printf("Khong mo duoc GIADIEN.BIN\n");
        return -1;
    }

    if (fwrite(default_tariffs, sizeof(struct tariff), MAX_TARIFF, fp) != MAX_TARIFF) {
        printf("Loi ghi file!\n");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    //printf("Da khoi tao file GIADIEN.BIN voi cac bac gia dien thuc te.\n");
    return 0;
}

// Thêm bậc giá điện mới (không khuyến khích vì đã cố định theo thực tế)
int add_GIADIEN(void) {
    printf("Cac bac gia dien da duoc co dinh theo thuc te, khong the them moi!\n");
    return -1;
}

// Sửa bậc giá điện theo level
int edit_GIADIEN(void) {
    struct tariff tariffs[MAX_TARIFF];
    struct tariff temp;
    int edit_level;

    // Đọc toàn bộ dữ liệu hiện tại
    FILE *fp = fopen("GIADIEN.BIN", "rb");
    if (fp == NULL) {
        printf("Khong mo duoc GIADIEN.BIN\n");
        return -1;
    }

    if (fread(tariffs, sizeof(struct tariff), MAX_TARIFF, fp) != MAX_TARIFF) {
        printf("Loi doc file!\n");
        fclose(fp);
        return -1;
    }
    fclose(fp);

    printf("Nhap bac can sua (1-%d): ", MAX_TARIFF);
    if (scanf("%d", &edit_level) != 1) {
        printf("Loi nhap bac!\n");
        while (getchar() != '\n');
        return -1;
    }
    while (getchar() != '\n');

    if (edit_level < 1 || edit_level > MAX_TARIFF) {
        printf("Bac gia dien khong hop le!\n");
        return -1;
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
                return -1;
            }
            while (getchar() != '\n');

            printf("Nhap chi so tren moi: ");
            if (scanf("%d", &tariffs[i].usage_top) != 1) {
                printf("Loi nhap chi so tren!\n");
                while (getchar() != '\n');
                return -1;
            }
            while (getchar() != '\n');

            printf("Nhap gia moi: ");
            if (scanf("%f", &tariffs[i].price) != 1) {
                printf("Loi nhap gia!\n");
                while (getchar() != '\n');
                return -1;
            }
            while (getchar() != '\n');

            // Kiểm tra tính hợp lệ của dữ liệu mới
            if (!isValidTariff(&tariffs[i])) {
                tariffs[i] = temp;  // Phục hồi dữ liệu cũ
                return -1;
            }

            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Khong tim thay bac gia dien!\n");
        return -1;
    }

    // Kiểm tra tính liên tục sau khi sửa
    if (!checkTariffContinuity(tariffs, MAX_TARIFF)) {
        return -1;
    }

    // Ghi lại toàn bộ dữ liệu
    fp = fopen("GIADIEN.BIN", "wb");
    if (fp == NULL) {
        printf("Khong mo duoc GIADIEN.BIN\n");
        return -1;
    }

    if (fwrite(tariffs, sizeof(struct tariff), MAX_TARIFF, fp) != MAX_TARIFF) {
        printf("Loi ghi file!\n");
        fclose(fp);
        return -1;
    }

    fclose(fp);
    printf("Sua bac gia dien thanh cong!\n");
    return 0;
}

// Xóa bậc giá điện theo level (không khuyến khích)
int remove_GIADIEN(void) {
    printf("Cac bac gia dien da duoc co dinh theo thuc te, khong the xoa!\n");
    return -1;
}