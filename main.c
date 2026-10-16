#include <stdio.h>
#include <string.h>

#define MAX_VE 50

// Cấu trúc dữ liệu vé sự kiện
struct VeSuKien {
    int maVe;
    char tenSuKien[50];
    char loaiVe[20];
    float giaGoc;
    int isEarlyBird; // 1: Giảm 15%, 0: Giá niêm yết
    int daCheckIn;   // 0: Chưa check-in, 1: Đã qua cổng
};

// Function Prototypes
float tinhGiaThucTe(const struct VeSuKien *ve);
int timVeTheoMa(const struct VeSuKien dsVe[], int soLuong, int maVeCanTim);
int themVe(struct VeSuKien dsVe[], int soLuong, struct VeSuKien veMoi);
int checkInVe(struct VeSuKien dsVe[], int soLuong, int maVe);
int huyVe(struct VeSuKien dsVe[], int soLuong, int maVe);
void hienThiDanhSachVe(const struct VeSuKien dsVe[], int soLuong);

int main(void) {
    struct VeSuKien dsVe[MAX_VE];
    int soLuongVe = 0;

    printf("========================================================================\n");
    printf("   HE THONG QUAN LY & SOAT VE CONG TU DONG - LIVE CONCERT VIBE 2025    \n");
    printf("========================================================================\n\n");

    // 1. Khoi tao danh sach gom 3 ve ban dau
    struct VeSuKien ve1 = {1001, "Live Concert VIBE 2025", "VIP", 2500000.0f, 1, 0};
    struct VeSuKien ve2 = {1002, "Live Concert VIBE 2025", "Zone A", 1500000.0f, 0, 0};
    struct VeSuKien ve3 = {1003, "Live Concert VIBE 2025", "GA", 800000.0f, 1, 0};

    soLuongVe = themVe(dsVe, soLuongVe, ve1);
    soLuongVe = themVe(dsVe, soLuongVe, ve2);
    soLuongVe = themVe(dsVe, soLuongVe, ve3);

    // 2. In danh sach ve ban dau
    printf("\n--- DANH SACH VE HIEN CO TRONG HE THONG ---\n");
    hienThiDanhSachVe(dsVe, soLuongVe);

    // 3. Phat hanh them 1 ve moi (ve4)
    printf("\n--- THAO TAC 1: PHAT HANH VE MOI ---\n");
    struct VeSuKien ve4 = {1004, "Live Concert VIBE 2025", "VIP", 2500000.0f, 0, 0};
    soLuongVe = themVe(dsVe, soLuongVe, ve4);

    // 4. Quet ma QR check-in thanh cong 1 ve (ve 1001)
    printf("\n--- THAO TAC 2: CHECK-IN VE 1001 LAN DAU ---\n");
    checkInVe(dsVe, soLuongVe, 1001);

    // 5. Quet lai chinh ma ve 1001 de kich hoat Edge Case Duplicate QR
    printf("\n--- THAO TAC 3: QUET LAI VE 1001 (KIEM TRA GIAN LAN) ---\n");
    checkInVe(dsVe, soLuongVe, 1001);

    // 6. Thuc hien huy 1 ve theo ma (huy ve 1002)
    printf("\n--- THAO TAC 4: HUY VE 1002 KHOI HE THONG ---\n");
    soLuongVe = huyVe(dsVe, soLuongVe, 1002);

    // 7. Quet ma ve khong ton tai (ve 9999)
    printf("\n--- THAO TAC 5: CHECK-IN MA VE KHONG TON TAI (9999) ---\n");
    checkInVe(dsVe, soLuongVe, 9999);

    // 8. Hien thi lai toan bo danh sach ve sau tat ca cac thao tac
    printf("\n--- DANH SACH TONG KET SAU CAC GIAO DICH ---\n");
    hienThiDanhSachVe(dsVe, soLuongVe);

    return 0;
}

// Ham tinh gia thuc te dua theo chinh sach Early Bird
float tinhGiaThucTe(const struct VeSuKien *ve) {
    if (ve == NULL) {
        return 0.0f;
    }
    if (ve->isEarlyBird == 1) {
        return ve->giaGoc * 0.85f; // Chiet khau 15%
    }
    return ve->giaGoc;
}

// Ham tim kiem hat nhan theo ma ve
int timVeTheoMa(const struct VeSuKien dsVe[], int soLuong, int maVeCanTim) {
    if (soLuong <= 0) {
        return -1;
    }
    for (int i = 0; i < soLuong; i++) {
        if (dsVe[i].maVe == maVeCanTim) {
            return i; // Tra ve chi so index tim thay
        }
    }
    return -1; // Khong ton tai
}

