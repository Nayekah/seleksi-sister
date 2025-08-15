# VM 3
> Client (Default 192.168.1.30, tentatives)

---
### Requirements
- Linux distro (Recommended: Debian 13.0.0)
- Python 3

</br>

> [!NOTE]
> Don't forget to setup your linux distro environment first, Debian is recommended
---

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
   
4. Install Python (and some dependencies)
   ```bash
    apt update
    apt install -y python3 python3-pip python3-requests curl wget dnsutils net-tools iputils-ping gcc make nano
   ```
   
5. Create the client program:
   ```bash
    mkdir -p ~/client-app
    cd ~/client-app

   # Implement the code
   nano ~/client-app/client.py (you can refer it to client.py too)
   chmod +x ~/client-app/client.py
   ```
   
6. Validate the config
      ```bash
      named-checkconf
      named-checkzone example.local /etc/bind/db.example.local
      named-checkzone 1.168.192.in-addr.arpa /etc/bind/db.192.168.1
      ```
7. Change the network interfaces again
   ```bash
   nano /etc/network/interfaces

    edit the enp0s3:
   
    auto enp0s3
    iface enp0s3 inet static
       address 192.168.1.30
       netmask 255.255.255.0
       network 192.168.1.0
       broadcast 192.168.1.255
       dns-nameservers 192.168.1.10
   ```

8. Configure the DNS
      ```bash
      nano /etc/resolv.conf

      # Then, edit the contents:
      nameserver 192.168.1.10
      search example.local
      ```

9. Restart the network
      ```bash
      systemctl restart networking
      ```

> [!NOTE]
> Don't forget to set the VirtualBox network configuration to internal network
