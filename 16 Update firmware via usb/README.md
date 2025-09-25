# Firmware Update via USB – Learning & Development Roadmap (2 Months)

Dự án: Xây dựng hệ thống cập nhật firmware tự động qua USB bằng Raspberry Pi.  
Thời gian: **8 tuần (2 tháng)**.  
Phương pháp: Chia theo sprint, vừa học vừa làm.  

---

## Sprint Plan

| Giai đoạn                 | Tuần  | Nhiệm vụ chính                                                                                          | Cột mốc đạt được                                                                 |
|----------------------------|-------|--------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------|
| **Sprint 1: Nền tảng Python & Raspberry Pi** | Tuần 1 | - Ôn Python cơ bản (function, class, exception, import)<br>- Học pip, venv<br>- Cài Raspberry Pi OS Lite headless (SSH, WiFi)<br>- Viết script Python chạy lệnh Linux | Raspberry Pi hoạt động headless, Python sẵn sàng sử dụng. |
|                            | Tuần 2 | - Học GPIO (gpiozero)<br>- Lắp mạch 3 LED + điện trở<br>- Viết script bật/tắt/nhấp nháy LED<br>- Hiểu state machine cơ bản | Có thể điều khiển LED bằng Python, state machine đơn giản chạy được. |
| **Sprint 2: USB & udev**   | Tuần 3 | - Tìm hiểu udev trong Linux<br>- Học công cụ lsusb, udevadm<br>- Viết script pyudev log sự kiện USB<br>- In VID/PID khi cắm USB | Script Python phát hiện USB, lấy được VID/PID thiết bị. |
|                            | Tuần 4 | - Viết Device Validator so sánh VID/PID<br>- Cài công cụ flash (esptool.py / dfu-util / avrdude)<br>- Thử nạp firmware từ command line<br>- Viết module Firmware Flasher (Python + subprocess) | Có thể nạp firmware thủ công qua Python subprocess. |
| **Sprint 3: Tích hợp & Logic** | Tuần 5 | - Tích hợp Device Validator + Flasher + LED<br>- Xây dựng state machine (IDLE, UPDATING, SUCCESS, FAILURE)<br>- Xử lý exception, timeout | Hệ thống hoạt động cơ bản: phát hiện đúng thiết bị và nạp firmware + LED báo. |
|                            | Tuần 6 | - Tìm hiểu systemd (unit file, service)<br>- Viết firmware-updater.service<br>- Cấu hình auto start khi boot<br>- Test restart khi script crash | Service systemd chạy ổn định, script tự động khởi chạy khi boot. |
| **Sprint 4: Testing & Hoàn thiện** | Tuần 7 | - Viết test plan<br>- Test case: success, failure, sai VID/PID<br>- Kiểm tra thời gian phản hồi <3s<br>- Debug qua journalctl | Hệ thống được kiểm thử đầy đủ, hoạt động ổn định. |
|                            | Tuần 8 | - Gom code thành repo GitHub<br>- Viết tài liệu (setup, run, troubleshooting)<br>- Viết báo cáo & slide<br>- Demo end-to-end | Dự án hoàn thiện, có repo + tài liệu + báo cáo. |

---

## Nguồn học tập & Tài liệu tham khảo

- **Python:** [Python Official Tutorial](https://docs.python.org/3/tutorial/), [Python for Beginners – freeCodeCamp](https://www.youtube.com/watch?v=rfscVS0vtbw)  
- **Raspberry Pi:** [Headless setup](https://www.raspberrypi.com/documentation/computers/getting-started.html)  
- **GPIO:** [GPIO Zero Docs](https://gpiozero.readthedocs.io/en/stable/), [Core Electronics GPIO Tutorial](https://www.youtube.com/watch?v=5X1y6IIk0n0)  
- **udev:** [pyudev Docs](https://pyudev.readthedocs.io/en/latest/), [Linux udev tutorial](https://www.youtube.com/watch?v=uuGdAq3WQ8I)  
- **Firmware tools:** [esptool.py](https://github.com/espressif/esptool), [dfu-util](http://dfu-util.sourceforge.net/), [avrdude](https://www.nongnu.org/avrdude/)  
- **systemd:** [systemd Documentation](https://www.freedesktop.org/wiki/Software/systemd/), [Techworld with Nana - systemd](https://www.youtube.com/watch?v=F2K1yLZ8XQw)  
- **Testing:** [STLC Overview](https://www.geeksforgeeks.org/software-testing-life-cycle-stlc/), [Debugging Python](https://www.youtube.com/watch?v=bp2wfX6XEOI)  
- **Documentation:** [Make a README](https://www.makeareadme.com/), [Technical Documentation Tips](https://www.youtube.com/watch?v=3b8vP4CuhHc)  

---
