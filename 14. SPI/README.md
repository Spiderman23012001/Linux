# ST7735 
## 1. Tổng quát
- 396 đường điều khiển cột (source lines)
- 162 đường điều khiển dòng (gate lines)
- => Điều này giúp điều khiển ma trận điểm ảnh trên màn hình.

Chip có thể đọc/ghi dữ liệu màn hình mà `không cần xung clock bên ngoài`, giúp tiết kiệm điện năng – rất hữu ích cho thiết bị di động, tiết kiệm pin.
Kích thước:
- Chiều ngang: 132 pixel
- Chiều dọc: 162 pixel
- **Chiều sâu: 262,144 (màu, 2^18 R-G-B 6-6-6)**

## 2. Features
### 2.1 Single chip TFT-LCD Controller/Driver with RAM
- Đây là **chip đơn** tích hợp cả:
    - Mạch **điều khiển (controller)** -> Xử lý lệnh từ vi điều khiển
    - Mạch **driver** -> Điều khiển điện áp đến các pixel LCD
    - **Bộ nhớ RAM tích hợp** dùng làm bộ đếm hiển thị (frame buffer)
## 2. Display Resolution 
- 132 (RGB) x 162 (GM=000)
- 128 (RGB) x 160 (GM=011)
- Tùy cấu hình GM[2:0], độ phân giải hiển thị có thể là:
    - 132 x 162 pixel (full)
    - 128 x 160 pixel (thường dùng) – để phù hợp khung hình
## 3. Color Mode
- Full color: 262K (RGB=666)
- 8-color: RGB=111 (Idle Mode ON) (chỉ hiển thị 8 màu cơ bản - **tiết kiệm pin**)
## 4. Pixel format
- 12-bit RGB: 4-4-4
- 16-bit RGB: 5-6-5
- 18-bit RGB: 6-6-6
## 5. Giao tiếp hỗ trợ
- Parallel 8080 (8,9,16,18 bit)
- SPI 3 wire,4 wire
## 6. Mạch tích hợp sẵn
- Mach tạo nguồn để chạy LCD 
- Mạch tạo điện áp VCOM
- Bộ nhớ không mất NV chứa cấu hình ban đầu
- Dao động nội để tạo xung clock cho LCD
- Bộ điều khiển thời gian (timing controller)