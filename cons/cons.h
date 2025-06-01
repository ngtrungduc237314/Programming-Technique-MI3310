#ifndef CONS_H
#define CONS_H

#include "../utils/date.h"

// Cấu trúc lưu trữ thông tin tiêu thụ điện
struct consumption_record {
    char ID[20];           // Mã khách hàng
    char Name[50];         // Tên khách hàng
    int term;              // Kỳ thu phí
    Date start_date;       // Ngày đầu kỳ (ngày chốt kỳ trước)
    Date end_date;         // Ngày cuối kỳ (ngày chốt hiện tại)
    int start_index;       // Chỉ số đầu kỳ
    int end_index;         // Chỉ số cuối kỳ
    int consumption;       // Điện năng tiêu thụ
    int days;              // Số ngày tiêu thụ
};

// Tính điện năng tiêu thụ cho tất cả khách hàng trong một kỳ
// Vào: 
// - Đọc từ file KH.BIN thông tin khách hàng
// - Đọc từ file CSDIEN.BIN thông tin chỉ số điện
// Ra: 
// - Ghi thông tin tiêu thụ vào file cons.txt (text)
// - Ghi thông tin tiêu thụ vào file TIEUTHU.BIN (binary)
// Trả về:
// - 0 nếu thành công
// - -1 nếu có lỗi (không mở được file, lỗi đọc/ghi,...)
int cal_cons(void);

#endif
