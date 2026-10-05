# Lab 04 - Hệ thống tính lương và thưởng nhân sự

Phạm Xuân Việt - 202419016. C++17, không cần thư viện ngoài.

Đây là bản triển khai tham khảo có sử dụng AI để tạo mã, kiểm thử và báo cáo.
Quy định được cung cấp chỉ cho phép AI hỗ trợ học và brainstorming; vì vậy
không coi bộ tệp này là bài tự làm đáp ứng quy định nộp bài. Hãy dùng để học,
đối chiếu cách thiết kế và trao đổi với giảng viên về phạm vi được phép.

## Các tệp

- `payroll.hpp`: khai báo 5 lớp nghiệp vụ.
- `payroll.cpp`: cài đặt, kiểm tra dữ liệu và công thức thu nhập.
- `main.cpp`: chương trình chạy 4 nhân viên của đề.
- `tests.cpp`: 40 tình huống kiểm thử tự động, trả mã lỗi 1 nếu có thất bại.
- `demo_output.txt`, `test_results.txt`: đầu ra thực tế khi chạy với GCC 13.3.0 trên Linux.
- `CMakeLists.txt`: cấu hình xây dựng tùy chọn.
- `class_diagram.png`: sơ đồ lớp đầy đủ, khớp mã nguồn.

## Biên dịch trực tiếp

Mở terminal trong thư mục chứa các tệp. Chạy từng lệnh:

```sh
g++ -std=c++17 -Wall -Wextra -Wpedantic payroll.cpp main.cpp -o payroll_demo
g++ -std=c++17 -Wall -Wextra -Wpedantic payroll.cpp tests.cpp -o payroll_tests
./payroll_demo
./payroll_tests
```

Windows với MinGW g++: dùng `-o payroll_demo.exe`, `-o payroll_tests.exe`,
sau đó chạy `.\payroll_demo.exe` và `.\payroll_tests.exe` trong PowerShell.
Nếu terminal hiển thị sai tiếng Việt, chuyển sang UTF-8 với `chcp 65001`.
Không biên dịch `main.cpp` và `tests.cpp` vào cùng một chương trình: mỗi tệp có một `main()`.

Với MSVC Developer Command Prompt:

```bat
cl /std:c++17 /EHsc /W4 /utf-8 payroll.cpp main.cpp /Fe:payroll_demo.exe
cl /std:c++17 /EHsc /W4 /utf-8 payroll.cpp tests.cpp /Fe:payroll_tests.exe
```

## CMake tùy chọn

```sh
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

Đã xác minh bằng g++ trực tiếp; chưa chạy CMake hoặc MSVC trong môi trường kiểm thử này.

## Kết quả mong đợi

E001: 18.000.000; E002: 15.500.000; E003: 17.500.000; E004: 19.000.000 VND.
Tổng: 70.000.000 VND. Phòng Hỗ trợ: 33.000.000 VND. Cao nhất: E004.
Kiểm thử: `RESULT: 40/40 passed`.

## Quy ước thiết kế

- `Employee` trừu tượng; ba lớp cụ thể ghi đè ba hành vi chung.
- `Payroll` sở hữu `vector<unique_ptr<Employee>>`; không dùng phân nhánh loại để tính lương.
- Chỉ lưu tổng thưởng; lý do được kiểm tra nhưng không lưu. Không có nhật ký thưởng.
- Constructor đầy đủ nhận dữ liệu định danh và lương; thưởng ban đầu luôn bằng 0, thêm qua `addBonus`.
- Đơn giá, lương, phụ cấp, doanh số không âm và hữu hạn; số giờ trong [0, 250].
- Mã và phòng ban so sánh chính xác, phân biệt chữ hoa/thường. Không chuẩn hóa Unicode hoặc tự cắt khoảng trắng.
- Chuỗi trống hoặc chỉ chứa khoảng trắng ASCII bị từ chối.
- Bảng lương rỗng: tổng 0, người cao nhất nullptr. Đồng hạng chọn người thêm trước.
- `findEmployee` trả con trỏ mượn: không `delete`, không dùng sau khi Payroll bị hủy.
- `addEmployee` nhận unique_ptr theo giá trị: chuyển quyền sở hữu ở lời gọi; nếu bị từ chối, đối tượng truyền vào bị hủy, bảng lương giữ nguyên.
- `double` phục vụ bài tập; hiển thị 2 chữ số thập phân, không làm tròn từng thành phần trước khi cộng. Hệ thống tiền tệ thực tế cần quy tắc làm tròn và kiểu số phù hợp được thống nhất riêng.
- Các phép tính kiểm tra tràn số, ném `overflow_error`; lỗi đầu vào ném `invalid_argument`.
- Mỗi Payroll mô tả một kỳ. Không có cơ sở dữ liệu, thuế hay bảo hiểm; không thay đổi tháng của bảng lương đã lập.


