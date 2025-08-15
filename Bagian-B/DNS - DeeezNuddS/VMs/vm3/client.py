#!/usr/bin/env python3

import requests
import socket
import sys
import subprocess
import time

class NetworkClient:
    def __init__(self):
        self.dns_server = "192.168.1.10"
        self.target_domain = "www.example.local"
        self.target_port = 8080
        self.timeout = 10
        
    def check_dns_resolution(self):
        print(f"Testing DNS resolution for {self.target_domain}")
        try:
            result = socket.gethostbyname(self.target_domain)
            print(f"DNS Resolution: {self.target_domain} -> {result}")
            return result
        except socket.gaierror as e:
            print(f"DNS Resolution failed: {e}")
            return None
    
    def test_connectivity(self, ip_address):
        print(f"Testing connectivity to {ip_address}")
        try:
            result = subprocess.run(['ping', '-c', '3', ip_address], 
                                  capture_output=True, text=True, timeout=self.timeout)
            if result.returncode == 0:
                print(f"Ping successful to {ip_address}")
                return True
            else:
                print(f"Ping failed to {ip_address}")
                return False
        except subprocess.TimeoutExpired:
            print(f"Ping timeout to {ip_address}")
            return False
    
    def make_http_request(self, endpoint="/"):
        url = f"http://{self.target_domain}:{self.target_port}{endpoint}"
        print(f"Making HTTP request to {url}")
        
        try:
            response = requests.get(url, timeout=self.timeout)
            print(f"HTTP Request successful")
            print(f"Status Code: {response.status_code}")
            print(f"Content Length: {len(response.text)} bytes")
            
            print("Response Headers:")
            for key, value in response.headers.items():
                print(f"  {key}: {value}")
            
            return response
        except requests.exceptions.RequestException as e:
            print(f"HTTP Request failed: {e}")
            return None
    
    def show_response_content(self, response, lines=10):
        if response:
            print(f"\nResponse Content Preview (first {lines} lines):")
            print("-" * 50)
            content_lines = response.text.split('\n')
            for i, line in enumerate(content_lines[:lines]):
                print(f"{i+1:2d}: {line}")
            if len(content_lines) > lines:
                print(f"... ({len(content_lines) - lines} more lines)")
            print("-" * 50)
    
    def test_api_endpoints(self):
        endpoints = ["/", "/test.html", "/info.html", "/status.json"]
        
        print("\nTesting API Endpoints:")
        for endpoint in endpoints:
            print(f"\nTesting {endpoint}")
            response = self.make_http_request(endpoint)
            if response:
                print(f"{endpoint}: Status {response.status_code}")
            else:
                print(f"{endpoint}: Failed")
    
    def network_diagnostic(self):
        print("\nNetwork Diagnostic:")
        print("-" * 50)
        
        try:
            result = subprocess.run(['ip', 'addr', 'show'], capture_output=True, text=True)
            print("Network Interfaces:")
            for line in result.stdout.split('\n'):
                if 'inet ' in line and '127.0.0.1' not in line:
                    print(f"  {line.strip()}")
        except:
            pass
        
        try:
            with open('/etc/resolv.conf', 'r') as f:
                print("\nDNS Configuration:")
                for line in f:
                    print(f"  {line.strip()}")
        except:
            pass
    
    def run_client_test(self):
        print("VM3 Network Client Application")
        print("-" * 50)
        
        self.network_diagnostic()
        
        print("\n" + "-" * 50)
        resolved_ip = self.check_dns_resolution()
        if not resolved_ip:
            print("Cannot proceed without DNS resolution")
            return False
        
        print("\n" + "-" * 50)
        if not self.test_connectivity(resolved_ip):
            print("Cannot proceed without connectivity")
            return False
        
        print("\n" + "-" * 50)
        response = self.make_http_request("/")
        if response:
            self.show_response_content(response)
        
        self.test_api_endpoints()
        
        print("\nClient test completed")
        return True

def configure_network():
    print("\nNetwork Configuration Menu")
    print("1. Use DHCP (automatic)")
    print("2. Set Manual IP")
    print("3. Current configuration")
    print("4. Back to main menu")
    
    choice = input("\nSelect option (1-4): ").strip()
    
    if choice == "1":
        configure_dhcp()
    elif choice == "2":
        configure_manual_ip()
    elif choice == "3":
        show_current_config()
    elif choice == "4":
        return
    else:
        print("Invalid option")

def configure_dhcp():
    print("\nConfiguring DHCP...")
    dhcp_config = """auto enp0s3
iface enp0s3 inet dhcp
"""
    try:
        with open('/tmp/interfaces_dhcp', 'w') as f:
            f.write(dhcp_config)
        print("DHCP configuration created")
        print("To apply: sudo cp /tmp/interfaces_dhcp /etc/network/interfaces && sudo systemctl restart networking")
    except Exception as e:
        print(f"Error: {e}")

def configure_manual_ip():
    print("\nManual IP Configuration")
    ip = input("Enter IP address (default: 192.168.1.30): ").strip() or "192.168.1.30"
    netmask = input("Enter netmask (default: 255.255.255.0): ").strip() or "255.255.255.0"
    dns = input("Enter DNS server (default: 192.168.1.10): ").strip() or "192.168.1.10"
    
    manual_config = f"""auto enp0s3
iface enp0s3 inet static
    address {ip}
    netmask {netmask}
    network 192.168.1.0
    broadcast 192.168.1.255
    dns-nameservers {dns}
"""
    try:
        with open('/tmp/interfaces_manual', 'w') as f:
            f.write(manual_config)
        
        resolv_config = f"nameserver {dns}\nsearch example.local\n"
        with open('/tmp/resolv.conf', 'w') as f:
            f.write(resolv_config)
            
        print("Manual IP configuration created")
        print("To apply:")
        print("  sudo cp /tmp/interfaces_manual /etc/network/interfaces")
        print("  sudo cp /tmp/resolv.conf /etc/resolv.conf") 
        print("  sudo systemctl restart networking")
    except Exception as e:
        print(f"Error: {e}")

def show_current_config():
    print("\nCurrent Network Configuration:")
    
    try:
        result = subprocess.run(['ip', 'addr', 'show'], capture_output=True, text=True)
        print("\nIP Addresses:")
        for line in result.stdout.split('\n'):
            if 'inet ' in line:
                print(f"  {line.strip()}")
    except:
        print("  Could not get IP addresses")
    
    try:
        with open('/etc/resolv.conf', 'r') as f:
            print("\nDNS Configuration:")
            for line in f:
                print(f"  {line.strip()}")
    except:
        print("  Could not read DNS configuration")

def main_menu():
    client = NetworkClient()
    
    while True:
        print("\n" + "-" * 50)
        print("VM3 Network Client Application")
        print("-" * 50)
        print("1. Run full network test")
        print("2. DNS resolution test only")
        print("3. HTTP request test only")
        print("4. Network configuration")
        print("5. Network diagnostic")
        print("6. Exit")
        
        choice = input("\nSelect option (1-6): ").strip()
        
        if choice == "1":
            client.run_client_test()
        elif choice == "2":
            client.check_dns_resolution()
        elif choice == "3":
            client.make_http_request("/")
        elif choice == "4":
            configure_network()
        elif choice == "5":
            client.network_diagnostic()
        elif choice == "6":
            print("Goodbye")
            break
        else:
            print("Invalid option, please try again")

if __name__ == "__main__":
    try:
        main_menu()
    except KeyboardInterrupt:
        print("\nApplication terminated by user")
    except Exception as e:
        print(f"Unexpected error: {e}")
