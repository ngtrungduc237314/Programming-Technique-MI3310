#ifndef SEARCH_H
#define SEARCH_H

#include "input.h"
#include "date.h"

// Định nghĩa các mã lỗi đặc thù cho module Search
typedef enum {
    SEARCH_ERR_NO_RESULT = -900,   // Không tìm thấy kết quả
    SEARCH_ERR_INVALID_DATE = -902 // Ngày tháng không hợp lệ
} SearchErrorCode;

// Hàm tìm kiếm theo mã khách hàng
ErrorCode search_by_id(const char* customer_id);

//Hàm tiềm kiếm theo tên khách hàng và kỳ thu phí   
ErrorCode search_by_name_and_term(const char* customer_name, int term);

#endif
//Tìm kiếm thông tin khách hàng và tra cứu điện năng tiêu thụ 