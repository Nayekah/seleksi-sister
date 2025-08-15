#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <iostream>

__device__ int cuda_mandelbrot_iterations(double real, double imag, int max_iterations) {
    double z_real = 0.0, z_imag = 0.0;
    int iterations = 0;
    
    while (z_real * z_real + z_imag * z_imag <= 4.0 && iterations < max_iterations) {
        double temp = z_real * z_real - z_imag * z_imag + real;
        z_imag = 2.0 * z_real * z_imag + imag;
        z_real = temp;
        iterations++;
    }
    
    return iterations;
}

__device__ int cuda_julia_iterations(double z_real, double z_imag, double c_real, double c_imag, int max_iterations) {
    int iterations = 0;
    
    while (z_real * z_real + z_imag * z_imag <= 4.0 && iterations < max_iterations) {
        double temp = z_real * z_real - z_imag * z_imag + c_real;
        z_imag = 2.0 * z_real * z_imag + c_imag;
        z_real = temp;
        iterations++;
    }
    
    return iterations;
}

__device__ void cuda_iterations_to_color(int iterations, int max_iterations, 
                                          unsigned char* r, unsigned char* g, unsigned char* b) {
    if (iterations == max_iterations) {
        *r = *g = *b = 0;
        return;
    }
    
    double t = (double)iterations / max_iterations;
    *r = (unsigned char)(9 * (1 - t) * t * t * t * 255);
    *g = (unsigned char)(15 * (1 - t) * (1 - t) * t * t * 255);
    *b = (unsigned char)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
}

__global__ void mandelbrot_cuda_kernel(unsigned char* image, int width, int height, int max_iterations,
                                       double x_min, double x_max, double y_min, double y_max) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x >= width || y >= height) return;
    
    double real = x_min + (x_max - x_min) * x / (width - 1);
    double imag = y_min + (y_max - y_min) * y / (height - 1);
    
    int iterations = cuda_mandelbrot_iterations(real, imag, max_iterations);
    
    unsigned char r, g, b;
    cuda_iterations_to_color(iterations, max_iterations, &r, &g, &b);
    
    int idx = (y * width + x) * 3;
    image[idx] = r;
    image[idx + 1] = g;
    image[idx + 2] = b;
}

__global__ void julia_cuda_kernel(unsigned char* image, int width, int height, int max_iterations,
                                  double x_min, double x_max, double y_min, double y_max,
                                  double c_real, double c_imag) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x >= width || y >= height) return;
    
    double z_real = x_min + (x_max - x_min) * x / (width - 1);
    double z_imag = y_min + (y_max - y_min) * y / (height - 1);
    
    int iterations = cuda_julia_iterations(z_real, z_imag, c_real, c_imag, max_iterations);
    
    unsigned char r, g, b;
    cuda_iterations_to_color(iterations, max_iterations, &r, &g, &b);
    
    int idx = (y * width + x) * 3;
    image[idx] = r;
    image[idx + 1] = g;
    image[idx + 2] = b;
}

extern "C" void launch_mandelbrot_kernel(unsigned char* d_image, int width, int height, int max_iterations,
                                         double x_min, double x_max, double y_min, double y_max) {
    int block_size_x = 16;
    int block_size_y = 16;
    
    int device;
    cudaGetDevice(&device);
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, device);
    
    if (prop.major >= 8) {
        block_size_x = 16;
        block_size_y = 16;
    }
    
    dim3 block_size(block_size_x, block_size_y);
    dim3 grid_size((width + block_size.x - 1) / block_size.x, 
                   (height + block_size.y - 1) / block_size.y);
    
    mandelbrot_cuda_kernel<<<grid_size, block_size>>>(d_image, width, height, max_iterations, 
                                                      x_min, x_max, y_min, y_max);
    
    cudaError_t err = cudaDeviceSynchronize();
    if (err != cudaSuccess) {
        std::cerr << "CUDA Mandelbrot kernel failed: " << cudaGetErrorString(err) << std::endl;
    }
}

extern "C" void launch_julia_kernel(unsigned char* d_image, int width, int height, int max_iterations,
                                    double x_min, double x_max, double y_min, double y_max,
                                    double c_real, double c_imag) {
    int block_size_x = 16;
    int block_size_y = 16;
    
    int device;
    cudaGetDevice(&device);
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, device);
    
    if (prop.major >= 8) {
        block_size_x = 16;
        block_size_y = 16;
    }
    
    dim3 block_size(block_size_x, block_size_y);
    dim3 grid_size((width + block_size.x - 1) / block_size.x, 
                   (height + block_size.y - 1) / block_size.y);
    
    julia_cuda_kernel<<<grid_size, block_size>>>(d_image, width, height, max_iterations, 
                                                 x_min, x_max, y_min, y_max, c_real, c_imag);
    
    cudaError_t err = cudaDeviceSynchronize();
    if (err != cudaSuccess) {
        std::cerr << "CUDA Julia kernel failed: " << cudaGetErrorString(err) << std::endl;
    }
}