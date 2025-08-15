# VM 1
> DNS Server

---
### Requirements
- Linux distro (Recommended: Debian 13.0.0)
- Python 3.8
- Bind9

</br>

> [!NOTE]
> Don't forget to setup your linux distro environment first, Debian is recommended

### Setting up the VMs
1. Access the root privilege with this commands
   ```bash
   su
   ```

2. Edit file network interfaces
   ```bash
   nano /etc/network/interfaces

    edit the enp0s3:
   
    auto enp0s3
   ```
3. Restart networking
   ```
   systemctl restart networking
   ```
4. Install and configure BIND9
   ```bash
    apt update
    apt install bind9 bind9-utils bind9-doc dnsutils -y
   ```
5. Make sure to use python 3.8:
   ```bash
   uv python install 3.8
   uv python pin 3.8
   ```
6. Setup OS-specific library
    - **Windows**
      ```bash
      pip install PyQtWebEngine
      ```
    - **Linux**
      ```bash
      uv add PyQtWebEngine
      ```
