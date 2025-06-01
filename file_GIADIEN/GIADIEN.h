#ifndef GIADIEN_H
#define GIADIEN_H

#include "../utils/date.h"

// Số bậc giá điện tối đa
#define MAX_TARIFF 6

// Định nghĩa cấu trúc bậc giá điện
struct tariff {
    int level;              // Bậc giá điện (1-6)
    int usage_bot;          // Điện năng tiêu thụ - giới hạn dưới
    int usage_top;          // Điện năng tiêu thụ - giới hạn trên
    float price;            // Đơn giá điện theo bậc (VND/kWh)
}; 

// Các hàm thao tác với file GIADIEN.BIN
// Trả về 0 nếu thành công, -1 nếu thất bại

// Khởi tạo file với các bậc giá điện mặc định
int open_GIADIEN(void);

// Thêm bậc giá điện mới (không khuyến khích vì đã cố định theo thực tế)
int add_GIADIEN(void);

// Sửa thông tin bậc giá điện
int edit_GIADIEN(void);

// Xóa bậc giá điện (không khuyến khích vì đã cố định theo thực tế)
int remove_GIADIEN(void);

#endif