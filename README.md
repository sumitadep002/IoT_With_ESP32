# IoT With ESP32

This project is dedicated to **exploring ESP32 capabilities in the IoT (Internet of Things) ecosystem**. It serves as a comprehensive playground for implementing networking protocols, peripheral control, and web-based management using the **ESP-IDF** framework.

---

## Project Overview

The core objective is to push the boundaries of the ESP32-S3 and other ESP variants by building a modular, service-oriented firmware. This includes:
- **Dual-mode Connectivity**: Simultaneous Access Point (AP) and Station (STA) modes.
- **Embedded Web Server**: A rich local interface for device configuration and monitoring.
- **Real-time Feedback**: Logic-driven NeoPixel indications for system events.
- **Robust Networking**: HTTP client services for cloud/internet connectivity checks.

---

## Feature Roadmap and Tracking

### Completed Milestones
The following features and bug fixes have been successfully implemented (tracked via GitHub issues):

| Issue | Description | Category |
| :--- | :--- | :--- |
| **#1** | Setup Development Environment for ESP-WROOM-32 | DevOps |
| **#4** | Bring up serial communication | Core |
| **#6** | WiFi: Passive scanning | Networking |
| **#8** | WiFi: Connection establishment | Networking |
| **#11** | WiFi: Setup AP mode | Networking |
| **#13** | WiFi: Setup AP+STA mode | Networking |
| **#2** | Basic HTTP and HTTPS functionalities | Networking |
| **#16** | Project Refactorization - Phase 1 | Architecture |
| **#18** | Web-Server: Implementation of Login Page | Web-UI |
| **#20** | Web-Server: Authentication implementation | Security |
| **#22** | Web-Server: Live Clock | Web-UI |
| **#24** | Web-Server: Internet Status Monitoring | Networking |
| **#25** | **Bug Fix**: Stack overflow in `http_client_ping()` | Stability |
| **#26** | NeoPixel: LED indications for system events | Feedback |
| **#28** | Device status through LED | Feedback |
| **#30** | **Bug Fix**: Stack overflow in `login_get_handler()` | Stability |
| **#31** | Web-Server: Colorboard (NeoPixel Control) | Web-UI |
| **#33** | Web-Server: WiFi Status Dashboard | Web-UI |

### In Progress / Planned
Active development is focused on the following areas:

| Issue | Task | Priority |
| :--- | :--- | :--- |
| **#15** | NVS: Permanent storage for WiFi credentials | High |
| **#29** | Web-Server: Over-the-Air (OTA) firmware updates | Medium |
| **#35** | Web-Server: Dynamic WiFi configuration updates | High |

---

## SDK, Toolchain, and Environment Setup

### 1. Install Prerequisites
```bash
sudo apt-get install git wget flex bison gperf python3 python3-pip python3-venv \
cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
```

### 2. Download ESP-IDF SDK
This project is verified with **ESP-IDF v5.5.1**.
```bash
mkdir -p ~/esp
cd ~/esp
git clone -b v5.5.1 --recursive https://github.com/espressif/esp-idf.git
```

### 3. Setup Toolchain (Workspace-Specific)
```bash
cd ~/esp/esp-idf/
export IDF_TOOLS_PATH="$HOME/.espressif_custom" # Recommended to keep workspace clean
./install.sh
```

### 4. Automate Toolchain Accessibility with `direnv`
1. **Hook into Shell**: Add `eval "$(direnv hook bash)"` to your `~/.bashrc`.
2. **Install**: `sudo apt install direnv`.
3. **Configure**: Inside the workspace root, run:
   ```bash
   echo 'source $HOME/esp/esp-idf/export.sh' > .envrc
   direnv allow
   ```

---

## Notes
- The SDK and toolchain are isolated to avoid global environment pollution.
- For deep dives into the implementation, refer to the `app/services` directory.