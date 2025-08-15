# Mandelbrot & Julia Set Generator

Generator himpunan Mandelbrot dan Julia yang canggih dengan implementasi serial, paralel CPU, GPU (CUDA), dan GUI interaktif.

## ✨ Fitur Utama

### 🖥️ Implementasi Multiple
- **Serial**: Implementasi dasar single-thread
- **Paralel CPU**: Menggunakan OpenMP dan std::thread
- **GPU CUDA**: Akselerasi menggunakan NVIDIA CUDA
- **Benchmark**: Perbandingan performa otomatis

### 🎮 GUI Interaktif
- **Zoom dinamis**: Klik dan drag untuk zoom ke area tertentu
- **Mode Julia**: Toggle antara Mandelbrot dan Julia set
- **Real-time**: Perubahan konstanta Julia dengan gerakan mouse
- **Pan & Navigate**: Navigasi mudah dengan mouse dan keyboard

### 📊 Export & Visualisasi
- **Format multiple**: Export ke BMP dan PNG
- **Resolusi tinggi**: Mendukung hingga 4K+ resolution
- **Coloring dinamis**: Algoritma pewarnaan HSV yang smooth

## 🛠️ Prerequisites

### Dependencies Dasar
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y libsfml-dev libomp-dev build-essential

# Atau gunakan Makefile
make install-deps
```

### CUDA (Opsional)
```bash
# Install CUDA toolkit
make install-cuda

# Atau manual install dari NVIDIA website
```

## 🚀 Kompilasi & Instalasi

### Build CPU Version (Recommended)
```bash
# Clone atau copy semua file ke directory
make cpu
```

### Build CUDA Version
```bash
# Membutuhkan NVIDIA GPU dan CUDA toolkit
make cuda
```

### Build Options Lain
```bash
make debug     # Debug version
make profile   # Profiling version  
make native    # Optimized untuk CPU saat ini
```

## 📖 Cara Penggunaan

### GUI Mode (Default)
```bash
./mandelbrot_cpu
# atau
make gui
```

#### Kontrol GUI:
- **Left Click + Drag**: Zoom ke area yang dipilih
- **Right Click**: Reset view ke default
- **J**: Toggle antara Mandelbrot dan Julia set
- **Mouse Move**: Ubah konstanta Julia (mode Julia)
- **S**: Simpan gambar saat ini
- **R**: Reset view
- **ESC**: Keluar

### Benchmark Mode
```bash
./mandelbrot_cpu --benchmark
# atau
make benchmark
```

### Command Line Options
```bash
./mandelbrot_cpu [options]

Options:
  --gui, -g        Launch GUI mode (default)
  --benchmark, -b  Run command line benchmark
  --help, -h       Show help message
```

## 📈 Performa & Benchmarking

Program secara otomatis melakukan benchmark untuk semua implementasi:

### Resolusi Testing:
- **800x600** - Testing dasar
- **1920x1080** - Full HD
- **3840x2160** - 4K

### Metrik Performa:
- **Waktu eksekusi** untuk setiap implementasi
- **Speedup ratio** paralel vs serial
- **GPU speedup** vs CPU implementations

### Contoh Output:
```
=== BENCHMARK RESULTS ===
Serial time:           2.456 seconds
Parallel time (OpenMP): 0.312 seconds
Parallel speedup:      7.87x
GPU time (CUDA):       0.089 seconds
GPU speedup:           27.6x
========================
```

## 🎨 Algoritma & Implementasi

### Mandelbrot Set
Formula: `z_{n+1} = z_n^2 + c`
- `z_0 = 0`
- `c` adalah koordinat kompleks pada bidang

### Julia Set  
Formula: `z_{n+1} = z_n^2 + c`
- `z_0` adalah koordinat kompleks pada bidang
- `c` adalah konstanta kompleks (dapat diubah)

### Coloring Algorithm
- **HSV Color Space**: Memberikan transisi warna yang smooth
- **Iteration-based**: Warna berdasarkan jumlah iterasi sebelum divergence
- **Escape radius**: Menggunakan radius 2.0 untuk deteksi divergence

### Parallelization Strategy
- **Row-based division**: Setiap thread mengerjakan set baris piksel
- **Dynamic scheduling**: OpenMP dynamic scheduling untuk load balancing
- **GPU kernel**: CUDA kernel 2D dengan block size 16x16

## 📁 Struktur File

```
mandelbrot_project/
├── mandelbrot.h              # Header dengan deklarasi class
├── mandelbrot.cpp            # Implementasi core algorithm
├── mandelbrot_gui.cpp        # Implementasi GUI
├── main.cpp                  # Program utama
├── Makefile                  # Build system
├── README.md                 # Dokumentasi ini
└── output/                   # Directory untuk output images
    ├── mandelbrot_*.bmp/png  # Generated Mandelbrot sets
    └── julia_set_*.png       # Generated Julia sets
