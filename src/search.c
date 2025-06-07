#include "search.h"
#include "KH.h"
#include "CSDIEN.h"
#include "GIADIEN.h" 
#include "date.h"
#include "cons.h"     // Thêm include để sử dụng struct consumption_record
#include "validation.h"  // Thêm header cho các hàm validation

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Khởi tạo danh sách
static struct search_list* init_search_list() {
    struct search_list* list = (struct search_list*)malloc(sizeof(struct search_list));
    if (!list) return NULL;
    list->head = NULL;
    list->count = 0;
    return list;
}

// Thêm kết quả vào danh sách
static int add_result(struct search_list* list, const struct search_result* result) {
    if (!list || !result) return -1;

    struct search_node* new_node = (struct search_node*)malloc(sizeof(struct search_node));
    if (!new_node) return -1;

    new_node->data = *result;
    new_node->next = list->head;
    list->head = new_node;
    list->count++;
    return 0;
}

// Giải phóng danh sách
static void free_search_list(struct search_list* list) {
    if (!list) return;

    struct search_node* current = list->head;
    while (current != NULL) {
        struct search_node* next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

// Hàm đọc dữ liệu từ file và lưu vào linked list
static struct search_list* read_data() {
    FILE *fkh = fopen("KH.BIN", "rb");
    FILE *fcs = fopen("CSDIEN.BIN", "rb");
    FILE *ftt = fopen("TIEUTHU.BIN", "rb");
    
    if (!fkh || !fcs || !ftt) {
        if (fkh) fclose(fkh);
        if (fcs) fclose(fcs);
        if (ftt) fclose(ftt);
        return NULL;
    }

    struct search_list* list = init_search_list();
    if (!list) {
        fclose(fkh);
        fclose(fcs);
        fclose(ftt);
        return NULL;
    }

    struct customer kh;
    struct eindex cs;
    struct consumption_record tt;

    // Đọc từng record một
    while (fread(&kh, sizeof(struct customer), 1, fkh) == 1) {
        // Reset file pointer của CSDIEN và TIEUTHU
        fseek(fcs, 0, SEEK_SET);
        fseek(ftt, 0, SEEK_SET);

        while (fread(&cs, sizeof(struct eindex), 1, fcs) == 1) {
            if (strcmp(kh.ID, cs.ID) == 0) {
                // Tìm thông tin tiêu thụ tương ứng
                int found_consumption = 0;
                while (fread(&tt, sizeof(struct consumption_record), 1, ftt) == 1) {
                    if (strcmp(tt.ID, kh.ID) == 0 && tt.term == cs.term) {
                        struct search_result result;
                        strcpy(result.ID, kh.ID);
                        strcpy(result.Name, kh.Name);
                        strcpy(result.Address, kh.Address);
                        strcpy(result.Meter, kh.Meter);
                        result.term = cs.term;
                        result.closing_date = cs.closing_date;  // Sử dụng trực tiếp closing_date
                        result.index = cs.index;
                        result.consumption = tt.consumption;
                        result.days = tt.days;

                        if (add_result(list, &result) != 0) {
                            free_search_list(list);
                            fclose(fkh);
                            fclose(fcs);
                            fclose(ftt);
                            return NULL;
                        }
                        found_consumption = 1;
                        break;
                    }
                }
                
                if (!found_consumption) {
                    struct search_result result;
                    strcpy(result.ID, kh.ID);
                    strcpy(result.Name, kh.Name);
                    strcpy(result.Address, kh.Address);
                    strcpy(result.Meter, kh.Meter);
                    result.term = cs.term;
                    result.closing_date = cs.closing_date;  // Sử dụng trực tiếp closing_date
                    result.index = cs.index;
                    result.consumption = -1;
                    result.days = 0;

                    if (add_result(list, &result) != 0) {
                        free_search_list(list);
                        fclose(fkh);
                        fclose(fcs);
                        fclose(ftt);
                        return NULL;
                    }
                }
            }
        }
    }

    fclose(fkh);
    fclose(fcs);
    fclose(ftt);
    return list;
}

// Hàm in kết quả tìm kiếm
static void print_result(const struct search_result* result) {
    printf("\nThông tin khách hàng:\n");
    printf("Mã KH: %s\n", result->ID);
    printf("Tên KH: %s\n", result->Name);
    printf("Địa chỉ: %s\n", result->Address);
    printf("Mã công tơ: %s\n", result->Meter);
    printf("Kỳ thu phí: %d\n", result->term);
    printf("Ngày chốt chỉ số: %02d/%02d/%04d\n", 
           result->closing_date.day,
           result->closing_date.month,
           result->closing_date.year);
    printf("Chỉ số điện: %d\n", result->index);
    if (result->consumption >= 0) {
        printf("Điện năng tiêu thụ: %d\n", result->consumption);
        printf("Số ngày tiêu thụ: %d\n", result->days);
    } else {
        printf("Chưa có dữ liệu tiêu thụ\n");
    }
    printf("\n");
}

// Ham in ket qua tim kiem ra file
static void print_result_to_file(const struct search_result* result, FILE* f) {
    // Ghi thoi gian thuc hien
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(f, "Thoi gian thuc hien: %02d/%02d/%04d %02d:%02d:%02d\n",
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec);
    fprintf(f, "----------------------------------------\n");

    // Ghi thong tin khach hang
    fprintf(f, "THONG TIN KHACH HANG:\n");
    fprintf(f, "Ma KH: %s\n", result->ID);
    fprintf(f, "Ten KH: %s\n", result->Name);
    fprintf(f, "Dia chi: %s\n", result->Address);
    fprintf(f, "Ma cong to: %s\n", result->Meter);

    // Ghi thong tin chi so dien
    fprintf(f, "\nTHONG TIN CHI SO DIEN:\n");
    fprintf(f, "Ky thu phi: %d\n", result->term);
    fprintf(f, "Ngay chot chi so: %02d/%02d/%04d\n",
            result->closing_date.day,
            result->closing_date.month,
            result->closing_date.year);
    fprintf(f, "Chi so dien: %d\n", result->index);

    // Ghi thong tin tieu thu
    fprintf(f, "\nTHONG TIN TIEU THU:\n");
    if (result->consumption >= 0) {
        fprintf(f, "Dien nang tieu thu: %d kWh\n", result->consumption);
        fprintf(f, "So ngay tieu thu: %d ngay\n", result->days);
        if (result->days > 0) {
            fprintf(f, "Trung binh: %.2f kWh/ngay\n", 
                    (float)result->consumption / result->days);
        }
    } else {
        fprintf(f, "Chua co du lieu tieu thu\n");
    }
    fprintf(f, "----------------------------------------\n\n");
}

// Tim kiem theo ma khach hang
int search_by_id(const char* customer_id) {
    if (!is_valid_customer_id(customer_id)) {
        return -3;
    }

    // Mo file log
    FILE* flog = fopen("search.txt", "w");
    if (!flog) {
        return -1;
    }

    // Ghi thong tin tim kiem
    fprintf(flog, "TIM KIEM THEO MA KHACH HANG\n");
    fprintf(flog, "Ma KH can tim: %s\n\n", customer_id);

    struct search_list* list = read_data();
    if (!list) {
        fprintf(flog, "Loi: Khong the doc du lieu tu file!\n");
        fclose(flog);
        return -1;
    }

    int found = 0;
    struct search_node* current = list->head;
    while (current != NULL) {
        if (strcmp(current->data.ID, customer_id) == 0) {
            print_result(&current->data);
            print_result_to_file(&current->data, flog);
            found++;
        }
        current = current->next;
    }

    // Ghi ket qua tim kiem
    fprintf(flog, "KET QUA TIM KIEM:\n");
    fprintf(flog, "- So ket qua tim thay: %d\n", found);
    if (found == 0) {
        fprintf(flog, "- Khong tim thay khach hang co ma %s\n", customer_id);
    }

    free_search_list(list);
    fclose(flog);
    return found ? 0 : -2;
}

/*
// Tim kiem theo ma khach hang va ky
int search_by_id_and_term(const char* customer_id, int term) {
    if (!is_valid_customer_id(customer_id) || !is_valid_term(term)) {
        return -3;
    }

    // Mo file log
    FILE* flog = fopen("search.txt", "w");
    if (!flog) {
        return -1;
    }

    // Ghi thong tin tim kiem
    fprintf(flog, "TIM KIEM THEO MA KHACH HANG VA KY\n");
    fprintf(flog, "Ma KH can tim: %s\n", customer_id);
    fprintf(flog, "Ky can tim: %d\n\n", term);

    struct search_list* list = read_data();
    if (!list) {
        fprintf(flog, "Loi: Khong the doc du lieu tu file!\n");
        fclose(flog);
        return -1;
    }

    int found = 0;
    struct search_node* current = list->head;
    while (current != NULL) {
        if (strcmp(current->data.ID, customer_id) == 0 && current->data.term == term) {
            print_result(&current->data);
            print_result_to_file(&current->data, flog);
            found++;
            break;
        }
        current = current->next;
    }

    // Ghi ket qua tim kiem
    fprintf(flog, "KET QUA TIM KIEM:\n");
    fprintf(flog, "- So ket qua tim thay: %d\n", found);
    if (found == 0) {
        fprintf(flog, "- Khong tim thay khach hang co ma %s tai ky %d\n", customer_id, term);
    }

    free_search_list(list);
    fclose(flog);
    return found ? 0 : -2;
}

// Tim kiem theo ten khach hang
int search_by_name(const char* customer_name) {
    if (!is_valid_customer_name(customer_name)) {
        return -3;
    }

    // Mo file log
    FILE* flog = fopen("search.txt", "w");
    if (!flog) {
        return -1;
    }

    // Ghi thong tin tim kiem
    fprintf(flog, "TIM KIEM THEO TEN KHACH HANG\n");
    fprintf(flog, "Ten KH can tim: %s\n", customer_name);
    fprintf(flog, "Che do tim kiem: Tim kiem mo (khong phan biet hoa thuong)\n\n");

    struct search_list* list = read_data();
    if (!list) {
        fprintf(flog, "Loi: Khong the doc du lieu tu file!\n");
        fclose(flog);
        return -1;
    }

    int found = 0;
    struct search_node* current = list->head;
    while (current != NULL) {
        char name_lower[50], search_lower[50];
        strcpy(name_lower, current->data.Name);
        strcpy(search_lower, customer_name);
        strlwr(name_lower);
        strlwr(search_lower);
        
        if (strstr(name_lower, search_lower)) {
            print_result(&current->data);
            print_result_to_file(&current->data, flog);
            found++;
        }
        current = current->next;
    }

    // Ghi ket qua tim kiem
    fprintf(flog, "KET QUA TIM KIEM:\n");
    fprintf(flog, "- So ket qua tim thay: %d\n", found);
    if (found == 0) {
        fprintf(flog, "- Khong tim thay khach hang co ten chua '%s'\n", customer_name);
    }

    free_search_list(list);
    fclose(flog);
    return found ? 0 : -2;
}
*/