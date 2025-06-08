#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>

/*
// Các mức độ log
#define LOG_INFO    0
#define LOG_WARNING 1
#define LOG_ERROR   2
*/

// Định nghĩa các lỗi chung
typedef enum {
    // Thành công
    SUCCESS = 0,            // Thao tác thành công

    // Lỗi file
    ERR_FILE_OPEN = -100,   // Không mở được file
    ERR_FILE_READ = -101,   // Không đọc được file
    ERR_FILE_WRITE = -102,  // Không ghi được file
    ERR_FILE_CLOSE = -103,  // Không đóng được file
    ERR_FILE_REMOVE = -104, // Không xóa được file
    ERR_FILE_RENAME = -105, // Không đổi tên được file
    
    // Lỗi nhập liệu
    ERR_INPUT_BUFFER = -200,  // Buffer overflow
    ERR_INPUT_FORMAT = -201,  // Định dạng không hợp lệ
    ERR_INPUT_LENGTH = -202,  // Độ dài không hợp lệ
    
    // Lỗi dữ liệu
    ERR_DATA_INVALID = -300,  // Dữ liệu không hợp lệ
    ERR_DATA_DUPLICATE = -301,// Dữ liệu trùng lặp
    ERR_DATA_NOTFOUND = -302, // Không tìm thấy dữ liệu

    // Lỗi hệ thống
    ERR_SYSTEM = -400        // Lỗi hệ thống khác
} ErrorCode;

// Các hàm xử lý input cơ bản
int isValidNumber(const char* str);
void clearInputBuffer(void);
int safeInput(char* buffer, size_t size);

// Các hàm kiểm tra độ dài và format
int isValidLength(const char* str, size_t maxLen);
int isValidCustomerId(const char* id);
int isValidMeterNumber(const char* meter);

// Các hàm kiểm tra trùng lặp
int isCustomerIdExists(const char* id);
int isMeterNumberExists(const char* meter);

#endif
// Hỗ trợ xử lý input từ người dùng