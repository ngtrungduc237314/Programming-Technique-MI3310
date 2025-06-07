#ifndef SEARCH_H
#define SEARCH_H

#include "date.h"

// Cấu trúc lưu trữ kết quả tìm kiếm
struct search_result {
    char ID[20];           // Mã khách hàng
    char Name[50];         // Tên khách hàng
    char Address[100];     // Địa chỉ
    char Meter[20];        // Mã công tơ
    int term;              // Kỳ thu phí
    Date closing_date;     // Ngày chốt chỉ số
    int index;             // Chỉ số điện
    int consumption;       // Điện năng tiêu thụ
    int days;              // Số ngày tiêu thụ
};

// Node trong linked list kết quả tìm kiếm
struct search_node {
    struct search_result data;
    struct search_node* next;
};

// Linked list kết quả tìm kiếm
struct search_list {
    struct search_node* head;
    int count;
};

// Tìm kiếm theo mã khách hàng
// Trả về: 0 nếu tìm thấy, -1 nếu lỗi file, -2 nếu không tìm thấy, -3 nếu dữ liệu không hợp lệ
int search_by_id(const char* customer_id);

/*
// Tìm kiếm theo mã khách hàng và kỳ thu phí
// Trả về: 0 nếu tìm thấy, -1 nếu lỗi file, -2 nếu không tìm thấy, -3 nếu dữ liệu không hợp lệ
int search_by_id_and_term(const char* customer_id, int term);

// Tìm kiếm theo tên khách hàng (hỗ trợ tìm kiếm mờ)
// Trả về: 0 nếu tìm thấy, -1 nếu lỗi file, -2 nếu không tìm thấy, -3 nếu dữ liệu không hợp lệ
int search_by_name(const char* customer_name);
*/

#endif
//Tìm kiếm thông tin khách hàng và tra cứu điện năng tiêu thụ 