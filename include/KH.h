#ifndef KH_H
#define KH_H

#include "input.h"

// Định nghĩa các mã lỗi đặc thù cho module KH
typedef enum {
    KH_ERR_INVALID_AGE = -500,     // Tuổi không hợp lệ
    KH_ERR_INVALID_GENDER = -501   // Giới tính không hợp lệ
} KHErrorCode;

// Cấu trúc thông tin khách hàng
struct customer {
    char ID[20];
    char Name[50];
    char Address[100];
    char Meter[20];
};

// Các hàm quản lý khách hàng
ErrorCode open_KH(void);
ErrorCode add_KH(void);
ErrorCode edit_KH(void);
ErrorCode remove_KH(void);

#endif

//Quản lý dữ liệu khách hàng trong file KH.BIN