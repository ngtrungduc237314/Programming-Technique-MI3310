#include "invoice.h"
#include "../file_KH/KH.h"
#include "../file_CSDIEN/CSDIEN.h"
#include "expense/expense.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Ham chuyen so thanh chu (ho tro den 1 trieu)
void number_to_text(int n, char *buf) {
    const char *ones[] = {"khong", "mot", "hai", "ba", "bon", "nam", "sau", "bay", "tam", "chin"};
    char temp[256] = "";
    
    if (n == 0) {
        strcpy(buf, "khong");
        return;
    }

    // Xử lý hàng trăm nghìn
    if (n >= 100000) {
        sprintf(temp + strlen(temp), "%s tram ", ones[n / 100000]);
        n %= 100000;
        
        // Thêm từ "không" nếu số hàng chục nghìn = 0 và có số hàng nghìn
        if ((n / 1000) % 10 == 0 && n >= 1000) {
            strcat(temp, "khong muoi ");
        }
    }

    // Xử lý hàng chục nghìn
    if (n >= 10000) {
        if ((n / 10000) == 1)
            strcat(temp, "muoi ");
        else
            sprintf(temp + strlen(temp), "%s muoi ", ones[n / 10000]);
        n %= 10000;
    }

    // Xử lý hàng nghìn
    if (n >= 1000) {
        if (n / 1000 == 5)
            strcat(temp, "lam ");
        else
            sprintf(temp + strlen(temp), "%s ", ones[n / 1000]);
        strcat(temp, "nghin ");
        n %= 1000;
        
        // Thêm từ "không trăm" nếu số hàng trăm = 0 và có số hàng chục hoặc đơn vị
        if (n > 0 && n < 100) {
            strcat(temp, "khong tram ");
        }
    }

    // Xử lý hàng trăm
    if (n >= 100) {
        sprintf(temp + strlen(temp), "%s tram ", ones[n / 100]);
        n %= 100;
        
        // Thêm từ "lẻ" nếu số hàng chục = 0 và có số hàng đơn vị
        if (n > 0 && n < 10) {
            strcat(temp, "le ");
        }
    }

    // Xử lý hàng chục
    if (n >= 10) {
        if (n / 10 == 1)
            strcat(temp, "muoi ");
        else
            sprintf(temp + strlen(temp), "%s muoi ", ones[n / 10]);
        n %= 10;
    }

    // Xử lý hàng đơn vị
    if (n > 0) {
        // Dùng "lăm" thay vì "năm" khi đứng sau chục
        if (n == 5 && strlen(temp) > 0)
            strcat(temp, "lam");
        else
            sprintf(temp + strlen(temp), "%s", ones[n]);
    }

    strcpy(buf, temp);
}


