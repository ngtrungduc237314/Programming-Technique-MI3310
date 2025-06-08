#include "input.h"
#include "KH.h"

// Kiểm tra input là số
int isValidNumber(const char* str) {
    if (str == NULL || *str == '\0') return 0;
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

// Xóa buffer
void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Đọc input an toàn
int safeInput(char* buffer, size_t size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
            return 1;
        }
        // Buffer overflow
        if (len == size - 1) {
            clearInputBuffer();
            return 0;
        }
    }
    return 0;
} 

// Kiểm tra độ dài chuỗi có phù hợp với buffer không
int isValidLength(const char* str, size_t maxLen) {
    if (str == NULL) return 0;
    size_t len = strlen(str);
    // Trừ 1 cho ký tự null terminator
    return (len > 0 && len < maxLen);
}

// Kiểm tra mã khách hàng có hợp lệ không (format KHXXXXXX)
int isValidCustomerId(const char* id) {
    if (id == NULL || strlen(id) != 8) return 0;
    
    // Kiểm tra 2 ký tự đầu là "KH"
    if (id[0] != 'K' || id[1] != 'H') return 0;
    
    // Kiểm tra 6 ký tự sau là số
    for (int i = 2; i < 8; i++) {
        if (!isdigit(id[i])) return 0;
    }
    
    return 1;
}

// Kiểm tra mã công tơ có hợp lệ không (format CTXXXXXX)
int isValidMeterNumber(const char* meter) {
    if (meter == NULL || strlen(meter) != 8) return 0;
    
    // Kiểm tra 2 ký tự đầu là "CT"
    if (meter[0] != 'C' || meter[1] != 'T') return 0;
    
    // Kiểm tra 6 ký tự sau là số
    for (int i = 2; i < 8; i++) {
        if (!isdigit(meter[i])) return 0;
    }
    
    return 1;
}

// Kiểm tra tên khách hàng có hợp lệ không
int isValidCustomerName(const char* name) {
    if (name == NULL || *name == '\0') return 0;  // Tên rỗng
    
    int hasLetter = 0;  // Biến đánh dấu có ký tự chữ cái
    
    for (int i = 0; name[i] != '\0'; i++) {
        if (isalpha(name[i])) {
            hasLetter = 1;    // Có ký tự chữ cái
        } else if (name[i] != ' ') {
            // Chỉ cho phép chữ cái và khoảng trắng
            return 0;
        }
    }
    
    // Tên hợp lệ khi có ít nhất 1 chữ cái
    return hasLetter;
}

// Kiểm tra địa chỉ có hợp lệ không
int isValidAddress(const char* address) {
    if (address == NULL || *address == '\0') return 0;  // Địa chỉ rỗng
    
    size_t len = strlen(address);
    if (len < 10) return 0;  // Địa chỉ quá ngắn
    
    int hasLetter = 0;  // Biến đánh dấu có ký tự chữ cái
    
    for (int i = 0; address[i] != '\0'; i++) {
        char c = address[i];
        if (isalpha(c)) {
            hasLetter = 1;
        } else if (!isdigit(c) && !isspace(c) && c != ',' && c != '.' && c != '/') {
            // Chỉ cho phép: chữ cái, số, khoảng trắng, dấu phẩy, dấu chấm, dấu /
            return 0;
        }
    }
    
    // Địa chỉ hợp lệ khi có ít nhất 1 chữ cái
    return hasLetter;
}


// Kiểm tra mã khách hàng đã tồn tại chưa
ErrorCode isCustomerIdExists(const char* id) {
    if (id == NULL) return ERR_DATA_INVALID;
    
    FILE* fp = fopen("KH.BIN", "rb");
    if (fp == NULL) return ERR_FILE_OPEN;
    
    struct customer temp;
    while (fread(&temp, sizeof(struct customer), 1, fp) == 1) {
        if (strcmp(temp.ID, id) == 0) {
            fclose(fp);
            return ERR_DATA_FOUND; // Đã tồn tại
        }
    }
    
    fclose(fp);
    return SUCCESS; // Chưa tồn tại
}

// Kiểm tra mã công tơ đã được sử dụng chưa
ErrorCode isMeterNumberExists(const char* meter) {
    if (meter == NULL) return ERR_DATA_INVALID;
    
    FILE* fp = fopen("KH.BIN", "rb");
    if (fp == NULL) return ERR_FILE_OPEN;
    
    struct customer temp;
    while (fread(&temp, sizeof(struct customer), 1, fp) == 1) {
        if (strcmp(temp.Meter, meter) == 0) {
            fclose(fp);
            return ERR_DATA_FOUND; // Đã được sử dụng
        }
    }
    
    fclose(fp);
    return SUCCESS; // Chưa được sử dụng
}

// Kiểm tra kỳ thu phí có hợp lệ không
int isValidTerm(int term) {
    return (term >= 1 && term <= 12);
}

