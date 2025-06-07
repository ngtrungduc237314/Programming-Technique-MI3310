#ifndef CSDIEN_H
#define CSDIEN_H

#include "date.h"

// Định nghĩa cấu trúc chỉ số điện
struct eindex {
    char ID[20];        // Mã khách hàng
    int index;          // Chỉ số điện
    Date closing_date;  // Ngày chốt chỉ số
    int term;           // Kỳ thu phí
};

// Trả về 0 nếu thành công, khác 0 nếu thất bại
int open_CSDIEN(void); // Mo file CSDIEN.BIN
int add_CSDIEN(void); // Them chỉ số điện
int edit_CSDIEN(void); // Sua chỉ số điện
int remove_CSDIEN(void); // Xoa chỉ số điện

// Kiểm tra trùng lặp chỉ số điện (mã KH + kỳ)
int isElectricIndexExists(const char* id, int term);

// Lấy chỉ số điện của kỳ trước
int getPreviousIndex(const char* id, int term, Date* prev_date);

#endif

//Quản lý chỉ số điện của khách hàng theo kỳ