// In hoa don ra man hinh
void print_invoice(const char *customer_id) {
    FILE *fbill = fopen("../expense/HOADON.BIN", "rb");
    FILE *fkh = fopen("../file_KH/KH.BIN", "rb");
    FILE *fcs = fopen("../file_CSDIEN/CSDIEN.BIN", "rb");
    if (!fbill || !fkh || !fcs) {
        printf("Khong mo duoc file du lieu hoa don hoac khach hang hoac chi so dien!\n");
        if (fbill) fclose(fbill);
        if (fkh) fclose(fkh);
        if (fcs) fclose(fcs);
        return;
    }

    // Doc danh sach khach hang
    struct customer kh[100];
    int n_kh = fread(kh, sizeof(struct customer), 100, fkh);
    fclose(fkh);

    // Doc danh sach chi so dien
    struct eindex cs[100];
    int n_cs = fread(cs, sizeof(struct eindex), 100, fcs);
    fclose(fcs);

    // Doc danh sach hoa don
    struct {
        char ID[20];
        char Name[50];
        int term;
        int consumption;
        float amount;
    } bills[1000];
    int n_bills = fread(bills, sizeof(bills[0]), 1000, fbill);
    fclose(fbill);

    int found = 0;
    for (int i = 0; i < n_bills; i++) {
        if (strcmp(bills[i].ID, customer_id) == 0) {
            struct invoice_record inv = {0};
            strcpy(inv.ID, bills[i].ID);
            strcpy(inv.Name, bills[i].Name);
            inv.term = bills[i].term;
            inv.consumption = bills[i].consumption;
            inv.amount = bills[i].amount;
            inv.tax = inv.amount * 0.1f;
            inv.total = inv.amount + inv.tax;
            number_to_text((int)inv.total, inv.amount_text);

             // Sửa phần in số tiền bằng chữ: chữ cái đầu viết hoa, kết thúc bằng "dong."
            char amount_text_cap[256];
            strcpy(amount_text_cap, inv.amount_text);
            if (strlen(amount_text_cap) > 0 && amount_text_cap[0] >= 'a' && amount_text_cap[0] <= 'z') {
                amount_text_cap[0] = amount_text_cap[0] - ('a' - 'A');
            }
            
            // Tim thong tin khach hang
            for (int k = 0; k < n_kh; k++) {
                if (strcmp(kh[k].ID, inv.ID) == 0) {
                    strcpy(inv.Address, kh[k].Address);
                    strcpy(inv.Meter, kh[k].Meter);
                    break;
                }
            }
            // Tim ngay chot chi so dau va cuoi ky
            inv.from_date[0] = inv.to_date[0] = 0;
            for (int c = 0; c < n_cs; c++) {
                if (strcmp(cs[c].ID, inv.ID) == 0 && cs[c].term == inv.term - 1) {
                    strcpy(inv.from_date, cs[c].closing);
                }
                if (strcmp(cs[c].ID, inv.ID) == 0 && cs[c].term == inv.term) {
                    strcpy(inv.to_date, cs[c].closing);
                }
            }

            // In hoa don ra man hinh
            printf("========================================\n");
            printf("HOA DON TIEN DIEN\n");
            printf("Ma KH: %s\n", inv.ID);
            printf("Ten KH: %s\n", inv.Name);
            printf("Dia chi: %s\n", inv.Address);
            printf("Ma cong to: %s\n", inv.Meter);
            printf("Ky: %d\n", inv.term);
            printf("Tu ngay: %s\n", inv.from_date);
            printf("Den ngay: %s\n", inv.to_date);
            printf("Dien nang tieu thu: %d kWh\n", inv.consumption);
            printf("Tien dien: %.2f VND\n", inv.amount);
            printf("Thue VAT (10%%): %.2f VND\n", inv.tax);
            printf("Tong cong thanh toan: %.2f VND\n", inv.total);
            printf("So tien bang chu: %s dong.\n", amount_text_cap);
            printf("========================================\n\n");
            found = 1;
        }
    }
    if (!found) {
        printf("Khong tim thay khach hang!\n");
    }
}

