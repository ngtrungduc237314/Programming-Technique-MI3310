#ifndef GIADIEN_H
#define GIADIEN_H

#include "input.h"

// Định nghĩa các mã lỗi đặc thù cho module GIADIEN
typedef enum {
    GIADIEN_ERR_INVALID_LEVEL = -700,  // Bậc giá không hợp lệ
    GIADIEN_ERR_INVALID_PRICE = -701,  // Đơn giá không hợp lệ
    GIADIEN_ERR_INVALID_RANGE = -702,  // Khoảng sử dụng không hợp lệ
    GIADIEN_ERR_DATA_FULL = -703,      // Số bậc đã đạt tối đa
    GIADIEN_ERR_DATA_DUPLICATE = -704, // Bậc giá điện đã tồn tại
    GIADIEN_ERR_DATA_NOTFOUND = -705   // Không tìm thấy bậc giá điện
} GIADIENErrorCode;

// Số bậc giá điện tối đa
#define MAX_TARIFF 6

// Định nghĩa cấu trúc bậc giá điện
struct tariff {
    int level;              // Bậc giá điện (1-6)
    int usage_bot;          // Điện năng tiêu thụ - giới hạn dưới
    int usage_top;          // Điện năng tiêu thụ - giới hạn trên
    float price;            // Đơn giá điện theo bậc (VND/kWh)
}; 

//Quản lý file GIADIEN.BIN
ErrorCode open_GIADIEN(void); // Mo file GIADIEN.BIN
ErrorCode add_GIADIEN(void); // Thêm bậc giá điện
ErrorCode edit_GIADIEN(void); // Sửa bậc giá điện
ErrorCode remove_GIADIEN(void); // Xóa bậc giá điện
ErrorCode view_GIADIEN(void); // Xem thông tin bảng giá điện

#endif

//Quản lý bảng giá điện theo bậc thang