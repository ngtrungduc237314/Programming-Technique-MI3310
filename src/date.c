#include <stdio.h>
#include <string.h>
#include "date.h"
#include "input.h"

// Mảng số ngày trong tháng (0-based)
static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// Kiểm tra năm nhuận
static int isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Lấy số ngày trong tháng
static int getDaysInMonth(int month, int year) {
    if (month == 2 && isLeapYear(year)) return 29;
    return daysInMonth[month - 1];
}

// Chuyển chuỗi sang ngày (format: DD/MM/YYYY)
int parseDate(const char* dateStr, Date* date) {
    if (!dateStr || !date) return 0;
    
    if (sscanf(dateStr, "%d/%d/%d", &date->day, &date->month, &date->year) != 3)
        return 0;
        
    return isValidDate(date);
}

// Kiểm tra ngày hợp lệ
int isValidDate(const Date* date) {
    if (!date) return 0;
    
    if (date->month < 1 || date->month > 12) return 0;
    if (date->day < 1) return 0;
    
    int maxDays = getDaysInMonth(date->month, date->year);
    if (date->day > maxDays) return 0;
    
    return 1;
}

// So sánh 2 ngày       
int compareDate(const Date* d1, const Date* d2) {
    if (!d1 || !d2) return 0;
    
    if (d1->year != d2->year)
        return d1->year > d2->year ? 1 : -1;
        
    if (d1->month != d2->month)
        return d1->month > d2->month ? 1 : -1;
        
    if (d1->day != d2->day)
        return d1->day > d2->day ? 1 : -1;
        
    return 0;
}

// Chuyển ngày thành số ngày kể từ 1/1/1
static int dateToNumber(const Date* date) {
    int y = date->year - 1;
    int days = y * 365 + y/4 - y/100 + y/400;
    
    for (int m = 1; m < date->month; m++) {
        days += getDaysInMonth(m, date->year);
    }
    
    return days + date->day;
}

// Tính số ngày giữa 2 ngày 
int daysBetween(const Date* start, const Date* end) {
    if (!start || !end) return 0;
    if (!isValidDate(start) || !isValidDate(end)) return 0;
    
    return dateToNumber(end) - dateToNumber(start);
}

// Chuyển ngày thành chuỗi (format: DD/MM/YYYY)
void dateToString(const Date* date, char* buffer) {
    if (!date || !buffer) return;
    sprintf(buffer, "%02d/%02d/%04d", date->day, date->month, date->year);
} 