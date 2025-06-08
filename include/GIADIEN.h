#ifndef GIADIEN_H
#define GIADIEN_H

#include "input.h"

// Định nghĩa các mã lỗi đặc thù cho module GIADIEN
typedef enum {
    GIADIEN_ERR_INVALID_LEVEL = -700,  // Bậc giá không hợp lệ
    GIADIEN_ERR_INVALID_PRICE = -701,  // Đơn giá không hợp lệ
    GIADIEN_ERR_INVALID_RANGE = -702   // Khoảng sử dụng không hợp lệ
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

// Khởi tạo file với các bậc giá điện mặc định
ErrorCode open_GIADIEN(void);

// Thêm bậc giá điện mới 
ErrorCode add_GIADIEN(void);

// Sửa thông tin bậc giá điện
ErrorCode edit_GIADIEN(void);

// Xóa bậc giá điện 
ErrorCode remove_GIADIEN(void);


#endif

//Quản lý bảng giá điện theo bậc thang