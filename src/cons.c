#include "input.h"
#include "cons.h"
#include "KH.h"
#include "CSDIEN.h"
#include "cons.h"
#include "date.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Tính điện năng tiêu thụ cho tất cả khách hàng trong một kỳ
ErrorCode cal_cons(void) {
    while (getchar() != '\n'); // Clear buffer

    FILE *fkh = fopen("KH.BIN", "rb");
    FILE *fcs = fopen("CSDIEN.BIN", "rb");
    FILE *fout = fopen("cons.txt", "w");
    FILE *fbout = fopen("TIEUTHU.BIN", "wb");

    if (!fkh || !fcs || !fout || !fbout) {
        printf("Khong mo duoc mot trong cac file: KH.BIN, CSDIEN.BIN, cons.txt hoac TIEUTHU.BIN\n");
        if (fkh) fclose(fkh);
        if (fcs) fclose(fcs);
        if (fout) fclose(fout);
        if (fbout) fclose(fbout);
        return ERR_FILE_OPEN;
    }

    // Kiểm tra file KH.BIN có dữ liệu không
    fseek(fkh, 0, SEEK_END);
    if (ftell(fkh) == 0) {
        printf("Chua co du lieu khach hang!\n");
        fclose(fkh);
        fclose(fcs);
        fclose(fout);
        fclose(fbout);
        return ERR_DATA_NOTFOUND;
    }
    fseek(fkh, 0, SEEK_SET);

    // Kiểm tra file CSDIEN.BIN có dữ liệu không
    fseek(fcs, 0, SEEK_END);
    if (ftell(fcs) == 0) {
        printf("Chua co du lieu chi so dien!\n");
        fclose(fkh);
        fclose(fcs);
        fclose(fout);
        fclose(fbout);
        return ERR_DATA_NOTFOUND;
    }
    fseek(fcs, 0, SEEK_SET);

    // Đọc dữ liệu khách hàng
    struct customer kh;
    struct eindex cs;
    struct consumption_record rec;
    int found_any_data = 0;

    fprintf(fout, "THONG TIN DIEN NANG TIEU THU\n\n");

    // Đọc từng khách hàng
    while (fread(&kh, sizeof(struct customer), 1, fkh) == 1) {
        int found_customer_data = 0;
        int max_term = 0;
        struct eindex max_term_record;

        // Tìm kỳ lớn nhất của khách hàng
        while (fread(&cs, sizeof(struct eindex), 1, fcs) == 1) {
            if (strcmp(kh.ID, cs.ID) == 0) {
                found_customer_data = 1;
                if (cs.term > max_term) {
                    max_term = cs.term;
                    max_term_record = cs;
                }
            }
        }

        // Nếu không tìm thấy dữ liệu của khách hàng này
        if (!found_customer_data) {
            fprintf(fout, "Ma so khach hang (KHXXXXXX): %s\n", kh.ID);
            fprintf(fout, "Khach hang chua co du lieu chi so dien!\n\n");
            fseek(fcs, 0, SEEK_SET);
            continue;
        }

        // Khởi tạo record mới cho kỳ lớn nhất
        memset(&rec, 0, sizeof(struct consumption_record));
        strcpy(rec.ID, kh.ID);
        rec.term = max_term_record.term;
        rec.end_index = max_term_record.index;

        // Tìm chỉ số kỳ trước
        fseek(fcs, 0, SEEK_SET);
        found_customer_data = 0;  // Dùng lại biến này để kiểm tra kỳ trước
        while (fread(&cs, sizeof(struct eindex), 1, fcs) == 1) {
            if (strcmp(kh.ID, cs.ID) == 0 && cs.term == max_term - 1) {
                found_customer_data = 1;
                rec.start_index = cs.index;
                rec.start_date = cs.closing_date;
                break;
            }
        }

        // Ghi ra file text
        fprintf(fout, "Ma so khach hang (KHXXXXXX): %s\n", rec.ID);
        fprintf(fout, "Ky thu phi (1-12): %d\n", rec.term);
        
        if (found_customer_data) {
            rec.consumption = rec.end_index - rec.start_index;
            if (rec.consumption < 0) {
                fprintf(fout, "Loi: Chi so dien ky truoc lon hon ky nay!\n\n");
                continue;
            }
        } else {
            rec.consumption = 0;  // Kỳ đầu tiên
            fprintf(fout, "Khach hang chi co 1 ky!\n");
        }
        
        fprintf(fout, "Dien nang tieu thu (kWh): %d\n\n", rec.consumption);
        found_any_data = 1;

        // Ghi ra file binary
        if (fwrite(&rec, sizeof(struct consumption_record), 1, fbout) != 1) {
            printf("Loi ghi file TIEUTHU.BIN\n");
            fclose(fkh);
            fclose(fcs);
            fclose(fout);
            fclose(fbout);
            return ERR_FILE_WRITE;
        }

        fseek(fcs, 0, SEEK_SET);
    }

    if (!found_any_data) {
        fprintf(fout, "Khong tim thay du lieu tieu thu dien nao!\n");
    }

    fclose(fkh);
    fclose(fcs);
    fclose(fout);
    fclose(fbout);

    printf("\nDa ghi thong tin tieu thu vao cons.txt va TIEUTHU.BIN\n");
    return SUCCESS;
}

// Tìm chỉ số điện của kỳ trước
ErrorCode find_previous_reading(const struct eindex* readings, int count, const char* id, int term, Date* prev_date, int* prev_index) {
    if (readings == NULL || id == NULL || prev_index == NULL) {
        return ERR_NULL_POINTER;
    }

    int prev_term = term - 1;
    if (prev_term < 1) {
        prev_term = 12;  // Quay lại tháng 12 năm trước
    }

    for (int i = 0; i < count; i++) {
        if (strcmp(readings[i].ID, id) == 0 && readings[i].term == prev_term) {
            if (prev_date != NULL) {
                *prev_date = readings[i].closing_date;
            }
            *prev_index = readings[i].index;
            return SUCCESS;
        }
    }
    
    *prev_index = 0;  // Đặt giá trị mặc định khi không tìm thấy
    return ERR_DATA_NOTFOUND;  // Không tìm thấy kỳ trước
}