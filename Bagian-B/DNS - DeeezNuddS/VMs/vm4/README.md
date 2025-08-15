# VM 4
> Reverse Proxy (192.168.1.40)

---
### Requirements
- Linux distro (Recommended: Debian 13.0.0)
- Python 3
- Nginx
- ufw

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

    # Edit the enp0s3:
   
    auto enp0s3
   ```
   
3. Restart networking
   ```
   systemctl restart networking
   ```
   
4. Install Dependencies
   ```bash
    apt update
    apt install -y nginx python3 python3-pip python3-requests curl wget net-tools ufw
   ```
   
5. Configure Nginx as Reverse Proxy
   ```bash
    nano /etc/nginx/sites-available/default

   # Then, edit:
    server {
        listen 8080 default_server;
        listen [::]:8080 default_server;
    
        server_name vm4.example.local;
        
        location / {
            proxy_pass http://192.168.1.20:8080;
            proxy_set_header Host $host;
            proxy_set_header X-Real-IP $remote_addr;
            proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        }
    
        location /proxy-status {
            return 200 "VM4 Reverse Proxy - Active\n";
            add_header Content-Type text/plain;
        }
    }

   
   
   ```
   
6. Create the program
      ```bash
      mkdir -p ~/reverse-proxy
      cd ~/reverse-proxy
      nano ~/reverse-proxy/proxy_manager.py (you can refer it to the proxy_manager.py)
      chmod +x ~/reverse-proxy/proxy_manager.py

      # dont forget to run proxy_manager.py and call setup_firewall()
      ```
      
7. Change the network interfaces again
   ```bash
   nano /etc/network/interfaces

    # Edit the enp0s3:
   
    auto enp0s3
    iface enp0s3 inet static
       address 192.168.1.40
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
