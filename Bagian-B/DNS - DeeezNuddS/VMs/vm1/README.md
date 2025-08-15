# VM 1
> DNS Server (192.168.1.10)

---
### Requirements
- Linux distro (Recommended: Debian 13.0.0)
- Bind9

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
   
4. Install Bind9
   ```bash
    apt update
    apt install bind9 bind9-utils bind9-doc dnsutils -y
   ```
   
5. Configure the Bind9:
   ```bash
   nano /etc/bind/named.conf.local

   zone "example.local" {
    type master;
    file "/etc/bind/db.example.local";
   };

   # then

   nano /etc/bind/db.example.local

   ;
   ; BIND data file for example.local
   ;
   $TTL    604800
   @       IN      SOA     ns1.example.local. admin.example.local. (
                        2024081201         ; Serial
                            604800         ; Refresh
                             86400         ; Retry
                           2419200         ; Expire
                            604800 )       ; Negative Cache TTL
   ;
   @       IN      NS      ns1.example.local.
   ns1     IN      A       192.168.1.10
   www     IN      A       192.168.1.40    ; Pointing to VM4 (Reverse Proxy)
   web     IN      A       192.168.1.40    ; Alternative name
   app     IN      A       192.168.1.40    ; Alternative name
   
   zone "1.168.192.in-addr.arpa" {
       type master;
       file "/etc/bind/db.192.168.1";
   };

   # then
   nano /etc/bind/db.192.168.1

   ;
   ; BIND reverse data file for 192.168.1.x
   ;
   $TTL    604800
   @       IN      SOA     ns1.example.local. admin.example.local. (
                        2024081201         ; Serial
                            604800         ; Refresh
                             86400         ; Retry
                           2419200         ; Expire
                            604800 )       ; Negative Cache TTL
   ;
   @       IN      NS      ns1.example.local.
   10      IN      PTR     ns1.example.local.
   20      IN      PTR     server.example.local.
   30      IN      PTR     client.example.local.
   40      IN      PTR     proxy.example.local.

   # then
   nano /etc/bind/named.conf.options

   options {
    directory "/var/cache/bind";
    
    listen-on { 192.168.1.10; 127.0.0.1; };
    allow-query { 192.168.1.0/24; 127.0.0.1; };
    
    forwarders {
        8.8.8.8;
        8.8.4.4;
    };
    
    dnssec-validation auto;
    
    recursion yes;
    allow-recursion { 192.168.1.0/24; 127.0.0.1; };
    
    auth-nxdomain no;
   };
   
   
   ```
6. Validate the config
      ```bash
      named-checkconf
      named-checkzone example.local /etc/bind/db.example.local
      named-checkzone 1.168.192.in-addr.arpa /etc/bind/db.192.168.1
      ```
      
7. Change the network interfaces again and restart the network
   ```bash
   nano /etc/network/interfaces

    # Edit the enp0s3:
   
    auto enp0s3
    iface enp0s3 inet static
       address 192.168.1.10
       netmask 255.255.255.0
       network 192.168.1.0
       broadcast 192.168.1.255

   # Then, restart the network:
   systemctl restart networking
   ```
   
8. Restart and enable the services
      ```bash
      systemctl restart bind9
      systemctl enable bind9
      systemctl status bind9
      ```

> [!NOTE]
> Don't forget to set the VirtualBox network configuration to internal network
