#!/usr/bin/env python3
import subprocess
import requests

def run_cmd(cmd):
    return subprocess.run(cmd.split(), capture_output=True, text=True)

def start_nginx():
    run_cmd("sudo systemctl start nginx")
    run_cmd("sudo systemctl enable nginx")
    print("Nginx started")

def stop_nginx():
    run_cmd("sudo systemctl stop nginx")
    print("Nginx stopped")

def restart_nginx():
    run_cmd("sudo systemctl restart nginx")
    print("Nginx restarted")

def check_status():
    result = run_cmd("systemctl is-active nginx")
    print(f"Nginx Status: {result.stdout.strip()}")
    
    result = run_cmd("netstat -tulpn")
    for line in result.stdout.split('\n'):
        if ':8080' in line and 'nginx' in line:
            print("Port 8080: LISTENING")
            break
    else:
        print("Port 8080: NOT LISTENING")

def test_proxy():
    print("Testing reverse proxy...")
    try:
        r = requests.get("http://localhost:8080/proxy-status", timeout=5)
        print(f"Proxy Status: OK ({r.status_code})")
        print(f"Response: {r.text.strip()}")
    except Exception as e:
        print(f"Proxy Test: FAILED ({e})")
    
    try:
        r = requests.get("http://localhost:8080/", timeout=5)
        print(f"Proxy Forward: OK (Size: {len(r.text)} bytes)")
    except Exception as e:
        print(f"Proxy Forward: FAILED ({e})")

def test_backend():
    print("Testing backend connectivity...")
    try:
        r = requests.get("http://192.168.1.20:8080/", timeout=5)
        print(f"Backend: OK (Size: {len(r.text)} bytes)")
    except Exception as e:
        print(f"Backend: FAILED ({e})")

def setup_firewall():
    print("Configuring firewall...")
    commands = [
        "sudo ufw --force reset",
        "sudo ufw default deny incoming",
        "sudo ufw default allow outgoing",
        "sudo ufw allow 8080/tcp",
        "sudo ufw allow ssh",
        "sudo ufw allow from 192.168.1.0/24",
        "sudo ufw deny from 192.168.1.50",
        "sudo ufw deny from 192.168.1.60/30",
        "sudo ufw --force enable"
    ]
    
    for cmd in commands:
        result = run_cmd(cmd)
        if result.returncode != 0:
            print(f"Warning: {cmd} failed")
    
    print("Firewall configured:")
    print("- Allow HTTP traffic (port 8080)")
    print("- Allow SSH access") 
    print("- Allow internal network")
    print("- Block 192.168.1.50 and 192.168.1.60/30")

def firewall_status():
    result = run_cmd("sudo ufw status verbose")
    print("Firewall Status:")
    print(result.stdout)

def show_logs():
    print("Recent access logs:")
    result = run_cmd("sudo tail -5 /var/log/nginx/proxy_access.log")
    if result.stdout.strip():
        print(result.stdout)
    else:
        print("No access logs yet")
    
    print("\nRecent error logs:")
    result = run_cmd("sudo tail -5 /var/log/nginx/proxy_error.log")
    if result.stdout.strip():
        print(result.stdout)
    else:
        print("No error logs")

def run_diagnostics():
    print("Running VM4 Diagnostics")
    print("=" * 40)
    
    print("\n1. Service Status:")
    check_status()
    
    print("\n2. Backend Test:")
    test_backend()
    
    print("\n3. Proxy Test:")
    test_proxy()
    
    print("\n4. Firewall Status:")
    firewall_status()
    
    print("\nDiagnostics completed")

def main():
    while True:
        print("\n" + "=" * 40)
        print("VM4 Reverse Proxy Manager")
        print("=" * 40)
        print("1. Start Nginx")
        print("2. Stop Nginx")
        print("3. Restart Nginx")
        print("4. Check Status")
        print("5. Test Proxy")
        print("6. Test Backend")
        print("7. Setup Firewall")
        print("8. Firewall Status")
        print("9. Show Logs")
        print("10. Run Diagnostics")
        print("11. Exit")
        
        choice = input("\nSelect option (1-11): ").strip()
        
        if choice == "1":
            start_nginx()
        elif choice == "2":
            stop_nginx()
        elif choice == "3":
            restart_nginx()
        elif choice == "4":
            check_status()
        elif choice == "5":
            test_proxy()
        elif choice == "6":
            test_backend()
        elif choice == "7":
            setup_firewall()
        elif choice == "8":
            firewall_status()
        elif choice == "9":
            show_logs()
        elif choice == "10":
            run_diagnostics()
        elif choice == "11":
            print("Goodbye")
            break
        else:
            print("Invalid option")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nProgram terminated")
    except Exception as e:
        print(f"Error: {e}")
