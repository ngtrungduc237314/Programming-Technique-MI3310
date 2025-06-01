#ifndef DATE_H
#define DATE_H

// Cấu trúc ngày tháng
typedef struct {
    int day;
    int month;
    int year;
} Date;

// Chuyển chuỗi sang ngày (format: DD/MM/YYYY)
int parseDate(const char* dateStr, Date* date);

// Kiểm tra ngày hợp lệ
int isValidDate(const Date* date);

// So sánh 2 ngày
// Trả về: -1 nếu d1 < d2, 0 nếu d1 = d2, 1 nếu d1 > d2
int compareDate(const Date* d1, const Date* d2);

// Tính số ngày giữa 2 ngày
int daysBetween(const Date* start, const Date* end);

// Chuyển ngày thành chuỗi (format: DD/MM/YYYY)
void dateToString(const Date* date, char* buffer);

#endif 