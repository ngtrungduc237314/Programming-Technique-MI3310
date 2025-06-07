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
int open_KH(void); // Mo file KH.BIN    
int add_KH(void); // Them khach hang
int edit_KH(void); // Sua khach hang
int remove_KH(void); // Xoa khach hang

#endif

//Quản lý dữ liệu khách hàng trong file KH.BIN