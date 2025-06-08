#include "expense.h"
#include "GIADIEN.h"
#include "CSDIEN.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "input.h"

// Helper function to format number with commas
void format_number(char* str, float number) {
    char temp[50];
    sprintf(temp, "%.2f", number);
    
    int len = strlen(temp);
    int dot_pos = -1;
    
    // Find decimal point position
    for(int i = 0; i < len; i++) {
        if(temp[i] == '.') {
            dot_pos = i;
            break;
        }
    }
    
    if(dot_pos == -1) dot_pos = len;
    
    int j = 0;
    int count = 0;
    
    // Process digits before decimal point
    for(int i = dot_pos - 1; i >= 0; i--) {
        if(count == 3 && i != 0) {
            str[j++] = ',';
            count = 0;
        }
        str[j++] = temp[i];
        count++;
    }
    
    // Reverse the string up to j
    for(int i = 0; i < j/2; i++) {
        char t = str[i];
        str[i] = str[j-1-i];
        str[j-1-i] = t;
    }
    
    // Add decimal part if exists
    if(dot_pos < len) {
        strcpy(str + j, temp + dot_pos);
    } else {
        str[j] = '\0';
    }
}

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

// Tính tiền điện cho tất cả khách hàng và ghi vào file HOADON.BIN
ErrorCode process_bills(void) {
    FILE *fgd = fopen("GIADIEN.BIN", "rb");
    FILE *fcs = fopen("CSDIEN.BIN", "rb");
    FILE *flog = fopen("expense.txt", "w");  // File log text
    if (!fgd || !fcs || !flog) {
        printf("Khong mo duoc file GIADIEN.BIN, CSDIEN.BIN hoac expense.txt\n");
        if (fgd) fclose(fgd);
        if (fcs) fclose(fcs);
        if (flog) fclose(flog);
        return ERR_FILE_OPEN;
    }

    // Ghi header cho file log
    fprintf(flog, "TIEN DIEN TRONG KY\n\n");

    // Đọc bảng giá điện
    struct tariff tariffs[10];
    int n_tariff = fread(tariffs, sizeof(struct tariff), 10, fgd);
    fclose(fgd);

    if (n_tariff <= 0) {
        printf("Khong doc duoc bang gia dien\n");
        fclose(fcs);
        fclose(flog);
        return ERR_FILE_READ;
    }

    // Mở file output
    FILE *fbill = fopen("HOADON.BIN", "wb");
    if (!fbill) {
        printf("Khong mo duoc file HOADON.BIN de ghi\n");
        fclose(fcs);
        fclose(flog);
        return ERR_FILE_OPEN;
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

        // Format số tiền với dấu phẩy
        char formatted_amount[50];
        format_number(formatted_amount, amount);

        // Ghi log theo định dạng mới
        fprintf(flog, "Ma so khach hang (KHXXXXXX): %s\n", new_bill.customer_id);
        fprintf(flog, "Ky thu phi (1-12): %d\n", new_bill.term);
        fprintf(flog, "Dien nang tieu thu (kWh): %d\n", new_bill.consumption);
        fprintf(flog, "Tien dien (VND): %s\n\n", formatted_amount);

        if (fwrite(&new_bill, sizeof(struct bill), 1, fbill) != 1) {
            printf("Loi ghi file HOADON.BIN\n");
            fclose(fcs);
            fclose(fbill);
            fclose(flog);
            return ERR_FILE_WRITE;
        }
    }

    printf("Da xu ly xong va ghi vao HOADON.BIN va expense.txt\n");
    // Đóng các file
    fclose(fcs);
    fclose(fbill);
    fclose(flog);
    return SUCCESS;
}