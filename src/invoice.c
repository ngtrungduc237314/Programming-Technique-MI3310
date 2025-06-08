#include "invoice.h"
#include "KH.h"
#include "CSDIEN.h"
#include "expense.h"
#include "input.h"
#include "date.h"
#include "GIADIEN.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "input.h"

// Ham chuyen so thanh chu (ho tro den 1 trieu)
static ErrorCode number_to_text(int n, char *buf) {
    if (buf == NULL) {
        return ERR_DATA_INVALID;
    }

    const char *ones[] = {"khong", "mot", "hai", "ba", "bon", "nam", "sau", "bay", "tam", "chin"};
    char temp[256] = "";
    
    if (n == 0) {
        strcpy(buf, "khong");
        return SUCCESS;
    }

    // Xử lý hàng trăm nghìn
    if (n >= 100000) {
        sprintf(temp + strlen(temp), "%s tram ", ones[n / 100000]);
        n %= 100000;
        
        // Thêm từ "không" nếu số hàng chục nghìn = 0 và có số hàng nghìn
        if ((n / 1000) % 10 == 0 && n >= 1000) {
            strcat(temp, "khong muoi ");
        }
    }

    // Xử lý hàng chục nghìn
    if (n >= 10000) {
        if ((n / 10000) == 1)
            strcat(temp, "muoi ");
        else
            sprintf(temp + strlen(temp), "%s muoi ", ones[n / 10000]);
        n %= 10000;
    }

    // Xử lý hàng nghìn
    if (n >= 1000) {
        if (n / 1000 == 5)
            strcat(temp, "lam ");
        else
            sprintf(temp + strlen(temp), "%s ", ones[n / 1000]);
        strcat(temp, "nghin ");
        n %= 1000;
        
        // Thêm từ "không trăm" nếu số hàng trăm = 0 và có số hàng chục hoặc đơn vị
        if (n > 0 && n < 100) {
            strcat(temp, "khong tram ");
        }
    }

    // Xử lý hàng trăm
    if (n >= 100) {
        sprintf(temp + strlen(temp), "%s tram ", ones[n / 100]);
        n %= 100;
        
        // Thêm từ "lẻ" nếu số hàng chục = 0 và có số hàng đơn vị
        if (n > 0 && n < 10) {
            strcat(temp, "le ");
        }
    }

    // Xử lý hàng chục
    if (n >= 10) {
        if (n / 10 == 1)
            strcat(temp, "muoi ");
        else
            sprintf(temp + strlen(temp), "%s muoi ", ones[n / 10]);
        n %= 10;
    }

    // Xử lý hàng đơn vị
    if (n > 0) {
        // Dùng "lăm" thay vì "năm" khi đứng sau chục
        if (n == 5 && strlen(temp) > 0)
            strcat(temp, "lam");
        else
            sprintf(temp + strlen(temp), "%s", ones[n]);
    }

    strcpy(buf, temp);
    return SUCCESS;
}

// Khởi tạo file hóa đơn
ErrorCode open_invoice(void) {
    FILE *fp = fopen("INVOICE.BIN", "wb");
    if (fp == NULL) {
        printf("Khong mo duoc file INVOICE.BIN\n");
        return ERR_FILE_OPEN;
    }
    fclose(fp);
    return SUCCESS;
}

// In hoa don ra man hinh
ErrorCode print_invoice(const struct invoice_record* inv) {
    if (inv == NULL) {
        return ERR_DATA_INVALID;
    }

    // In hoa don ra man hinh
    printf("========================================\n");
    printf("HOA DON TIEN DIEN\n");
    printf("Ma KH: %s\n", inv->ID);
    printf("Ten KH: %s\n", inv->Name);
    printf("Dia chi: %s\n", inv->Address);
    printf("Ma cong to: %s\n", inv->Meter);
    printf("Ky: %d\n", inv->term);
    printf("Tu ngay: %s\n", inv->from_date);
    printf("Den ngay: %s\n", inv->to_date);
    //printf("Chi so cu: %d\n", inv->prev_index);
    printf("Chi so moi: %d\n", inv->curr_index);
    printf("Dien nang tieu thu: %d kWh\n", inv->usage);
    printf("Tien dien: %.2f VND\n", inv->amount);
    printf("Thue VAT (10%%): %.2f VND\n", inv->tax);
    printf("Tong cong thanh toan: %.2f VND\n", inv->total);
    printf("So tien bang chu: %s dong.\n", inv->amount_text);
    printf("========================================\n\n");

    return SUCCESS;
}

// Lưu hóa đơn vào file
ErrorCode save_invoice(const struct invoice_record* inv) {
    if (inv == NULL) {
        return ERR_DATA_INVALID;
    }

    FILE* fp = fopen("INVOICE.BIN", "ab");
    if (fp == NULL) {
        printf("Khong mo duoc file INVOICE.BIN\n");
        return ERR_FILE_OPEN;
    }

    if (fwrite(inv, sizeof(struct invoice_record), 1, fp) != 1) {
        printf("Loi ghi file!\n");
        fclose(fp);
        return ERR_FILE_WRITE;
    }

    fclose(fp);
    return SUCCESS;
}

