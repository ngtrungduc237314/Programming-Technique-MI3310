#ifndef KH_H
#define KH_H

#include "input.h"

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
ErrorCode view_KH(void);  // Thêm hàm xem thông tin khách hàng

#endif

//Quản lý dữ liệu khách hàng trong file KH.BIN