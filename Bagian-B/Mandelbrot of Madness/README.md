# Mandelbrot of Madness

> Seleksi Asisten Laboratorium Sistem Paralel dan Terdistribusi 2025
<p align="center">
    <img src="./etc/explosion.gif">
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
   https://github.com/Nayekah/seleksi-sister.git
   ```

2. Navigate to the cloned repository.
   ```bash
   cd seleksi-sister
   cd "Bagian-B"
   cd "Mandelbrot of Madness"
   cd src
   ```
   
3. Do
   ```dotenv
   make cli (to make cli binary)
   make gui (to make gui binary)

   # Or

   make all (to make both)
   ```
   
4. To run in cli mode, do:
   ```bash
   make run-cli # default

   # Or

   ./mandelbrot [width] [height] [iterations]
   ```

5. To run in gui mode, do:
   ```bash
   make run-gui # default

   # Or

   ./mandelbrot_gui
   ```

6. Cheatsheets for GUI
   ```bash
   left mouse drag         : zoom
   right mouse drag        : pan
   'R' or 's'              : Reset to original position
   'S' or 's'              : Save the current mandelbrot set view
   'J' or 'j'              : Save the current position julia set
   mouse scroll            : zoom in/out (alternative)
   escape button (esc)     : return to input window
   ```
   
> [!NOTE]
> If you are planning to develop, you must set your system python to use version 3.8 and install pyqt5-tools

---

