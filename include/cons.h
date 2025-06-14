#ifndef CONS_H
#define CONS_H

#include "date.h"
#include "input.h"
#include "CSDIEN.h"  // Cần thêm vì dùng struct eindex

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
ErrorCode cal_cons(void);

// Tìm chỉ số điện của kỳ trước
ErrorCode find_previous_reading(const struct eindex* readings, int count, const char* id, int term, Date* prev_date, int* prev_index);

#endif
// Quản lý điện năng tiêu thụ của khách hàng theo kỳ