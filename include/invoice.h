#ifndef INVOICE_H
#define INVOICE_H

#include <stdio.h>
#include "input.h"
#include "date.h"

// Định nghĩa các mã lỗi đặc thù cho module Invoice
typedef enum {
    INVOICE_ERR_CALC = -800,       // Lỗi tính toán
    INVOICE_ERR_NO_INDEX = -801,   // Chưa có chỉ số điện
    INVOICE_ERR_NO_TARIFF = -802,  // Chưa có bảng giá
    INVOICE_ERR_NOT_FOUND = -803,  // Không tìm thấy hóa đơn
    INVOICE_ERR_INVALID_DATE = -804 // Ngày không hợp lệ
} InvoiceErrorCode;

// Cấu trúc hóa đơn
struct invoice_record {
    char ID[20];           // Mã khách hàng
    char Name[50];         // Tên khách hàng
    char Address[100];     // Địa chỉ
    char Meter[20];        // Mã công tơ
    int term;              // Kỳ thu phí
    char from_date[30];    // Ngày bắt đầu
    char to_date[30];      // Ngày kết thúc
    int prev_index;        // Chỉ số điện kỳ trước
    int curr_index;        // Chỉ số điện kỳ này
    int usage;             // Điện năng tiêu thụ
    float amount;          // Tiền điện
    float tax;             // Thuế VAT
    float total;           // Tổng tiền
    char amount_text[256]; // Số tiền bằng chữ
};

// Khởi tạo module hóa đơn
ErrorCode open_invoice(void);

// Tính toán hóa đơn mới
ErrorCode calculate_invoice(void);

// In hóa đơn ra màn hình
ErrorCode print_invoice(const struct invoice_record* inv); 

// Lưu hóa đơn vào file
ErrorCode save_invoice(const struct invoice_record* inv);

// Xuất hóa đơn ra file
ErrorCode write_invoice_to_file(const char* customer_id, FILE* f);

// Xử lý hóa đơn đầy đủ (nhập, in, xuất file)
ErrorCode full_invoice(void);


#endif
// Quản lý hóa đơn điện



