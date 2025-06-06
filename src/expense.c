#include "expense.h"
#include "GIADIEN.h"
#include "CSDIEN.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Tính tiền điện theo bậc thang (business logic)
float calc_amount(int consumption, struct tariff *tariffs, int tariff_count) {
    float total = 0;
    int remain = consumption;
    
    // Duyệt qua từng bậc giá
    for (int i = 0; i < tariff_count; i++) {
        int lower = tariffs[i].usage_bot;
        int upper = tariffs[i].usage_top;
        int used = 0;
        
        // Dừng nếu đã tính hết số điện tiêu thụ
        if (remain <= 0) break;
        
        // Tính số kWh sử dụng trong bậc này
        if (consumption > upper) {
            used = upper - lower + 1;  // Sử dụng toàn bộ khoảng của bậc
        } else if (consumption >= lower) {
            used = remain;  // Sử dụng phần còn lại
        }
        
        // Tính tiền cho bậc này và cập nhật số điện còn lại
        if (used > 0) {
            total += used * tariffs[i].price;
            remain -= used;
        }
    }
    return total;
}

// Tính tiền điện cho tất cả khách hàng và ghi vào file HOADON.BIN (data access)
int process_bills(void) {
    FILE *fgd = fopen("GIADIEN.BIN", "rb");
    FILE *fcs = fopen("CSDIEN.BIN", "rb");
    FILE *flog = fopen("expense.txt", "w");  // File log text
    if (!fgd || !fcs || !flog) {
        printf("Khong mo duoc file GIADIEN.BIN, CSDIEN.BIN hoac expense.txt\n");
        if (fgd) fclose(fgd);
        if (fcs) fclose(fcs);
        if (flog) fclose(flog);
        return -1;
    }

    // Ghi header cho file log
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(flog, "==========================================================\n");
    fprintf(flog, "CHUONG TRINH QUAN LY DIEN NANG - TINH TIEN DIEN\n");
    fprintf(flog, "Thoi gian: %02d/%02d/%04d %02d:%02d:%02d\n", 
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec);
    fprintf(flog, "==========================================================\n\n");
    fprintf(flog, "MA_KH\tKY\tTIEU_THU\tTIEN_DIEN\n");

    // Đọc bảng giá điện
    struct tariff tariffs[10];
    int n_tariff = fread(tariffs, sizeof(struct tariff), 10, fgd);
    fclose(fgd);

    if (n_tariff <= 0) {
        printf("Khong doc duoc bang gia dien\n");
        fclose(fcs);
        fclose(flog);
        return -1;
    }

    // Mở file output
    FILE *fbill = fopen("HOADON.BIN", "wb");
    if (!fbill) {
        printf("Khong mo duoc file HOADON.BIN de ghi\n");
        fclose(fcs);
        fclose(flog);
        return -1;
    }

    // Đọc và xử lý từng chỉ số điện
    struct eindex current_index;
    while (fread(&current_index, sizeof(struct eindex), 1, fcs) == 1) {
        // Tìm chỉ số đầu kỳ
        int starting_index = 0;
        fpos_t pos;
        fgetpos(fcs, &pos);  // Lưu vị trí hiện tại
        
        // Reset về đầu file để tìm
        fseek(fcs, 0, SEEK_SET);
        struct eindex prev_index;
        while (fread(&prev_index, sizeof(struct eindex), 1, fcs) == 1) {
            if (strcmp(prev_index.ID, current_index.ID) == 0 && 
                prev_index.term == current_index.term - 1) {
                starting_index = prev_index.index;
                break;
            }
        }
        
        // Trở lại vị trí đọc cũ
        fsetpos(fcs, &pos);

        // Tính điện năng tiêu thụ
        int consumption = current_index.index - starting_index;
        if (consumption < 0) consumption = 0;

        // Tính tiền điện
        float amount = calc_amount(consumption, tariffs, n_tariff);

        // Tạo và ghi hóa đơn
        struct bill new_bill;
        strcpy(new_bill.customer_id, current_index.ID);
        new_bill.term = current_index.term;
        new_bill.consumption = consumption;
        new_bill.amount = amount;

        // Ghi log tinh toan
        fprintf(flog, "%s\t%d\t%d\t%.2f\n", 
                new_bill.customer_id, new_bill.term, 
                new_bill.consumption, new_bill.amount);

        if (fwrite(&new_bill, sizeof(struct bill), 1, fbill) != 1) {
            printf("Loi ghi file HOADON.BIN\n");
            fclose(fcs);
            fclose(fbill);
            fclose(flog);
            return -1;
        }
    }

    printf("Da xu ly xong va ghi vao HOADON.BIN va expense.txt\n");
    // Đóng các file
    fclose(fcs);
    fclose(fbill);
    fclose(flog);
    return 0;
}