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

// Hàm nội bộ để lấy chỉ số điện và ngày từ file
static ErrorCode get_index_and_date(const char* id, int term, struct eindex* curr_index, struct eindex* prev_index) {
    FILE* fp = fopen("CSDIEN.BIN", "rb");
    if (fp == NULL) {
        return ERR_FILE_OPEN;
    }

    int found_curr = 0, found_prev = 0;
    struct eindex ei;
    
    while (fread(&ei, sizeof(struct eindex), 1, fp) == 1) {
        if (strcmp(ei.ID, id) == 0) {
            if (ei.term == term) {
                *curr_index = ei;
                found_curr = 1;
            } else if (ei.term == term - 1) {
                *prev_index = ei;
                found_prev = 1;
            }
        }
    }
    fclose(fp);

    if (!found_curr) {
        printf("Khong tim thay chi so dien ky %d\n", term);
        return INVOICE_ERR_NO_INDEX;
    }
    if (!found_prev) {
        printf("Khong tim thay chi so dien ky %d\n", term - 1);
        return INVOICE_ERR_NO_INDEX;
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

// Tính toán và xuất hóa đơn
static ErrorCode process_invoice(const char* customer_id, int term, FILE* output_file) {
    if (!customer_id || !output_file || !isValidCustomerId(customer_id)) {
        return ERR_DATA_INVALID;
    }

    // Đọc thông tin khách hàng
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

    // Khởi tạo record hóa đơn
    struct invoice_record inv = {0};
    strcpy(inv.ID, kh.ID);
    strcpy(inv.Name, kh.Name);
    strcpy(inv.Address, kh.Address);
    strcpy(inv.Meter, kh.Meter);
    inv.term = term;

    // Lấy chỉ số điện và ngày tháng
    struct eindex curr_ei, prev_ei;
    ErrorCode error = get_index_and_date(inv.ID, inv.term, &curr_ei, &prev_ei);
    if (error != SUCCESS) {
        return error;
    }

    // Cập nhật thông tin từ chỉ số điện
    inv.curr_index = curr_ei.index;
    inv.prev_index = prev_ei.index;
    inv.usage = inv.curr_index - inv.prev_index;

    // Định dạng ngày tháng
    sprintf(inv.from_date, "%02d/%02d/%04d", 
            prev_ei.closing_date.day,
            prev_ei.closing_date.month,
            prev_ei.closing_date.year);
    
    sprintf(inv.to_date, "%02d/%02d/%04d",
            curr_ei.closing_date.day,
            curr_ei.closing_date.month,
            curr_ei.closing_date.year);

    // Lấy và tính giá điện
    struct tariff tariffs[MAX_TARIFF];
    int tariff_count = 0;
    error = read_tariffs(tariffs, &tariff_count);
    if (error != SUCCESS || tariff_count == 0) {
        return INVOICE_ERR_NO_TARIFF;
    }

    // Tính tiền
    inv.amount = calc_amount(inv.usage, tariffs, tariff_count);
    inv.tax = inv.amount * 0.1f;
    inv.total = inv.amount + inv.tax;

    // Chuyển số tiền thành chữ
    error = number_to_text((int)inv.total, inv.amount_text);
    if (error != SUCCESS) {
        return error;
    }

    // Lưu hóa đơn
    error = save_invoice(&inv);
    if (error != SUCCESS) {
        return error;
    }

    // In hóa đơn ra file
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    // Format các số tiền
    char amount_str[50], tax_str[50], total_str[50];
    format_number(amount_str, inv.amount);
    format_number(tax_str, inv.tax);
    format_number(total_str, inv.total);

    fprintf(output_file, "==========================================================\n");
    fprintf(output_file, "CHUONG TRINH QUAN LY DIEN NANG - HOA DON TIEN DIEN\n");
    fprintf(output_file, "Thoi gian: %02d/%02d/%04d %02d:%02d:%02d\n", 
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec);
    fprintf(output_file, "==========================================================\n\n");
    fprintf(output_file, "HOA DON TIEN DIEN\n");
    fprintf(output_file, "Ma KH: %s\n", inv.ID);
    fprintf(output_file, "Ten KH: %s\n", inv.Name);
    fprintf(output_file, "Dia chi: %s\n", inv.Address);
    fprintf(output_file, "Ma cong to: %s\n", inv.Meter);
    fprintf(output_file, "Ky: %d\n", inv.term);
    fprintf(output_file, "Tu ngay: %s\n", inv.from_date);
    fprintf(output_file, "Den ngay: %s\n", inv.to_date);
    fprintf(output_file, "Chi so moi: %d kWh\n", inv.curr_index);
    fprintf(output_file, "Dien nang tieu thu: %d kWh\n", inv.usage);
    fprintf(output_file, "Tien dien: %s VND\n", amount_str);
    fprintf(output_file, "Thue VAT (10%%): %s VND\n", tax_str);
    fprintf(output_file, "Tong cong thanh toan: %s VND\n", total_str);
    fprintf(output_file, "So tien bang chu: %s dong.\n", inv.amount_text);
    fprintf(output_file, "==========================================================\n\n");

    return SUCCESS;
}

// Xử lý hóa đơn đầy đủ
ErrorCode full_invoice(void) {
    char customer_id[20];
    printf("Nhap ma khach hang: ");
    if (!safeInput(customer_id, sizeof(customer_id))) {
        printf("Loi nhap ma khach hang!\n");
        return ERR_INPUT_BUFFER;
    }

    int term;
    printf("Nhap ky (1-12): ");
    if (scanf("%d", &term) != 1 || term < 1 || term > 12) {
        printf("Ky thu phi khong hop le!\n");
        while (getchar() != '\n');
        return ERR_INPUT_FORMAT;
    }
    while (getchar() != '\n');

    FILE *f = fopen("invoice.txt", "w");
    if (f == NULL) {
        printf("Khong the mo file invoice.txt de ghi!\n");
        return ERR_FILE_OPEN;
    }

    ErrorCode error = process_invoice(customer_id, term, f);
    fclose(f);

    if (error == SUCCESS) {
        printf("\nDa xuat hoa don ra file invoice.txt\n");
    }

    return error;
}

// Ham chuyen so thanh chu (ho tro den 1 trieu)
ErrorCode number_to_text(int n, char *buf) {
    if (buf == NULL) {
        return ERR_DATA_INVALID;
    }

    const char *ones[] = {"khong", "mot", "hai", "ba", "bon", "nam", "sau", "bay", "tam", "chin"};
    char temp[256] = "";
    
    if (n == 0) {
        strcpy(buf, "Khong");
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

    // Viết hoa chữ cái đầu tiên
    if (strlen(temp) > 0) {
        temp[0] = temp[0] - 32; // Chuyển từ chữ thường sang chữ hoa
    }

    strcpy(buf, temp);
    return SUCCESS;
}