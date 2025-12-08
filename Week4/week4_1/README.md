# Symbol Table Project

## Mô tả
Đây là project thực hành quản lý bảng ký hiệu (symbol table) cho môn IT3323. Dự án gồm các file mã nguồn C để xây dựng, quản lý và kiểm tra bảng ký hiệu, bao gồm các chức năng như thêm, tìm kiếm, và giải phóng bộ nhớ cho các kiểu dữ liệu.

## Cấu trúc thư mục
```
incompleted/
  debug.c
  debug.h
  main.c
  Makefile
  symtab.c
  symtab.h
```

- `main.c`: Chương trình chính, kiểm thử các chức năng của bảng ký hiệu.
- `symtab.c`, `symtab.h`: Định nghĩa và cài đặt các hàm quản lý bảng ký hiệu.
- `debug.c`, `debug.h`: Các hàm hỗ trợ debug.
- `Makefile`: Tập tin build project bằng lệnh `mingw32-make`.

## Hướng dẫn sử dụng
1. Build chương trình:
   ```sh
   mingw32-make
   ```
2. Chạy chương trình:
   ```sh
   ./symtab
   ```