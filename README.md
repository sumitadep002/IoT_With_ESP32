## SDK, Toolchain, and Environment Setup

### 1. Install prerequisites
```bash
sudo apt-get install git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
````

### 2. Download ESP-IDF SDK

```bash
mkdir -p ~/esp
cd ~/esp
git clone -b v5.5.1 --recursive https://github.com/espressif/esp-idf.git
```

### 3. Set up the toolchain (workspace-specific)

```bash
cd ~/esp/esp-idf/
export IDF_TOOLS_PATH="$HOME/your_path"
./install.sh
```

### 4. Automate toolchain accessibility with direnv

1. Add the direnv hook to bash:

   ```bash
   echo 'eval "$(direnv hook bash)"' >> ~/.bashrc
   source ~/.bashrc
   ```

2. Install direnv:

   ```bash
   sudo apt install direnv
   ```

3. Inside your workspace, configure `.envrc`:

   ```bash
   cd $HOME/your_path
   echo 'source $HOME/esp/esp-idf/export.sh' > .envrc
   direnv allow
   ```

---

## Notes

* The SDK and toolchain are set up in a **workspace-specific manner** to avoid modifying your global environment.
* These setup steps are also documented and tracked in **[Issue #1](../../issues/1)**.