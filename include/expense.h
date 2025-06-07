#ifndef EXPENSE_H
#define EXPENSE_H

#include "date.h"
#include "GIADIEN.h"


// Cấu trúc hóa đơn tiền điện
struct bill {
    char customer_id[20];  // Mã số khách hàng
    int term;              // Kỳ thu phí
    int consumption;       // Điện năng tiêu thụ
    float amount;          // Tiền điện
};

// Tính tiền điện theo bậc thang
float calc_amount(int consumption, struct tariff *tariffs, int tariff_count);

// Tính tiền điện cho tất cả khách hàng và ghi vào file HOADON.BIN  
int process_bills(void);

#endif
// Tính toán chi phí tiền điện theo bậc thang