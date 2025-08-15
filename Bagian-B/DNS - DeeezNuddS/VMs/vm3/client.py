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
            result = subprocess.run(['ping', '-c', '3', ip_address], capture_output=True, text=True, timeout=self.timeout)
            
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
        print(f"Making http request to {url}")
        
        try:
            response = requests.get(url, timeout=self.timeout)
            print("HTTP Request successful")
            print(f"Status Code: {response.status_code}")
            print(f"Content Length: {len(response.text)} bytes")
            
            print("Response Headers:")
            for key, value in response.headers.items():
                print(f"    {key}: {value}")
                
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
                    print(f"    {line.strip()}")
        except:
            pass

        try:
            with open('/etc/resolv.conf', 'r') as f:
                print("\nDNS Configuration:")
                for line in f:
                    print(f"    {line.strip()}")
        except:
            pass

    def run_client_test(self):
        print("VM Network Client Application")
        print("=" * 50)
        
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

    def test_vm4_firewall(self):
        print("Testing VM4 Firewall from VM3 Client")
        print("=" * 45)
        
        vm4_ip = "192.168.1.40"
        
        # Test 1: HTTP Traffic Only
        print("\nTest 1: HTTP Traffic Only")
        print("-" * 35)
        
        print("Testing HTTP port 8080:")
        try:
            response = requests.get(f"http://{vm4_ip}:8080/proxy-status", timeout=5)
            if response.status_code == 200:
                print("Success")
                print(f"Response: {response.text.strip()}")
        except Exception as e:
            print(f"HTTP 8080 Blocked - {e}")

        # Test 2: HTTP 80
        print("\nTesting port 80...")
        if self.test_port_connection(vm4_ip, 80, timeout=3):
            print("  HTTP 80 ACCESSIBLE!")
        else:
            print("  HTTP 80 BLOCKED")

        # Test 3: HTTPS 443
        print("\nTesting HTTPS...")
        if self.test_port_connection(vm4_ip, 443, timeout=3):
            print("  HTTPS ACCESSIBLE!")
        else:
            print("  HTTPS BLOCKED")

        # Test 4: Current IP Access
        print("\nTest Blocked IP Range")
        print("-" * 35)
        
        current_ip = self.get_current_ip()
        print(f"\nTesting from current IP {current_ip}:")
        
        try:
            response = requests.get(f"http://{vm4_ip}:8080/proxy-status", timeout=5)
            print("  Current IP can access")
        except Exception as e:
            print(f"  Current IP Blocked: {e}")

        # Test 5: Blocked IP
        print("\nTesting blocked IP...")
        self.test_blocked_ip_simulation(vm4_ip)

        # Test 6: Target Port 8080 Only
        print("\nTesting target port 8080 ...")
        print("-" * 35)
        
        try:
            response = requests.get(f"http://{vm4_ip}:8080", timeout=5)
            print("  Target port 8080: Accessible")
        except:
            print("  Target port 8080 blocked - {e}")

        print("Testing other target ports...")
        target_ports = [80, 443, 3000, 5000]
        for port in target_ports:
            if self.test_port_connection(vm4_ip, port, timeout=2):
                print(f"  Target port {port}: ACCESSIBLE")
            else:
                print(f"  Target port {port}: blocked")

    def test_port_connection(self, host, port, timeout=3):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(timeout)
            result = sock.connect_ex((host, port))
            sock.close()
            return result == 0
        except:
            return False

    def test_blocked_ip_simulation(self, vm4_ip):
        blocked_ip = "192.168.1.50"
        print(f"  Adding blocked IP ({blocked_ip})...")
        
        add_result = subprocess.run(['ip', 'addr', 'add', f'{blocked_ip}/24', 'dev', 'enp0s3'], capture_output=True)
        
        if add_result.returncode == 0:
            time.sleep(2)
            print("  Testing access from blocked IP...")
            
            test_result = subprocess.run([
                'timeout', '8', 'wget', '--bind-address', blocked_ip, '--timeout=5', '-qO-', f'http://{vm4_ip}:8080/proxy-status'
            ], capture_output=True)
            
            if test_result.returncode == 0:
                print("  Firewall not working")
            else:
                print("  Firewall working")
                
            subprocess.run(['ip', 'addr', 'del', f'{blocked_ip}/24', 'dev', 'enp0s3'], capture_output=True)
            print("Temporary IP removed")
        else:
            print("Could not add temporary IP")

    def get_current_ip(self):
        try:
            result = subprocess.run(['ip', 'route', 'get', '1'], capture_output=True, text=True)
            for line in result.stdout.split('\n'):
                if 'src' in line:
                    return line.split('src')[1].split()[0]
            return "unknown"
        except:
            return "unknown"

    def configure_network(self):
        print("\nNetwork Configuration Menu")
        print("1. Use DHCP (automatic)")
        print("2. Set Manual IP")
        print("3. Current configuration")
        print("4. Back to main menu")
        
        choice = input("\nSelect option (1-4): ").strip()
        
        if choice == "1":
            self.configure_dhcp()
        elif choice == "2":
            self.configure_manual_ip()
        elif choice == "3":
            self.show_current_config()
        elif choice == "4":
            return
        else:
            print("Invalid option")

    def configure_dhcp(self):
        print("\nConfiguring DHCP...")
        
        print("Flushing existing IP addresses...")
        subprocess.run(['ip', 'addr', 'flush', 'dev', 'enp0s3'])
        
        dhcp_config = """auto enp0s3
iface enp0s3 inet dhcp
    dns-nameservers 192.168.1.10
    dns-search example.local
"""
        
        try:
            with open('/tmp/interfaces_dhcp', 'w') as f:
                f.write(dhcp_config)
                
            subprocess.run(['cp', '/tmp/interfaces_dhcp', '/etc/network/interfaces'])
            subprocess.run(['systemctl', 'restart', 'networking'])
            
            print("DHCP applied! Waiting for IP assignment...")
            time.sleep(5)
            
            dns_fix = "nameserver 192.168.1.10\nsearch example.local\n"
            with open('/tmp/resolv.conf', 'w') as f:
                f.write(dns_fix)
                
            subprocess.run(['cp', '/tmp/resolv.conf', '/etc/resolv.conf'])
            
            result = subprocess.run(['ip', 'addr', 'show', 'enp0s3'], capture_output=True, text=True)
            for line in result.stdout.split('\n'):
                if 'inet ' in line and '127.0.0.1' not in line:
                    print(f"New IP: {line.split()[1]}")
                    
            print("DHCP configuration completed!")
        except Exception as e:
            print(f"Error: {e}")

    def configure_manual_ip(self):
        print("\nManual IP Configuration")
        ip = input("Enter IP address (default: 192.168.1.30): ").strip() or "192.168.1.30"
        netmask = "255.255.255.0"
        dns = "192.168.1.10"
        
        print("Flushing existing IP address...")
        subprocess.run(['ip', 'addr', 'flush', 'dev', 'enp0s3'])
        
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
                
            subprocess.run(['cp', '/tmp/interfaces_manual', '/etc/network/interfaces'])
            time.sleep(2)
            subprocess.run(['cp', '/tmp/resolv.conf', '/etc/resolv.conf'])
            subprocess.run(['systemctl', 'restart', 'networking'])
            
            time.sleep(3)
            print(f"Manual IP Config created: {ip}")
        except Exception as e:
            print(f"Error: {e}")

    def show_current_config(self):
        print("\nCurrent Network Configuration:")
        
        try:
            result = subprocess.run(['ip', 'addr', 'show'], capture_output=True, text=True)
            print("\nIPAddresses:")
            for line in result.stdout.split('\n'):
                if 'inet ' in line:
                    print(f"    {line.strip()}")
        except:
            print("  Could not get IP addresses")
            
        try:
            with open('/etc/resolv.conf', 'r') as f:
                print("\nDNS Configuration:")
                for line in f:
                    print(f"    {line.strip()}")
        except:
            print("  Could not read DNS configuration")

def main_menu():
    client = NetworkClient()
    
    while True:
        print("\n" + "=" * 50)
        print("VM3 Network Client Application")
        print("=" * 50)
        print("1. Run full network test")
        print("2. DNS resolution test only")
        print("3. HTTP request test only")
        print("4. Network configuration")
        print("5. Network diagnostic")
        print("6. Test firewall")
        print("7. Exit")
        
        choice = input("\nSelect option (1-6): ").strip()
        
        if choice == "1":
            client.run_client_test()
        elif choice == "2":
            client.check_dns_resolution()
        elif choice == "3":
            client.make_http_request("/")
        elif choice == "4":
            client.configure_network()
        elif choice == "5":
            client.network_diagnostic()
        elif choice == "6":
            client.test_vm4_firewall()
        elif choice == "7":
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
