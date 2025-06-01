#include "cons.h"
#include "../file_KH/KH.h"
#include "../file_CSDIEN/CSDIEN.h"
#include "../utils/date.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Tìm chỉ số điện của kỳ trước
static int find_previous_reading(const struct eindex* readings, int count, 
                               const char* id, int term, Date* prev_date) {
    int prev_term = term - 1;
    if (prev_term < 1) {
        prev_term = 12;  // Quay lại tháng 12 năm trước
    }

    for (int i = 0; i < count; i++) {
        if (strcmp(readings[i].ID, id) == 0 && readings[i].term == prev_term) {
            if (prev_date != NULL) {
                *prev_date = readings[i].closing_date;
            }
            return readings[i].index;
        }
    }
    return -1;  // Không tìm thấy kỳ trước
}

// Tính điện năng tiêu thụ cho tất cả khách hàng trong một kỳ
int cal_cons(void) {
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
        return -1;
    }

    // Đọc dữ liệu khách hàng
    struct customer kh;
    struct eindex cs;
    struct consumption_record rec;

    // Ghi header cho file text
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(fout, "==========================================================\n");
    fprintf(fout, "CHUONG TRINH QUAN LY DIEN NANG - TINH DIEN NANG TIEU THU\n");
    fprintf(fout, "Thoi gian: %02d/%02d/%04d %02d:%02d:%02d\n", 
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec);
    fprintf(fout, "==========================================================\n\n");
    fprintf(fout, "MaKH\tTenKH\tKy\tNgayDau\tNgayCuoi\tChiSoDau\tChiSoCuoi\tTieuThu\tSoNgay\n");

    // Đọc từng khách hàng
    while (fread(&kh, sizeof(struct customer), 1, fkh) == 1) {
        // Lưu vị trí hiện tại trong file chỉ số điện
        long cs_pos = ftell(fcs);
        
        // Đọc từng chỉ số điện
        while (fread(&cs, sizeof(struct eindex), 1, fcs) == 1) {
            if (strcmp(kh.ID, cs.ID) == 0) {
                // Khởi tạo record mới
                memset(&rec, 0, sizeof(struct consumption_record));
                strcpy(rec.ID, kh.ID);
                strcpy(rec.Name, kh.Name);
                rec.term = cs.term;
                rec.end_date = cs.closing_date;
                rec.end_index = cs.index;

                // Tìm chỉ số kỳ trước
                rec.start_index = find_previous_reading((struct eindex*)fcs, 
                                                      cs_pos / sizeof(struct eindex),
                                                      cs.ID, cs.term, &rec.start_date);

                // Nếu là kỳ đầu tiên
                if (rec.start_index == -1) {
                    rec.start_index = 0;
                    rec.start_date = cs.closing_date;  // Ngày đầu = ngày cuối
                    rec.days = 0;
                    rec.consumption = 0;
                } else {
                    // Tính số ngày tiêu thụ
                    rec.days = daysBetween(&rec.start_date, &rec.end_date);
                    if (rec.days < 0) {
                        printf("Loi: Ngay chot ky truoc (%d/%d/%d) sau ngay chot hien tai (%d/%d/%d)\n",
                               rec.start_date.day, rec.start_date.month, rec.start_date.year,
                               rec.end_date.day, rec.end_date.month, rec.end_date.year);
                        continue;
                    }

                    // Tính điện năng tiêu thụ
                    rec.consumption = rec.end_index - rec.start_index;
                    if (rec.consumption < 0) {
                        printf("Loi: Chi so dien ky truoc (%d) lon hon ky nay (%d)\n",
                               rec.start_index, rec.end_index);
                        continue;
                    }
                }

                // Ghi ra file text
                char start_date_str[20], end_date_str[20];
                dateToString(&rec.start_date, start_date_str);
                dateToString(&rec.end_date, end_date_str);
                fprintf(fout, "%s\t%s\t%d\t%s\t%s\t%d\t%d\t%d\t%d\n",
                        rec.ID, rec.Name, rec.term,
                        start_date_str, end_date_str,
                        rec.start_index, rec.end_index,
                        rec.consumption, rec.days);

                // Ghi ra file binary
                if (fwrite(&rec, sizeof(struct consumption_record), 1, fbout) != 1) {
                    printf("Loi ghi file TIEUTHU.BIN\n");
                    fclose(fkh);
                    fclose(fcs);
                    fclose(fout);
                    fclose(fbout);
                    return -1;
                }
            }
        }
        
        // Quay lại đầu file chỉ số điện cho khách hàng tiếp theo
        fseek(fcs, 0, SEEK_SET);
    }

    fclose(fkh);
    fclose(fcs);
    fclose(fout);
    fclose(fbout);
    printf("Da ghi thong tin tieu thu vao cons.txt va TIEUTHU.BIN\n");
    return 0;
}
