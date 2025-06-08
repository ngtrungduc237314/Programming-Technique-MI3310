#include "search.h"
#include "input.h"
#include "KH.h"
#include "cons.h"
#include <stdio.h>
#include <string.h>

ErrorCode search_by_id(const char* customer_id) {
    if (!customer_id || !isValidCustomerId(customer_id)) {
        printf("Ma khach hang khong hop le!\n");
        return ERR_INPUT_FORMAT;
    }

    // Mở các file
    FILE *fkh = fopen("KH.BIN", "rb");
    FILE *fcons = fopen("TIEUTHU.BIN", "rb");
    FILE *fout = fopen("IDsearch.txt", "w");
    
    if (!fkh || !fcons || !fout) {
        printf("Khong mo duoc file!\n");
        if (fkh) fclose(fkh);
        if (fcons) fclose(fcons);
        if (fout) fclose(fout);
        return ERR_FILE_OPEN;
    }

    // Ghi header
    fprintf(fout, "THONG TIN KHACH HANG DUOC TIM KIEM\n\n");

    // Tìm thông tin khách hàng
    struct customer kh;
    int found_customer = 0;
    
    while (fread(&kh, sizeof(struct customer), 1, fkh) == 1) {
        if (strcmp(kh.ID, customer_id) == 0) {
            found_customer = 1;
            // Ghi thông tin khách hàng
            fprintf(fout, "Ma so khach hang (KHXXXXXX): %s\n", kh.ID);
            fprintf(fout, "Ten khach hang: %s\n", kh.Name);
            fprintf(fout, "Dia chi: %s\n", kh.Address);
            fprintf(fout, "Ma so cong to (CTXXXXXX): %s\n\n", kh.Meter);
            break;
        }
    }

    if (!found_customer) {
        printf("Khong tim thay khach hang!\n");
        fclose(fkh);
        fclose(fcons);
        fclose(fout);
        return SEARCH_ERR_NO_RESULT;
    }

    // Tìm thông tin tiêu thụ
    struct consumption_record cons;
    int found_consumption = 0;
    
    fprintf(fout, "THONG TIN TIEU THU DIEN:\n");
    while (fread(&cons, sizeof(struct consumption_record), 1, fcons) == 1) {
        if (strcmp(cons.ID, customer_id) == 0) {
            found_consumption = 1;
            fprintf(fout, "\nKy %d:\n", cons.term);
            fprintf(fout, "- Thoi gian: Tu %02d/%02d/%04d den %02d/%02d/%04d\n", 
                cons.start_date.day, cons.start_date.month, cons.start_date.year,
                cons.end_date.day, cons.end_date.month, cons.end_date.year);
            fprintf(fout, "- Chi so dau ky: %d\n", cons.start_index);
            fprintf(fout, "- Chi so cuoi ky: %d\n", cons.end_index);
            fprintf(fout, "- Dien nang tieu thu: %d kWh\n", cons.consumption);
            fprintf(fout, "- So ngay su dung: %d ngay\n", cons.days);
        }
    }

    if (!found_consumption) {
        fprintf(fout, "\nChua co du lieu tieu thu dien\n");
    }

    fclose(fkh);
    fclose(fcons);
    fclose(fout);

    printf("Da ghi ket qua tim kiem vao file IDsearch.txt\n");
    return SUCCESS;
}

ErrorCode search_by_name_and_term(const char* customer_name, int term) {
    if (!customer_name || strlen(customer_name) == 0 || !isValidCustomerName(customer_name)) {
        printf("Ten khach hang khong hop le!\n");
        return ERR_DATA_INVALID;
    }

    if (!isValidTerm(term)) {
        printf("Ky thu phi phai tu 1 den 12!\n");
        return ERR_DATA_INVALID;
    }

    // Mở các file
    FILE *fkh = fopen("KH.BIN", "rb");
    FILE *fcons = fopen("TIEUTHU.BIN", "rb");
    FILE *fout = fopen("NTsearch.txt", "w");
    
    if (!fkh || !fcons || !fout) {
        printf("Khong mo duoc file!\n");
        if (fkh) fclose(fkh);
        if (fcons) fclose(fcons);
        if (fout) fclose(fout);
        return ERR_FILE_OPEN;
    }

    // Ghi header
    fprintf(fout, "THONG TIN KHACH HANG DUOC TIM KIEM\n\n");

    // Tìm thông tin khách hàng
    struct customer kh;
    int found = 0;
    
    while (fread(&kh, sizeof(struct customer), 1, fkh) == 1) {
        if (strstr(kh.Name, customer_name) != NULL) {
            // Tìm thông tin tiêu thụ của khách hàng này trong kỳ cần tìm
            struct consumption_record cons;
            int found_consumption = 0;
            
            // Reset file pointer
            fseek(fcons, 0, SEEK_SET);
            
            while (fread(&cons, sizeof(struct consumption_record), 1, fcons) == 1) {
                if (strcmp(cons.ID, kh.ID) == 0 && cons.term == term) {
                    found = 1;
                    found_consumption = 1;
                    
                    // Ghi thông tin khách hàng
                    fprintf(fout, "Ma so khach hang (KHXXXXXX): %s\n", kh.ID);
                    fprintf(fout, "Ten khach hang: %s\n", kh.Name);
                    fprintf(fout, "Dia chi: %s\n", kh.Address);
                    fprintf(fout, "Ma so cong to (CTXXXXXX): %s\n", kh.Meter);
                    fprintf(fout, "Ky thu phi: %d\n", cons.term);
                    fprintf(fout, "Dien nang tieu thu: %d kWh\n\n", cons.consumption);
                    break;
                }
            }
            
            if (!found_consumption) {
                // Khách hàng được tìm thấy nhưng không có dữ liệu tiêu thụ trong kỳ này
                found = 1;
                fprintf(fout, "Ma so khach hang (KHXXXXXX): %s\n", kh.ID);
                fprintf(fout, "Ten khach hang: %s\n", kh.Name);
                fprintf(fout, "Dia chi: %s\n", kh.Address);
                fprintf(fout, "Ma so cong to (CTXXXXXX): %s\n", kh.Meter);
                fprintf(fout, "Ky thu phi: %d\n", term);
                fprintf(fout, "Dien nang tieu thu: Chua co du lieu\n\n");
            }
        }
    }

    fclose(fkh);
    fclose(fcons);
    fclose(fout);

    if (!found) {
        printf("Khong tim thay khach hang!\n");
        return SEARCH_ERR_NO_RESULT;
    }

    printf("Da ghi ket qua tim kiem vao file NTsearch.txt\n");
    return SUCCESS;
}

