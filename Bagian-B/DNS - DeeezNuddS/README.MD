# DeezNudds

> Seleksi Asisten Laboratorium Sistem Paralel dan Terdistribusi 2025
<p align="center">
    <img src="./etc/newjeans-eta.gif">
</p>
    <h3 align="center">What's your DNS?</h3>

---

## About <a name="about"></a>
<div align="right">(<a href="#table-of-contents">back to top</a>)</div>  

<p align="justify">This project implements a simple DNS system running locally in linux using Debian, Bind9, Apache Server, and NGINX </p>

---

## Implementations <a name="algorithms"></a>

There are 4 Virtual Machines that implemented in Debian 13.0.0 (Trixie), you can refer it to https://www.debian.org/download for the .iso installations

### Virtual Machines (VMs)  
We support three classic exact‐match algorithms:

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
- Python 3.8
- Docker
- Dependencies (listed [here](./pyproject.toml))

> [!IMPORTANT]
> This project is managed with [uv](https://github.com/astral-sh/uv) and it's recommended to install the package manager on your system and run this application (_not glazing btw_)

### Running the Application With `uv`
1. Clone this repo
   ```bash
   git clone https://github.com/l0stplains/Tubes3_TheRecruiter.git
   ```
   > or just clicks button on your git gui if you feel uncomfortable with terminal
2. Navigate to the cloned repository.
   ```bash
   cd ./Tubes3_TheRecruiter
   ```
3. Setup `.env` like [example](./.env.example) with your data
   ```dotenv
   # Database Configuration
    DB_HOST=localhost
    DB_PORT=2025
    DB_NAME=ats_system
    DB_USER=gongyoo
    DB_PASSWORD=REDACTED
    
    # MySQL Root
    MYSQL_ROOT_PASSWORD=REDACTED
    
    # Encryption Configuration
    ENCRYPTION_PASSWORD=REDACTED
   ```
4. Run `docker-compose` (make sure you have docker instance running)
   ```bash
   docker-compose up -d
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
    <details>
        <summary>
            <i>Why Windows can't use <code>uv</code>?</i>
        </summary>
        <br/>
        The short answer is that <b>it's not compatible</b>. <i>well at least for this project</i>
        <br/>
        <br/>
        Using <code>uv</code> means we need to use a specific version constraints of the library that <b>built for</b> the project python version (3.8). The problem is that PyQtWebEngine version for python 3.8 does not support windows. By using pip install directly it bypass the constraints and install it for the system or venv. 
    </details>
7. Place all the CV pdf in `data/` folder at root
8. Run the seeder to fill the database
   ```bash
   uv run scripts/seeder.py
   ```
9. Run the program:
   ```bash
   uv run -m src -d gui
   ```
> [!NOTE]
> If you are planning to develop, you must set your system python to use version 3.8 and install pyqt5-tools

---

