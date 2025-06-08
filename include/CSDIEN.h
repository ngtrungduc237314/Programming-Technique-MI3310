#ifndef CSDIEN_H
#define CSDIEN_H

#include "input.h"
#include "date.h"

// Định nghĩa các mã lỗi đặc thù cho module CSDIEN
typedef enum {
    CSDIEN_ERR_INVALID_INDEX = -601,   // Chỉ số điện không hợp lệ
    CSDIEN_ERR_INDEX_SMALLER = -602    // Chỉ số điện nhỏ hơn kỳ trước
} CSDIENErrorCode;

// Định nghĩa cấu trúc chỉ số điện
struct eindex {
    char ID[20];        // Mã khách hàng
    int index;          // Chỉ số điện
    Date closing_date;  // Ngày chốt chỉ số
    int term;           // Kỳ thu phí
};

// Các hàm quản lý chỉ số điện
ErrorCode open_CSDIEN(void); // Mo file CSDIEN.BIN
ErrorCode add_CSDIEN(void); // Them chỉ số điện
ErrorCode edit_CSDIEN(void); // Sua chỉ số điện
ErrorCode remove_CSDIEN(void); // Xoa chỉ số điện
ErrorCode view_CSDIEN(void); // Xem thông tin chỉ số điện

// Lấy chỉ số điện của kỳ trước
ErrorCode getPreviousIndex(const char* id, int term, int* prev_index, Date* prev_date);

// Kiểm tra chỉ số điện hợp lệ
ErrorCode isValidIndex(int new_index, int prev_index);

// Kiểm tra trùng lặp chỉ số điện (mã KH + kỳ)
ErrorCode isElectricIndexExists(const char* id, int term);

#endif

//Quản lý chỉ số điện của khách hàng theo kỳ