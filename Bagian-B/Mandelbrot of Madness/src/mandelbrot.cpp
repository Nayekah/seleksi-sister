#include "mandelbrot.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cmath>

#ifdef USE_CUDA
#include <cuda_runtime.h>

static bool cuda_initialized = false;
static int cuda_device = 0;

extern "C" void launch_mandelbrot_kernel(unsigned char* d_image, int width, int height, int max_iterations,
                                         double x_min, double x_max, double y_min, double y_max);
extern "C" void launch_julia_kernel(unsigned char* d_image, int width, int height, int max_iterations,
                                    double x_min, double x_max, double y_min, double y_max,
                                    double c_real, double c_imag);

#endif // USE_CUDA

MandelbrotGenerator::MandelbrotGenerator(int w, int h, int max_iter) 
    : width(w), height(h), max_iterations(max_iter),
      x_min(-2.5), x_max(1.5), y_min(-2.0), y_max(2.0) {}

void MandelbrotGenerator::set_bounds(double xmin, double xmax, double ymin, double ymax) {
    x_min = xmin;
    x_max = xmax;
    y_min = ymin;
    y_max = ymax;
}

int MandelbrotGenerator::mandelbrot_iterations(std::complex<double> c) {
    std::complex<double> z = 0;
    int iterations = 0;
    
    while (std::abs(z) <= 2.0 && iterations < max_iterations) {
        z = z * z + c;
        iterations++;
    }
    
    return iterations;
}

int MandelbrotGenerator::julia_iterations(std::complex<double> z, std::complex<double> c) {
    int iterations = 0;
    
    while (std::abs(z) <= 2.0 && iterations < max_iterations) {
        z = z * z + c;
        iterations++;
    }
    
    return iterations;
}

Color MandelbrotGenerator::iterations_to_color(int iterations) {
    if (iterations == max_iterations) {
        return Color(0, 0, 0);
    }
    
    double t = (double)iterations / max_iterations;
    unsigned char r = (unsigned char)(9 * (1 - t) * t * t * t * 255);
    unsigned char g = (unsigned char)(15 * (1 - t) * (1 - t) * t * t * 255);
    unsigned char b = (unsigned char)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
    
    return Color(r, g, b);
}

void MandelbrotGenerator::generate_serial(std::vector<Color>& image) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double real = x_min + (x_max - x_min) * x / (width - 1);
            double imag = y_min + (y_max - y_min) * y / (height - 1);
            
            std::complex<double> c(real, imag);
            int iterations = mandelbrot_iterations(c);
            
            image[y * width + x] = iterations_to_color(iterations);
        }
    }
}

