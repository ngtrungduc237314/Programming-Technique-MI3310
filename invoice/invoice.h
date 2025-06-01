#ifndef INVOICE_H
#define INVOICE_H

#include <stdio.h>

// Cau truc hoa don
struct invoice_record {
    char ID[20];
    char Name[50];
    char Address[100];
    char Meter[20];
    int term;
    char from_date[30];
    char to_date[30];
    int consumption;
    float amount;
    float tax;
    float total;
    char amount_text[256];
};


// Ham chuyen so thanh chu (ho tro den 1 trieu)
void number_to_text(int n, char *buf);

// In hoa don ra man hinh
void print_invoice(const char *customer_id);

// Xuat hoa don ra file
void write_invoice_to_file(const char *customer_id, FILE *f);

// Nhap ma khach hang, in hoa don ra man hinh va file
void full_invoice(void);

#endif