// Ham phat hanh (them) ve moi vao danh sach
int themVe(struct VeSuKien dsVe[], int soLuong, struct VeSuKien veMoi) {
    // Kiem tra loi tran bo nho mang
    if (soLuong >= MAX_VE) {
        printf("[LOI PHAT HANH]: He thong da dat gioi han toi da %d ve! Khong the them ma ve %d.\n", 
               MAX_VE, veMoi.maVe);
        return soLuong;
    }

    // Kiem tra trung ma ve
    if (timVeTheoMa(dsVe, soLuong, veMoi.maVe) != -1) {
        printf("[LOI PHAT HANH]: Ma ve %d da ton tai trong he thong!\n", veMoi.maVe);
        return soLuong;
    }

    dsVe[soLuong] = veMoi;
    soLuong++;
    printf("[THANH CONG]: Da phat hanh ve %d | Khu vuc: %s | Su kien: %s.\n", 
           veMoi.maVe, veMoi.loaiVe, veMoi.tenSuKien);
    return soLuong;
}

// Ham check-in ma QR tai cong
int checkInVe(struct VeSuKien dsVe[], int soLuong, int maVe) {
    if (soLuong <= 0) {
        printf("[LOI CHECK-IN]: Danh sach he thong rong!\n");
        return -1;
    }

    int viTri = timVeTheoMa(dsVe, soLuong, maVe);

    // Kiem tra ma ve co hop le tren he thong khong
    if (viTri == -1) {
        printf("[CANH BAO SOAT VE]: Ma ve %d KHONG TON TAI tren he thong!\n", maVe);
        return -1;
    }

    // Kiem tra sai sot Duplicate QR scan
    if (dsVe[viTri].daCheckIn == 1) {
        printf("[CANH BAO GIAN LAN]: Ma ve %d DA CHECK-IN truoc do! Tu choi mo cong.\n", maVe);
        return 0;
    }

    // Xac nhan hop le va cap nhat trang thai
    dsVe[viTri].daCheckIn = 1;
    printf("[CHECK-IN HOP LE]: Xin moi qua cong! [Ma: %d | Khu vuc: %s | Gia TT: %.0f VND]\n",
           dsVe[viTri].maVe, dsVe[viTri].loaiVe, tinhGiaThucTe(&dsVe[viTri]));
    return 1;
}

// Ham huy ve (don mang sang trai)
int huyVe(struct VeSuKien dsVe[], int soLuong, int maVe) {
    if (soLuong <= 0) {
        printf("[LOI HUY VE]: He thong khong co ve nao de huy (Underflow)!\n");
        return soLuong;
    }

    int viTri = timVeTheoMa(dsVe, soLuong, maVe);

    if (viTri == -1) {
        printf("[LOI HUY VE]: Khong tim thay ma ve %d can huy!\n", maVe);
        return soLuong;
    }

    // Don mang de loai bo phan tu tai index viTri
    for (int i = viTri; i < soLuong - 1; i++) {
        dsVe[i] = dsVe[i + 1];
    }

    soLuong--;
    printf("[THANH CONG]: Da huy ma ve %d khoi he thong. So luong ve con lai: %d.\n", maVe, soLuong);
    return soLuong;
}

// Ham xuat toan bo danh sach ve
void hienThiDanhSachVe(const struct VeSuKien dsVe[], int soLuong) {
    if (soLuong == 0) {
        printf("[THONG BAO]: Hien tai he thong khong co ve nao.\n");
        return;
    }

    printf("+------+---------+------------------------+---------+---------------+---------------+-------------------+\n");
    printf("| STT  | Ma Ve   | Ten Su Kien            | Loai Ve | Gia Goc (VND) | Gia TT (VND)  | Trang Thai Cong   |\n");
    printf("+------+---------+------------------------+---------+---------------+---------------+-------------------+\n");

    for (int i = 0; i < soLuong; i++) {
        float giaThucTe = tinhGiaThucTe(&dsVe[i]);
        const char *trangThai = (dsVe[i].daCheckIn == 1) ? "DA CHECK-IN" : "Chua check-in";

        printf("| %-4d | %-7d | %-22.22s | %-7s | %-13.0f | %-13.0f | %-17s |\n",
               i + 1,
               dsVe[i].maVe,
               dsVe[i].tenSuKien,
               dsVe[i].loaiVe,
               dsVe[i].giaGoc,
               giaThucTe,
               trangThai);
    }
    printf("+------+---------+------------------------+---------+---------------+---------------+-------------------+\n");
}