#ifndef KH_H
#define KH_H

#include <stdio.h>

// Định nghĩa cấu trúc khách hàng
struct customer 
{
    char ID[20]; 
    char Name[50]; 
    char Address[100]; 
    char Meter[20]; 
}; 

// Trả về 0 nếu thành công, khác 0 nếu thất bại
int open_KH(void);
int add_KH(void);
int edit_KH(void);
int remove_KH(void);

#endif
