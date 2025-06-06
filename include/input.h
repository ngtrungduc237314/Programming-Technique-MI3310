#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>

// Các mức độ log
#define LOG_INFO    0
#define LOG_WARNING 1
#define LOG_ERROR   2

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