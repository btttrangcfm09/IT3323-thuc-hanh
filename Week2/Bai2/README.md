# IT3323 Thực Hành

## Mô tả
Đây là repository chứa các bài thực hành cho môn IT3323.

## Cấu trúc thư mục
- `incompleted/`: Chứa các file mã nguồn và header.
- `test/`: Chứa các file test và kết quả tương ứng.

## Hướng dẫn sử dụng
1. Biên dịch mã nguồn bằng Makefile trong thư mục `incompleted/`.
2. Chạy các file test trong thư mục `test/`.

## Chạy chương trình với Windows
1. Điều hướng đến thư mục `incompleted`:
   ```
   cd incompleted
   ```
2. Sử dụng `mingw32-make` để biên dịch:
   ```
   mingw32-make
   ```
3. Chọn file test đi kèm với scanner và chạy chương trình:
   ```
   ./scanner ../test/<file_name>
   ```
4. Với bài 2 test là file example8.kpl và result8.txt
