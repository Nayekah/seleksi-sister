# DeezNudds

> Seleksi Asisten Laboratorium Sistem Paralel dan Terdistribusi 2025
<p align="center">
    <img src="./etc/newjeans-eta.gif">
</p>
    <h3 align="center">What's your DNS?</h3>

---

## About <a name="about"></a>

<p align="justify">This project implements a simple DNS system running locally in linux using Debian, Bind9, Apache Server, and NGINX </p>

---

## Implementations <a name="algorithms"></a>

There are 4 Virtual Machines that implemented in Debian 13.0.0 (Trixie), you can refer it to https://www.debian.org/download for the .iso installations

### Virtual Machines (VMs)  
supported four Virtual Machines:

- **VM 1**  
  VM1 will act as the DNS server, with the static IP Address of 192.168.1.10

- **VM 2**  
  VM2 act as web server, with the static IP Address of 192.168.1.20

- **VM 3**  
  VM3 acts as client, the IP implemenetations depends on the user (can be manually defined or using DHCP)

- **VM 4**  
  VM4 acts as proxy, with the static IP Address of 192.168.1.40 (and acts as a firewall too)
--- 

## How to Run <a name="how-to-run"></a>

### Requirements
- Debian 13.0.0 (or any distro you like, just improvise)
- Python3
- Bind9
- Apache Server
- Nginx
- ufw

> [!IMPORTANT]
> This project is managed VirtualBox, so configure the network configurations based on the app you used

Next, please refer to the each VM's implementations in the VMs folder

Demo of the program: 

