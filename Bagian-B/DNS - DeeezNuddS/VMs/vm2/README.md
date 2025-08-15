# VM 2
> Web Server (192.168.1.20)

---
### Requirements
- Linux distro (Recommended: Debian 13.0.0)
- Apache2

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
4. Install Apache2
   ```bash
    apt update
    apt install apache2 -y
   ```
5. Configure Apache2 to port 8080:
   ```bash
   nano /etc/apache2/ports.conf

   change the "Listen" to:
   Listen 8080

   # then, edit default virtual hosts, this is the simplified ver:
    <VirtualHost *:8080>
      ServerAdmin webmaster@localhost
      DocumentRoot /var/www/html
      ServerName vm2.example.local
      
      ErrorLog ${APACHE_LOG_DIR}/error.log
      CustomLog ${APACHE_LOG_DIR}/access.log combined
    </VirtualHost>
   
   
   
   ```
6. Make the HTTP content (implemented in the repository, you can refer it to index.html, test.html, info.html, and status.json)

7. Set the html and json file permissions
   ```bash
    chown -R www-data:www-data /var/www/html/
    chmod -R 755 /var/www/html/
    chmod 644 /var/www/html/*.html
    chmod 644 /var/www/html/*.json
   ```
8. Change the network interfaces again
   ```bash
   nano /etc/network/interfaces

    edit the enp0s3:
   
    auto enp0s3
    iface enp0s3 inet static
       address 192.168.1.20
       netmask 255.255.255.0
       network 192.168.1.0
       broadcast 192.168.1.255
   ```
   
9. Restart and enable the services
      ```bash
      systemctl restart apache2
      systemctl start apache2
      systemctl enable apache2
      systemctl status apache2
      ```

> [!NOTE]
> Don't forget to set the VirtualBox network configuration to internal network
