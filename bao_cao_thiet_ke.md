# BÁO CÁO PHÂN TÍCH VÀ THIẾT KẾ KIẾN TRÚC MODULE CRUD - TICKETBOX

## 1. Phân tích I/O (Input/Output) của các hàm

| Tên hàm | Input (Tham số đầu vào) | Output (Giá trị trả về) | Mục đích / Trách nhiệm (SRP) |
|---|---|---|---|
| `tinhGiaThucTe` | `const struct VeSuKien *ve`: Con trỏ đọc thông tin vé | `float`: Giá sau khi chiết khấu | Tính giá thực tế của vé dựa vào cờ `isEarlyBird` (giảm 15% nếu là 1). |
| `timVeTheoMa` | - `const struct VeSuKien dsVe[]`: Mảng danh sách vé<br>- `int soLuong`: Số lượng vé hiện có<br>- `int maVeCanTim`: Mã vé cần tra cứu | `int`: Chỉ số `index` (0 đến `soLuong - 1`) nếu tìm thấy; `-1` nếu không tồn tại hoặc mảng rỗng | Hàm lõi tìm kiếm vé theo mã định danh duy nhất. |
| `themVe` | - `struct VeSuKien dsVe[]`: Mảng danh sách vé<br>- `int soLuong`: Số lượng vé hiện tại<br>- `struct VeSuKien veMoi`: Vé cần thêm vào | `int`: Số lượng vé mới sau khi thêm (`soLuong + 1` nếu thành công, giữ nguyên `soLuong` nếu thất bại) | Kiểm tra giới hạn `MAX_VE`, kiểm tra trùng mã vé, phát hành vé mới vào mảng. |
| `checkInVe` | - `struct VeSuKien dsVe[]`: Mảng danh sách vé<br>- `int soLuong`: Số lượng vé hiện có<br>- `int maVe`: Mã vé quét tại cổng | `int`: `1` nếu check-in thành công; `0` nếu thất bại (trùng lặp check-in); `-1` nếu không tìm thấy vé | Xử lý kiểm soát vào cổng, chống gian lận quét QR lặp lại. |
| `huyVe` | - `struct VeSuKien dsVe[]`: Mảng danh sách vé<br>- `int soLuong`: Số lượng vé hiện có<br>- `int maVe`: Mã vé cần hủy | `int`: Số lượng vé sau khi hủy (`soLuong - 1` nếu thành công, giữ nguyên `soLuong` nếu thất bại) | Xóa vé khỏi hệ thống bằng giải thuật dồn mảng sang trái và cập nhật quy mô. |
| `hienThiDanhSachVe` | - `const struct VeSuKien dsVe[]`: Mảng danh sách vé<br>- `int soLuong`: Số lượng vé hiện hành | `void` | Xuất toàn bộ danh sách vé dạng bảng trực quan kèm giá thực tế và trạng thái soát vé. |

---

## 2. Thiết kế Kiến trúc Hàm & Vai trò của Hàm Hạt nhân

### 2.1. Danh sách Prototype
```c
float tinhGiaThucTe(const struct VeSuKien *ve);
int timVeTheoMa(const struct VeSuKien dsVe[], int soLuong, int maVeCanTim);
int themVe(struct VeSuKien dsVe[], int soLuong, struct VeSuKien veMoi);
int checkInVe(struct VeSuKien dsVe[], int soLuong, int maVe);
int huyVe(struct VeSuKien dsVe[], int soLuong, int maVe);
void hienThiDanhSachVe(const struct VeSuKien dsVe[], int soLuong);
2.2. Vai trò của hàm tìm kiếm hạt nhân (timVeTheoMa)Tái sử dụng logic (DRY - Don't Repeat Yourself): Cả luồng "Check-in QR", "Hủy vé" và "Thêm vé" đều cần xác định xem một mã vé đã tồn tại trong mảng hay chưa và nằm ở vị trí nào.Tách biệt mối quan tâm (Decoupling): Các hàm nghiệp vụ cấp cao (checkInVe, huyVe) không cần tự triển khai vòng lặp duyệt mảng. Chúng chỉ cần gọi timVeTheoMa(), nhận về chỉ số index (hoặc -1) rồi tập trung xử lý đúng logic nghiệp vụ của riêng mình.3. Các bước xử lý Logic & Luồng đồng bộ trạng thái3.1. Luồng Check-in QR Cổng tự độngKiểm tra mảng rỗng: Nếu soLuong <= 0 $\rightarrow$ Thông báo hệ thống chưa có vé, trả về -1.Tìm vị trí vé: Gọi index = timVeTheoMa(dsVe, soLuong, maVe).Kiểm tra tồn tại: Nếu index == -1 $\rightarrow$ Thông báo mã vé không tồn tại trên hệ thống, trả về -1.Kiểm tra trạng thái quét:Nếu dsVe[index].daCheckIn == 1: Cảnh báo gian lận (vé đã được check-in trước đó), từ chối mở cổng, trả về 0.Nếu dsVe[index].daCheckIn == 0: Cập nhật dsVe[index].daCheckIn = 1, thông báo hợp lệ, cho phép qua cổng, trả về 1.3.2. Luồng Hủy vé (Dồn mảng) & Đồng bộ số lượng véKiểm tra mảng rỗng: Nếu soLuong <= 0 $\rightarrow$ Thông báo lỗi Underflow, trả về soLuong.Tìm vị trí vé cần hủy: Gọi index = timVeTheoMa(dsVe, soLuong, maVe).Kiểm tra tồn tại: Nếu index == -1 $\rightarrow$ Báo lỗi vé không tồn tại, trả về soLuong ban đầu.Thực hiện dồn mảng:Chạy vòng lặp từ i = index đến soLuong - 2:dsVe[i] = dsVe[i + 1];Đồng bộ về main(): Hàm trả về soLuong - 1. Trong main(), biến quản lý số lượng được đồng bộ qua phép gán:soLuongVe = huyVe(dsVe, soLuongVe, maVeCanHuy);