# Programming-Technique-MI3310
Project for Programming Technique MI3310 


## Hướng dẫn Build

### Yêu cầu:
Bài 2. Viết chương trình thực hiện các chức năng sau:
1) Bổ sung, sửa chữa, xóa dữ liệu cho 3 file nhị phân:
+ File KH.BIN lưu thông tin về mã số khách hàng, tên khách hàng, địa chỉ, mã số
công tơ của khách hàng (giả thiết mỗi mã khách hàng dùng một công tơ). Thao tác
nhập dữ liệu đảm bảo không có 2 bản ghi giống nhau về Mã số khách hàng.
+ File CSDIEN.BIN chứa các thông tin về Mã số khách hàng, Chỉ số điện, Ngày chốt
chỉ số điện, Kỳ thu phí. Thao tác nhập dữ liệu đảm bảo không trùng lặp trên đồng thời
cả Mã số khách hàng và Kỳ thu phí.
+ File GIADIEN.BIN chứa thông tin về bảng giá tính tiền điện, gồm: Điện năng tiêu
thụ và Đơn giá điện theo bậc.
3) Tính điện năng tiêu thụ của một khách hàng trong một kỳ, mỗi kỳ ứng với một
tháng, được tính từ Ngày đầu kỳ (là ngày chốt chỉ số điện tháng trước) đến Ngày cuối
kỳ (là ngày chốt chỉ số điện của tháng hiện thời cần tính tiền điện).
4) Tìm kiếm thông tin về khách hàng, tra cứu điện năng tiêu thụ trong kỳ của khách.
5) Tính tiền điện trong kỳ theo giá bậc thang được quy định trong file GIADIEN.BIN
+ Ghi các kết quả tính được vào file nhị phân HOADON.BIN gồm các thông tin: mã
số khách hàng, kỳ (kỳ thu phí), điện năng tiêu thụ, tiền điện.
6) Nhập vào mã số khách hàng và in ra màn hình hóa đơn tiền điện của khách hàng đó
(gồm Mã khách hàng, Tên khách hàng, Địa chỉ, Mã số công tơ, Kỳ, Từ ngày (ngày
đầu kỳ), Đến ngày (ngày cuối kỳ), Điện năng tiêu thụ, Tiền điện, Thuế (10%), Tổng
cộng tiền thanh toán, Số tiền viết bằng chữ.
Yêu cầu:
- Điều khiển chương trình bằng menu sử dụng các phím chức năng.
- Ghi vào tệp văn bản để thể hiện toàn bộ quá trình thực hiện chương trình và các
kết quả vào, ra.

### Makefiles
- Đã cài đặt [CMake](https://cmake.org/download/)
- Đã cài MinGW [Hướng dẫn](https://codecute.com/c/huong-dan-cai-dat-trinh-bien-dich-c-c-mingw-gcc.html) và cài đặt Make. Thêm thư mục `bin` của MinGW và thư mục có chứa `Make` vào biến môi trường `PATH` 

### Các bước build (Sử dụng Terminal Powershell):

```bash
git clone https://github.com/qanhta2710/BTLCTDL.git
cd BTLCTDL
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```
Sau khi build thành công, chương trình program.exe nằm trong thư mục `build`
