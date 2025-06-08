#ifndef SEARCH_H
#define SEARCH_H

#include "input.h"
#include "date.h"

// Định nghĩa các mã lỗi đặc thù cho module Search
typedef enum {
    SEARCH_ERR_NO_RESULT = -900,   // Không tìm thấy kết quả
    SEARCH_ERR_TOO_MANY = -901,    // Quá nhiều kết quả
    SEARCH_ERR_INVALID_DATE = -902 // Ngày tháng không hợp lệ
} SearchErrorCode;

// Cấu trúc kết quả tìm kiếm
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

// Các hàm tìm kiếm
ErrorCode search_by_id(const char* customer_id);

/*
ErrorCode search_by_name(const char* customer_name);
ErrorCode search_by_date(const Date* from_date, const Date* to_date);
ErrorCode search_by_amount(float min_amount, float max_amount);
*/

#endif
//Tìm kiếm thông tin khách hàng và tra cứu điện năng tiêu thụ 