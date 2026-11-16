cmake_minimum_required(VERSION 3.10)
project(Program C)

include_directories(include)

# Logic nghiep vu (khong phu thuoc giao dien) - dung chung cho chuong trinh va test
set(CORE_SOURCES
    src/cons.c src/CSDIEN.c src/date.c src/expense.c src/GIADIEN.c
    src/input.c src/invoice.c src/KH.c src/search.c)

# Chuong trinh chinh dung conio.h / windows.h nen chi build tren Windows
if(WIN32)
    add_executable(program src/main.c src/UI.c ${CORE_SOURCES})
endif()

# Unit test - chay duoc tren moi he dieu hanh
enable_testing()
add_executable(test_core tests/test_core.c ${CORE_SOURCES})
if(NOT MSVC)
    target_link_libraries(test_core m)
endif()
add_test(NAME test_core COMMAND test_core)