// Hàm nội bộ để lấy chỉ số điện từ file
static ErrorCode readCurrentIndex(const char* id, int term, int* current_index) {
    FILE* fp = fopen("CSDIEN.BIN", "rb");
    if (fp == NULL) {
        return ERR_FILE_OPEN;
    }

    struct eindex ei;
    int found = 0;
    while (fread(&ei, sizeof(struct eindex), 1, fp) == 1) {
        if (strcmp(ei.ID, id) == 0 && ei.term == term) {
            *current_index = ei.index;
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) {
        return ERR_DATA_NOTFOUND;
    }
    return SUCCESS;
}

// Lấy bảng giá điện theo bậc thang
static ErrorCode read_tariffs(struct tariff* tariffs, int* tariff_count) {
    if (!tariffs || !tariff_count) return ERR_DATA_INVALID;

    FILE* fp = fopen("GIADIEN.BIN", "rb");
    if (!fp) return ERR_FILE_OPEN;

    *tariff_count = 0;
    struct tariff t;
    while (fread(&t, sizeof(struct tariff), 1, fp) == 1 && *tariff_count < MAX_TARIFF) {
        tariffs[*tariff_count] = t;
        (*tariff_count)++;
    }

    fclose(fp);
    return SUCCESS;
}

// Tính toán hóa đơn
ErrorCode calculate_invoice(void) {
    struct invoice_record inv = {0};
    char customer_id[20];
    ErrorCode error;

    printf("Nhap ma khach hang: ");
    if (!safeInput(customer_id, sizeof(customer_id))) {
        printf("Loi nhap ma khach hang!\n");
        return ERR_INPUT_BUFFER;
    }

    // Kiểm tra mã khách hàng
    if (!isValidCustomerId(customer_id)) {
        printf("Ma khach hang khong hop le!\n");
        return ERR_INPUT_FORMAT;
    }

    // Đọc thông tin khách hàng từ file
    FILE* fp = fopen("KH.BIN", "rb");
    if (fp == NULL) {
        printf("Khong mo duoc file KH.BIN\n");
        return ERR_FILE_OPEN;
    }

    struct customer kh;
    int found = 0;
    while (fread(&kh, sizeof(struct customer), 1, fp) == 1) {
        if (strcmp(kh.ID, customer_id) == 0) {
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("Khong tim thay khach hang!\n");
        return ERR_DATA_NOTFOUND;
    }

    strcpy(inv.ID, kh.ID);
    strcpy(inv.Name, kh.Name);
    strcpy(inv.Address, kh.Address);
    strcpy(inv.Meter, kh.Meter);

    // Nhập kỳ thu phí
    printf("Nhap ky (1-12): ");
    if (scanf("%d", &inv.term) != 1) {
        printf("Loi nhap ky!\n");
        while (getchar() != '\n');
        return ERR_INPUT_FORMAT;
    }
    while (getchar() != '\n');

    if (inv.term < 1 || inv.term > 12) {
        printf("Ky thu phi khong hop le!\n");
        return ERR_DATA_INVALID;
    }

    // Lấy chỉ số điện kỳ này
    error = readCurrentIndex(inv.ID, inv.term, &inv.curr_index);
    if (error == ERR_DATA_NOTFOUND) {
        printf("Chua co chi so dien ky nay!\n");
        return INVOICE_ERR_NO_INDEX;
    } else if (error != SUCCESS) {
        return error;
    }

    // Lấy chỉ số điện kỳ trước
    error = getPreviousIndex(inv.ID, inv.term, &inv.prev_index, NULL);
    if (error != SUCCESS && error != ERR_DATA_NOTFOUND) {
        return error;
    }

    // Tính điện năng tiêu thụ
    inv.usage = inv.curr_index - inv.prev_index;

    // Lấy bảng giá điện theo bậc thang
    struct tariff tariffs[MAX_TARIFF];
    int tariff_count = 0;
    error = read_tariffs(tariffs, &tariff_count);
    if (error != SUCCESS) {
        return error;
    }

    if (tariff_count == 0) {
        printf("Khong co bang gia dien!\n");
        return INVOICE_ERR_NO_TARIFF;
    }

    // Tính tiền điện theo bậc thang
    inv.amount = calc_amount(inv.usage, tariffs, tariff_count);

    // Tính thuế và tổng tiền
    inv.tax = inv.amount * 0.1f;
    inv.total = inv.amount + inv.tax;

    // Chuyển số tiền thành chữ
    error = number_to_text((int)inv.total, inv.amount_text);
    if (error != SUCCESS) {
        return error;
    }

    // In và lưu hóa đơn
    error = print_invoice(&inv);
    if (error != SUCCESS) {
        return error;
    }

    return save_invoice(&inv);
}

// Xuất hóa đơn ra file
ErrorCode write_invoice_to_file(const char* customer_id, FILE* f) {
    if (customer_id == NULL || f == NULL) {
        return ERR_DATA_INVALID;
    }

    // Kiểm tra mã khách hàng
    if (!isValidCustomerId(customer_id)) {
        fprintf(f, "Ma khach hang khong hop le!\n");
        return ERR_INPUT_FORMAT;
    }

    // Đọc thông tin khách hàng từ file
    FILE* fp = fopen("KH.BIN", "rb");
    if (fp == NULL) {
        fprintf(f, "Khong mo duoc file KH.BIN\n");
        return ERR_FILE_OPEN;
    }

    struct customer kh;
    int found = 0;
    while (fread(&kh, sizeof(struct customer), 1, fp) == 1) {
        if (strcmp(kh.ID, customer_id) == 0) {
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found) {
        fprintf(f, "Khong tim thay khach hang!\n");
        return ERR_DATA_NOTFOUND;
    }

    struct invoice_record inv = {0};
    strcpy(inv.ID, kh.ID);
    strcpy(inv.Name, kh.Name);
    strcpy(inv.Address, kh.Address);
    strcpy(inv.Meter, kh.Meter);

    // Lấy kỳ thu phí hiện tại
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    inv.term = t->tm_mon + 1;

    // Lấy chỉ số điện kỳ này
    ErrorCode error = readCurrentIndex(inv.ID, inv.term, &inv.curr_index);
    if (error == ERR_DATA_NOTFOUND) {
        fprintf(f, "Chua co chi so dien ky nay!\n");
        return INVOICE_ERR_NO_INDEX;
    } else if (error != SUCCESS) {
        return error;
    }

    // Lấy chỉ số điện kỳ trước
    error = getPreviousIndex(inv.ID, inv.term, &inv.prev_index, NULL);
    if (error != SUCCESS && error != ERR_DATA_NOTFOUND) {
        return error;
    }

    // Tính điện năng tiêu thụ
    inv.usage = inv.curr_index - inv.prev_index;

    // Lấy bảng giá điện theo bậc thang
    struct tariff tariffs[MAX_TARIFF];
    int tariff_count = 0;
    error = read_tariffs(tariffs, &tariff_count);
    if (error != SUCCESS) {
        fprintf(f, "Loi doc bang gia dien!\n");
        return error;
    }

    if (tariff_count == 0) {
        fprintf(f, "Khong co bang gia dien!\n");
        return INVOICE_ERR_NO_TARIFF;
    }

    // Tính tiền điện theo bậc thang
    inv.amount = calc_amount(inv.usage, tariffs, tariff_count);

    // Tính thuế và tổng tiền
    inv.tax = inv.amount * 0.1f;
    inv.total = inv.amount + inv.tax;

    // Chuyển số tiền thành chữ
    error = number_to_text((int)inv.total, inv.amount_text);
    if (error != SUCCESS) {
        return error;
    }

    // In hoa don ra file
    fprintf(f, "==========================================================\n");
    fprintf(f, "CHUONG TRINH QUAN LY DIEN NANG - HOA DON TIEN DIEN\n");
    fprintf(f, "Thoi gian: %02d/%02d/%04d %02d:%02d:%02d\n", 
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec);
    fprintf(f, "==========================================================\n\n");
    fprintf(f, "HOA DON TIEN DIEN\n");
    fprintf(f, "Ma KH: %s\n", inv.ID);
    fprintf(f, "Ten KH: %s\n", inv.Name);
    fprintf(f, "Dia chi: %s\n", inv.Address);
    fprintf(f, "Ma cong to: %s\n", inv.Meter);
    fprintf(f, "Ky: %d\n", inv.term);
    fprintf(f, "Tu ngay: %s\n", inv.from_date);
    fprintf(f, "Den ngay: %s\n", inv.to_date);
    fprintf(f, "Chi so moi: %d\n", inv.curr_index);
    fprintf(f, "Dien nang tieu thu: %d kWh\n", inv.usage);
    fprintf(f, "Tien dien: %.2f VND\n", inv.amount);
    fprintf(f, "Thue VAT (10%%): %.2f VND\n", inv.tax);
    fprintf(f, "Tong cong thanh toan: %.2f VND\n", inv.total);
    fprintf(f, "So tien bang chu: %s dong.\n", inv.amount_text);
    fprintf(f, "==========================================================\n\n");

    return SUCCESS;
}

// xử lý hóa đơn đầy đủ
ErrorCode full_invoice(void) {
    char customer_id[20];
    ErrorCode error;

    printf("Nhap ma khach hang: ");
    if (!safeInput(customer_id, sizeof(customer_id))) {
        printf("Loi nhap ma khach hang!\n");
        return ERR_INPUT_BUFFER;
    }

    // Tính toán hóa đơn mới
    error = calculate_invoice();
    if (error != SUCCESS) {
        return error;
    }

    // Xuất hóa đơn ra file
    FILE *f = fopen("invoice.txt", "w");
    if (f == NULL) {
        printf("Khong the mo file invoice.txt de ghi!\n");
        return ERR_FILE_OPEN;
    }

    error = write_invoice_to_file(customer_id, f);
    fclose(f);

    if (error == SUCCESS) {
        printf("\nDa xuat hoa don ra file invoice.txt\n");
    }

    return error;
}