void MandelbrotGenerator::generate_parallel_threads(std::vector<Color>& image) {
    const int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    
    int rows_per_thread = height / num_threads;
    
    for (int t = 0; t < num_threads; t++) {
        int start_row = t * rows_per_thread;
        int end_row = (t == num_threads - 1) ? height : (t + 1) * rows_per_thread;
        
        threads.emplace_back([this, &image, start_row, end_row]() {
            for (int y = start_row; y < end_row; y++) {
                for (int x = 0; x < width; x++) {
                    double real = x_min + (x_max - x_min) * x / (width - 1);
                    double imag = y_min + (y_max - y_min) * y / (height - 1);
                    
                    std::complex<double> c(real, imag);
                    int iterations = mandelbrot_iterations(c);
                    
                    image[y * width + x] = iterations_to_color(iterations);
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

void MandelbrotGenerator::generate_julia_serial(std::vector<Color>& image, std::complex<double> julia_c) {
    // Julia set bounds
    double julia_x_min = -2.0, julia_x_max = 2.0;
    double julia_y_min = -2.0, julia_y_max = 2.0;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double real = julia_x_min + (julia_x_max - julia_x_min) * x / (width - 1);
            double imag = julia_y_min + (julia_y_max - julia_y_min) * y / (height - 1);
            
            std::complex<double> z(real, imag);
            int iterations = julia_iterations(z, julia_c);
            
            image[y * width + x] = iterations_to_color(iterations);
        }
    }
}

void MandelbrotGenerator::generate_julia_parallel(std::vector<Color>& image, std::complex<double> julia_c) {
    const int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    
    int rows_per_thread = height / num_threads;
    
    for (int t = 0; t < num_threads; t++) {
        int start_row = t * rows_per_thread;
        int end_row = (t == num_threads - 1) ? height : (t + 1) * rows_per_thread;
        
        threads.emplace_back([this, &image, julia_c, start_row, end_row]() {
            double julia_x_min = -2.0, julia_x_max = 2.0;
            double julia_y_min = -2.0, julia_y_max = 2.0;
            
            for (int y = start_row; y < end_row; y++) {
                for (int x = 0; x < width; x++) {
                    double real = julia_x_min + (julia_x_max - julia_x_min) * x / (width - 1);
                    double imag = julia_y_min + (julia_y_max - julia_y_min) * y / (height - 1);
                    
                    std::complex<double> z(real, imag);
                    int iterations = julia_iterations(z, julia_c);
                    
                    image[y * width + x] = iterations_to_color(iterations);
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

#ifdef USE_CUDA
bool MandelbrotGenerator::init_cuda() {
    if (cuda_initialized) return true;
    
    int device_count;
    cudaError_t err = cudaGetDeviceCount(&device_count);
    if (err != cudaSuccess || device_count == 0) {
        std::cerr << "No CUDA devices found: " << cudaGetErrorString(err) << std::endl;
        return false;
    }
    
    int best_device = 0;
    int best_major = 0, best_minor = 0;
    
    for (int i = 0; i < device_count; i++) {
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, i);
        
        if (prop.major > best_major || 
           (prop.major == best_major && prop.minor > best_minor)) {
            best_device = i;
            best_major = prop.major;
            best_minor = prop.minor;
        }
    }
    
    err = cudaSetDevice(best_device);
    if (err != cudaSuccess) {
        std::cerr << "Failed to set CUDA device: " << cudaGetErrorString(err) << std::endl;
        return false;
    }
    
    cuda_device = best_device;
    cuda_initialized = true;
    
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, cuda_device);
    std::cout << "Using CUDA device " << cuda_device << ": " << prop.name << std::endl;
    std::cout << "  Compute Capability: " << prop.major << "." << prop.minor << std::endl;
    std::cout << "  Global Memory: " << prop.totalGlobalMem / 1024 / 1024 << " MB" << std::endl;
    std::cout << "  Multiprocessors: " << prop.multiProcessorCount << std::endl;
    std::cout << std::endl;
    
    return true;
}

void MandelbrotGenerator::cleanup_cuda() {
    if (cuda_initialized) {
        cudaDeviceReset();
        cuda_initialized = false;
    }
}

void MandelbrotGenerator::generate_cuda(std::vector<Color>& image) {
    if (!init_cuda()) {
        std::cerr << "CUDA initialization failed" << std::endl;
        return;
    }
    
    unsigned char* d_image;
    size_t image_size = width * height * 3 * sizeof(unsigned char);
    
    cudaError_t err = cudaMalloc(&d_image, image_size);
    if (err != cudaSuccess) {
        std::cerr << "CUDA malloc failed: " << cudaGetErrorString(err) << std::endl;
        return;
    }
    
    launch_mandelbrot_kernel(d_image, width, height, max_iterations, x_min, x_max, y_min, y_max);
    
    unsigned char* host_image = new unsigned char[width * height * 3];
    err = cudaMemcpy(host_image, d_image, image_size, cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
        std::cerr << "CUDA memcpy failed: " << cudaGetErrorString(err) << std::endl;
        delete[] host_image;
        cudaFree(d_image);
        return;
    }
    
    for (int i = 0; i < width * height; i++) {
        image[i].r = host_image[i * 3];
        image[i].g = host_image[i * 3 + 1];
        image[i].b = host_image[i * 3 + 2];
    }
    
    delete[] host_image;
    cudaFree(d_image);
}

void MandelbrotGenerator::generate_julia_cuda(std::vector<Color>& image, std::complex<double> julia_c) {
    if (!init_cuda()) {
        std::cerr << "CUDA initialization failed" << std::endl;
        return;
    }
    
    unsigned char* d_image;
    size_t image_size = width * height * 3 * sizeof(unsigned char);
    
    cudaError_t err = cudaMalloc(&d_image, image_size);
    if (err != cudaSuccess) {
        std::cerr << "CUDA malloc failed: " << cudaGetErrorString(err) << std::endl;
        return;
    }
    
    // Julia set bounds
    double julia_x_min = -2.0, julia_x_max = 2.0;
    double julia_y_min = -2.0, julia_y_max = 2.0;
    
    launch_julia_kernel(d_image, width, height, max_iterations, 
                       julia_x_min, julia_x_max, julia_y_min, julia_y_max,
                       julia_c.real(), julia_c.imag());
    
    unsigned char* host_image = new unsigned char[width * height * 3];
    err = cudaMemcpy(host_image, d_image, image_size, cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
        std::cerr << "CUDA memcpy failed: " << cudaGetErrorString(err) << std::endl;
        delete[] host_image;
        cudaFree(d_image);
        return;
    }
    
    for (int i = 0; i < width * height; i++) {
        image[i].r = host_image[i * 3];
        image[i].g = host_image[i * 3 + 1];
        image[i].b = host_image[i * 3 + 2];
    }
    
    delete[] host_image;
    cudaFree(d_image);
}
#endif // USE_CUDA

void MandelbrotGenerator::save_bmp(const std::vector<Color>& image, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot create file " << filename << std::endl;
        return;
    }
    
    int file_size = 54 + 3 * width * height;
    int offset = 54;
    
    // BMP Header
    file.write("BM", 2);
    file.write(reinterpret_cast<const char*>(&file_size), 4);
    file.write("\0\0\0\0", 4);
    file.write(reinterpret_cast<const char*>(&offset), 4);
    
    // DIB Header
    int header_size = 40;
    file.write(reinterpret_cast<const char*>(&header_size), 4);
    file.write(reinterpret_cast<const char*>(&width), 4);
    file.write(reinterpret_cast<const char*>(&height), 4);
    
    short planes = 1;
    short bits_per_pixel = 24;
    file.write(reinterpret_cast<const char*>(&planes), 2);
    file.write(reinterpret_cast<const char*>(&bits_per_pixel), 2);
    
    int compression = 0;
    int image_size = 3 * width * height;
    int x_pixels_per_meter = 0;
    int y_pixels_per_meter = 0;
    int colors_used = 0;
    int colors_important = 0;
    
    file.write(reinterpret_cast<const char*>(&compression), 4);
    file.write(reinterpret_cast<const char*>(&image_size), 4);
    file.write(reinterpret_cast<const char*>(&x_pixels_per_meter), 4);
    file.write(reinterpret_cast<const char*>(&y_pixels_per_meter), 4);
    file.write(reinterpret_cast<const char*>(&colors_used), 4);
    file.write(reinterpret_cast<const char*>(&colors_important), 4);
    
    // Image data (BMP stores bottom-to-top)
    int row_padding = (4 - (width * 3) % 4) % 4;
    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            Color pixel = image[y * width + x];
            file.write(reinterpret_cast<const char*>(&pixel.b), 1);
            file.write(reinterpret_cast<const char*>(&pixel.g), 1);
            file.write(reinterpret_cast<const char*>(&pixel.r), 1);
        }
        for (int p = 0; p < row_padding; p++) {
            file.write("\0", 1);
        }
    }
    
    file.close();
    std::cout << "Image saved: " << filename << std::endl;
}

double benchmark_function(const std::function<void()>& func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return duration.count() / 1000.0;
}

void print_system_info() {
    std::cout << "=== System Information ===" << std::endl;
    std::cout << "Hardware threads: " << std::thread::hardware_concurrency() << std::endl;
    
    #ifdef USE_CUDA
    int device_count;
    cudaError_t err = cudaGetDeviceCount(&device_count);
    if (err == cudaSuccess && device_count > 0) {
        std::cout << "CUDA support: Yes" << std::endl;
        std::cout << "CUDA devices: " << device_count << std::endl;
        
        for (int i = 0; i < device_count; i++) {
            cudaDeviceProp prop;
            cudaGetDeviceProperties(&prop, i);
            std::cout << "  Device " << i << ": " << prop.name << std::endl;
        }
    } else {
        std::cout << "CUDA support: No" << std::endl;
    }
    #else
    std::cout << "CUDA support: No (not compiled with CUDA)" << std::endl;
    #endif
    
    std::cout << std::endl;
}