```

## 🔧 Kustomisasi & Ekstensibilitas

### Mengubah Parameter Default
```cpp
// Di main.cpp atau saat create MandelbrotParams
MandelbrotParams custom_params(
    -2.5, 1.5,    // xmin, xmax
    -2.0, 2.0,    // ymin, ymax  
    1920, 1080,   // width, height
    2000          // max_iterations
);
```

### Menambah Julia Constants
```cpp
// Di main.cpp, tambahkan konstanta Julia menarik
std::vector<std::complex<double>> julia_constants = {
    std::complex<double>(-0.7269, 0.1889),  // Dragon
    std::complex<double>(-0.8, 0.156),      // Rabbit
    std::complex<double>(-0.4, 0.6),        // Spiral
    std::complex<double>(0.285, 0.01),      // Airplane
    // Tambahkan konstanta baru di sini
};
```

### Custom Color Schemes
Modifikasi fungsi `iterations_to_color()` di `mandelbrot.cpp` untuk skema warna berbeda.

## 🐛 Troubleshooting

### GUI Tidak Muncul
```bash
# Check SFML installation
pkg-config --libs sfml-all

# Fallback ke benchmark mode
./mandelbrot_cpu --benchmark
```

### CUDA Compilation Error
```bash
# Check CUDA installation
nvcc --version

# Check GPU compatibility
nvidia-smi

# Build CPU version instead
make cpu
```

### Performance Issues
```bash
# Check OpenMP support
echo $OMP_NUM_THREADS

# Set thread count manually
export OMP_NUM_THREADS=8
```

### Memory Issues (High Resolution)
- Reduce resolution untuk testing awal
- Monitor memory usage: `htop` atau `nvidia-smi`
- Untuk 4K+: pastikan RAM >= 8GB

## 📊 Sample Results

### Speedup Examples (i7-8700K + RTX 3070):
- **8-core CPU**: ~6-8x speedup vs serial
- **GPU CUDA**: ~20-30x speedup vs serial
- **Memory bandwidth**: GPU lebih efisien untuk resolusi tinggi

### Generated Images:
- **Mandelbrot**: Classic black set dengan colorful escape regions
- **Julia Sets**: Berbagai pola menarik tergantung konstanta
- **High-res**: Detail fraktal yang tajam pada zoom tinggi

## 🔗 Referensi & Credits

- **Mandelbrot Set**: [Wikipedia](https://en.wikipedia.org/wiki/Mandelbrot_set)
- **Julia Set**: [Wikipedia](https://en.wikipedia.org/wiki/Julia_set)
- **SFML**: Graphics library untuk GUI
- **OpenMP**: Parallel programming API
- **CUDA**: NVIDIA parallel computing platform

## 📝 License

Program ini dibuat untuk tujuan educational dan dapat dimodifikasi sesuai kebutuhan.

---

**Happy Fractal Exploring! 🌀**