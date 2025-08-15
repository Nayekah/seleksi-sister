# Mandelbrot of Madness

> Seleksi Asisten Laboratorium Sistem Paralel dan Terdistribusi 2025
<p align="center">
    <img src="./doc/TheRecruiterBanner.png">
</p>

---

## About
<p align="justify">This project implements a high-performance Mandelbrot and Julia set fractal generator with multiple computational backends to demonstrate parallel computing optimization techniques. The system features four distinct implementations: a baseline serial CPU version, a multi-threaded parallel CPU implementation using std::thread, a GPU-accelerated CUDA version, and an interactive SFML-based GUI with real-time exploration capabilities. The generator supports resolutions up to 8000x8000 pixels with up to 10,000 iterations, providing automatic performance benchmarking to showcase speedup improvements from serial execution to massively parallel GPU acceleration. The GUI enables real-time fractal exploration with zoom capabilities up to 50x magnification, pan controls, and interactive Julia set generation through mouse hover, making it both an educational tool for understanding parallel computing concepts and a practical application for mathematical visualization. </p>

---

## Implementations

### CLI (Command Line Interface):
- **Serial with CPU**  
  Builds a mandelbrot set with CPU - Serial implementations, up to 8000 x 8000 resolutions and max 10000 iterations (image saving is supported)

- **Parallel with CPU**  
  Builds a mandelbrot set with CPU - Paralel implementations using std::thread, up to 8000 x 8000 resolutions and max 10000 iterations (image saving is supported)

- **GPU (Cuda)**  
  Builds a mandelbrot set with GPU using CUDA, up to 8000 x 8000 resolutions and max 10000 iterations (image saving is supported), fast computations

- **Benchmarking**  
  Showing full informations of the benchmark (all implementations)


### GUI (Command Line Interface):
- **Serial with CPU**  
  Builds a mandelbrot set with CPU - Serial implementations, up to 2000 x 2000 resolutions and max 10000 iterations (image saving is supported)

- **Parallel with CPU**  
  Builds a mandelbrot set with CPU - Paralel implementations using std::thread, up to 2000 x 2000 resolutions and max 10000 iterations (image saving is supported)

- **GPU (Cuda)**  
  Builds a mandelbrot set with GPU using CUDA, up to 2000 x 2000 resolutions and max 10000 iterations (image saving is supported), fast computations

- **Zoom**  
  Zooming with left mouse to drag intended zoom area, and mouse scroll to zoom in out, 'R' keys shortcut to reset the display. Real-time calculaations

- **Pan**  
  Panning the mandelbrot set with real time calculations using GPU power

- **Benchmarking**  
  Showing full informations of the benchmark (all implementations)

- **Julia Set**  
  Showing the julia set overview when hovering the mouse above the mandelbrot set respect to the cursor positions (top-right minimap). (julia set saving is supported with 'J' keys shortcut)

--- 

## How to Run

### Requirements
- g++/c++
- CUDA
- nvcc (CUDA compiler)
- Make
- SFML 2.5.1

> [!IMPORTANT]
> Make sure to use SFML 2.5.1 Version and setup the nvcc environment correctly.

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