// Xuat hoa don ra file
void write_invoice_to_file(const char *customer_id, FILE *f) {
    FILE *fbill = fopen("../expense/HOADON.BIN", "rb");
    FILE *fkh = fopen("../file_KH/KH.BIN", "rb");
    FILE *fcs = fopen("../file_CSDIEN/CSDIEN.BIN", "rb");
    if (!fbill || !fkh || !fcs) {
        fprintf(f, "Khong mo duoc file du lieu hoa don hoac khach hang hoac chi so dien!\n");
        if (fbill) fclose(fbill);
        if (fkh) fclose(fkh);
        if (fcs) fclose(fcs);
        return;
    }

    // Doc danh sach khach hang
    struct customer kh[100];
    int n_kh = fread(kh, sizeof(struct customer), 100, fkh);
    fclose(fkh);

    // Doc danh sach chi so dien
    struct eindex cs[100];
    int n_cs = fread(cs, sizeof(struct eindex), 100, fcs);
    fclose(fcs);

    // Doc danh sach hoa don
    struct {
        char ID[20];
        char Name[50];
        int term;
        int consumption;
        float amount;
    } bills[1000];
    int n_bills = fread(bills, sizeof(bills[0]), 1000, fbill);
    fclose(fbill);

    int found = 0;
    for (int i = 0; i < n_bills; i++) {
        if (strcmp(bills[i].ID, customer_id) == 0) {
            struct invoice_record inv = {0};
            strcpy(inv.ID, bills[i].ID);
            strcpy(inv.Name, bills[i].Name);
            inv.term = bills[i].term;
            inv.consumption = bills[i].consumption;
            inv.amount = bills[i].amount;
            inv.tax = inv.amount * 0.1f;
            inv.total = inv.amount + inv.tax;
            number_to_text((int)inv.total, inv.amount_text);

            char amount_text_cap[256];
            strcpy(amount_text_cap, inv.amount_text);
            if (strlen(amount_text_cap) > 0 && amount_text_cap[0] >= 'a' && amount_text_cap[0] <= 'z') {
                amount_text_cap[0] = amount_text_cap[0] - ('a' - 'A');
            }
            
            // Tim thong tin khach hang
            for (int k = 0; k < n_kh; k++) {
                if (strcmp(kh[k].ID, inv.ID) == 0) {
                    strcpy(inv.Address, kh[k].Address);
                    strcpy(inv.Meter, kh[k].Meter);
                    break;
                }
            }

            // Tim ngay chot chi so dau va cuoi ky
            inv.from_date[0] = inv.to_date[0] = 0;
            for (int c = 0; c < n_cs; c++) {
                if (strcmp(cs[c].ID, inv.ID) == 0 && cs[c].term == inv.term - 1) {
                    strcpy(inv.from_date, cs[c].closing);
                }
                if (strcmp(cs[c].ID, inv.ID) == 0 && cs[c].term == inv.term) {
                    strcpy(inv.to_date, cs[c].closing);
                }
            }

            // In hoa don ra file
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            fprintf(f, "==========================================================\n");
            fprintf(f, "CHUONG TRINH QUAN LY DIEN NANG - HOA DON TIEN DIEN\n");
            fprintf(f, "Thoi gian: %02d/%02d/%04d %02d:%02d:%02d\n", 
                    t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
                    t->tm_hour, t->tm_min, t->tm_sec);
            fprintf(f, "==========================================================\n\n");
            fprintf(f, "HOA DON TIEN DIEN\n");
            fprintf(f, "Ma KH: %s\n", inv.ID);
            fprintf(f, "Ten KH: %s\n", inv.Name);
            fprintf(f, "Dia chi: %s\n", inv.Address);
            fprintf(f, "Ma cong to: %s\n", inv.Meter);
            fprintf(f, "Ky: %d\n", inv.term);
            fprintf(f, "Tu ngay: %s\n", inv.from_date);
            fprintf(f, "Den ngay: %s\n", inv.to_date);
            fprintf(f, "Dien nang tieu thu: %d kWh\n", inv.consumption);
            fprintf(f, "Tien dien: %.2f VND\n", inv.amount);
            fprintf(f, "Thue VAT (10%%): %.2f VND\n", inv.tax);
            fprintf(f, "Tong cong thanh toan: %.2f VND\n", inv.total);
            fprintf(f, "So tien bang chu: %s dong.\n", amount_text_cap);
            fprintf(f, "==========================================================\n\n");
            found = 1;
        }
    }
    if (!found) {
        fprintf(f, "Khong tim thay khach hang!\n");
    }
}

//Nhap ma khach hang, in hoa don ra man hinh va file
void full_invoice(void) {
    char customer_id[20];
    printf("Nhap ma khach hang: ");
    scanf("%s", customer_id);
    
    // In hóa đơn ra màn hình
    print_invoice(customer_id);
    
    // Xuất hóa đơn ra file
    FILE *f = fopen("invoice.txt", "w");
    if (f) {
        write_invoice_to_file(customer_id, f);
        fclose(f);
        printf("\nDa xuat hoa don ra file invoice.txt\n");
    } else {
        printf("\nKhong the mo file invoice.txt de ghi!\n");
    }
}