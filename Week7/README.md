# Chương trình Index từ văn bản

Chương trình C tạo index các từ từ file văn bản, loại bỏ stop words và danh từ riêng, sau đó xuất kết quả ra file với số lần xuất hiện và vị trí các dòng.

## Yêu cầu hệ thống

- GCC Compiler
- MinGW Make (cho Windows)
- Windows PowerShell

## Cấu trúc project

```
IT3323-thuc-hanh/
├── indexer.c       # Mã nguồn chính
├── Makefile        # File build
├── stopw.txt       # Danh sách stop words
├── alice30.txt     # File văn bản mẫu
├── output.txt      # File kết quả (được tạo sau khi chạy)
└── README.md       # File này
```

## Hướng dẫn cài đặt

### 1. Cài đặt GCC (nếu chưa có)

Download và cài đặt MinGW hoặc TDM-GCC từ:
- MinGW: https://www.mingw-w64.org/
- TDM-GCC: https://jmeubank.github.io/tdm-gcc/

### 2. Cài đặt Make

Sử dụng mingw32-make (đi kèm với MinGW) hoặc cài qua Chocolatey:

```powershell
choco install make
```

## Hướng dẫn sử dụng

### Biên dịch chương trình

```powershell
mingw32-make
```

Lệnh này sẽ tạo file `index.exe` từ `indexer.c`.

### Chạy chương trình

**Cách 1: Dùng Make với file mặc định**

```powershell
mingw32-make run
```

Chạy với file input mặc định `alice30.txt` và output `output.txt`.

**Cách 2: Dùng Make với file tùy chỉnh**

```powershell
mingw32-make run INPUT=vanban.txt OUTPUT=result.txt
```

**Cách 3: Chạy trực tiếp**

```powershell
./index.exe <input_file> <output_file>
```

Ví dụ:
```powershell
./index.exe alice30.txt output.txt
./index.exe thuchanh1.txt result.txt
```

### Dọn dẹp file

```powershell
mingw32-make clean
```

Xóa file thực thi và file object.

## Định dạng Input/Output

### File Input

- File văn bản thuần (text file)
- Mã hóa: UTF-8 hoặc ASCII
- Ví dụ: `alice30.txt`, `vanban.txt`

### File Output

Mỗi dòng có định dạng:

```
từ          số_lần_xuất_hiện, dòng1, dòng2, dòng3, ...
```

**Ví dụ:**

```
alice        15, 1, 3, 5, 7, 9, 12, 15, 18, 21, 23, 25, 27, 29, 30
garden       3, 2, 8, 14
rabbit       7, 1, 4, 6, 9, 15, 22, 28
```

**Giải thích:**
- Cột 1: Từ (căn trái, độ rộng 12 ký tự)
- Cột 2: Số lần xuất hiện
- Cột 3+: Danh sách số dòng cách nhau bởi dấu phẩy

## File Stop Words

File `stopw.txt` chứa danh sách các từ cần loại bỏ (một từ mỗi dòng):

```
a
an
the
is
are
...
```

## Quy tắc xử lý

1. **Chuyển sang chữ thường**: Tất cả từ được chuyển sang lowercase
2. **Loại bỏ stop words**: Các từ trong `stopw.txt` không được index
3. **Loại bỏ danh từ riêng**: Từ viết hoa không đứng sau dấu chấm bị bỏ qua
4. **Sắp xếp alphabet**: Kết quả được sắp xếp theo thứ tự bảng chữ cái

## Ví dụ sử dụng đầy đủ

```powershell
# 1. Biên dịch
PS D:\IT3323-thuc-hanh> mingw32-make
gcc -Wall -Wextra -O2 indexer.c -o index

# 2. Chạy với file mặc định
PS D:\IT3323-thuc-hanh> mingw32-make run
./index alice30.txt output.txt
Successfully processed 245 words from 'alice30.txt'
Results written to 'output.txt'

# 3. Chạy với file khác
PS D:\IT3323-thuc-hanh> ./index.exe vanban.txt ketqua.txt
Successfully processed 182 words from 'vanban.txt'
Results written to 'ketqua.txt'

# 4. Dọn dẹp
PS D:\IT3323-thuc-hanh> mingw32-make clean
Cleaned
```

## Xử lý lỗi

### Lỗi: "cannot open stopw.txt"

**Nguyên nhân**: Thiếu file stopw.txt

**Giải pháp**: Tạo file stopw.txt với các stop words, một từ mỗi dòng

### Lỗi: "cannot open input file"

**Nguyên nhân**: File input không tồn tại hoặc đường dẫn sai

**Giải pháp**: Kiểm tra lại tên file và đảm bảo file tồn tại trong thư mục

### Lỗi: "cannot create output file"

**Nguyên nhân**: Không có quyền ghi hoặc thư mục không tồn tại

**Giải pháp**: Kiểm tra quyền ghi và đường dẫn thư mục output

## Giới hạn

- Tối đa 10,000 từ khác nhau
- Độ dài tối đa mỗi từ: 1,000 ký tự
- Độ dài tối đa mỗi dòng: 10,000 ký tự
- Độ dài tối đa danh sách vị trí: 3,000 ký tự

## Tác giả

IT3323 - Thực hành

## License

Educational purposes only
