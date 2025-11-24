# Chương trình tạo Index từ văn bản

## Mô tả
Chương trình đọc file văn bản và tạo index các từ xuất hiện, bỏ qua stop words và danh từ riêng.

## Yêu cầu
- GCC compiler
- MinGW Make (cho Windows)

## Biên dịch

```bash
mingw32-make
```

## Chạy chương trình

```bash
# Với file mặc định
mingw32-make run

# Với file tùy chỉnh
mingw32-make run INPUT=vanban.txt OUTPUT=result.txt

# Hoặc chạy trực tiếp
./index.exe <input_file> <output_file>
```

## Ví dụ

```bash
./index.exe alice30.txt output.txt
```

## Định dạng output

Mỗi dòng có định dạng:
```
từ          số_lần_xuất_hiện, dòng1, dòng2, dòng3, ...
```

Ví dụ:
```
alice        15, 1, 3, 5, 7, 9, 12, 15, 18, 21, 23, 25, 27, 29, 30
garden       3, 2, 8, 14
```

## Dọn dẹp

```bash
mingw32-make clean
```

## File cần thiết
- `stopw.txt` - Danh sách stop words
- File văn bản input (ví dụ: `alice30.txt`)
