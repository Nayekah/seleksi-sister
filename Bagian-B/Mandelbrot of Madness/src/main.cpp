#include "mandelbrot.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>

std::string get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

int main(int argc, char* argv[]) {
    const int MAX_CLI_RESOLUTION = 8000;
    const int MAX_CLI_ITERATIONS = 10000;
    
    int width = 1920;
    int height = 1080;
    int max_iterations = 1000;
    
    if (argc >= 4) {
        width = std::atoi(argv[1]);
        height = std::atoi(argv[2]);
        max_iterations = std::atoi(argv[3]);
    } else if (argc >= 3) {
        width = std::atoi(argv[1]);
        height = std::atoi(argv[2]);
    } else if (argc >= 2) {
        std::cout << "Usage: " << argv[0] << " [width] [height] [iterations]" << std::endl;
        std::cout << "Max resolution: " << MAX_CLI_RESOLUTION << "x" << MAX_CLI_RESOLUTION << std::endl;
        std::cout << "Max iterations: " << MAX_CLI_ITERATIONS << std::endl;
        std::cout << "Example: " << argv[0] << " 1920 1080 1000" << std::endl;
        return 1;
    }
    
    if (width <= 0 || width > MAX_CLI_RESOLUTION) {
        std::cerr << "Error: Width must be 1-" << MAX_CLI_RESOLUTION << std::endl;
        return 1;
    }
    if (height <= 0 || height > MAX_CLI_RESOLUTION) {
        std::cerr << "Error: Height must be 1-" << MAX_CLI_RESOLUTION << std::endl;
        return 1;
    }
    if (max_iterations <= 0 || max_iterations > MAX_CLI_ITERATIONS) {
        std::cerr << "Error: Iterations must be 1-" << MAX_CLI_ITERATIONS << std::endl;
        return 1;
    }
    
    std::cout << "=== Mandelbrot CLI Generator ===" << std::endl;
    std::cout << "Resolution: " << width << "x" << height << std::endl;
    std::cout << "Max iterations: " << max_iterations << std::endl;
    std::cout << "Total pixels: " << width * height << std::endl;
    std::cout << std::endl;
    
    print_system_info();
    
    MandelbrotGenerator generator(width, height, max_iterations);
    std::vector<Color> image(width * height);

    double serial_time = 0.0;
    double parallel_time = 0.0;
    double cuda_time = 0.0;

    std::cout << "=== Serial CPU Implementation ===" << std::endl;
    std::cout << "Running serial CPU implementation..." << std::endl;
    serial_time = benchmark_function([&]() {
        generator.generate_serial(image);
    });
    std::cout << "Serial time: " << std::fixed << std::setprecision(3) << serial_time << " seconds" << std::endl;

    std::string serial_filename = "../output/cli_mandelbrot_serial_" + 
                                 std::to_string(width) + "x" + std::to_string(height) + 
                                 "_iter" + std::to_string(max_iterations) + "_" + 
                                 get_timestamp() + ".bmp";
    generator.save_bmp(image, serial_filename);
    std::cout << "Saved: " << serial_filename << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Parallel CPU Implementation ===" << std::endl;
    std::cout << "Running parallel CPU implementation..." << std::endl;
    parallel_time = benchmark_function([&]() {
        generator.generate_parallel_threads(image);
    });
    std::cout << "Parallel CPU time: " << std::fixed << std::setprecision(3) << parallel_time << " seconds" << std::endl;
    std::cout << "CPU speedup: " << std::fixed << std::setprecision(2) << serial_time / parallel_time << "x" << std::endl;

    std::string parallel_filename = "../output/cli_mandelbrot_parallel_" + 
                                   std::to_string(width) + "x" + std::to_string(height) + 
                                   "_iter" + std::to_string(max_iterations) + "_" + 
                                   get_timestamp() + ".bmp";
    generator.save_bmp(image, parallel_filename);
    std::cout << "Saved: " << parallel_filename << std::endl;
    std::cout << std::endl;

    #ifdef USE_CUDA
    std::cout << "=== CUDA GPU Implementation ===" << std::endl;
    std::cout << "Initializing CUDA..." << std::endl;
    if (!generator.init_cuda()) {
        std::cout << "CUDA initialization failed" << std::endl;
    } else {
        std::cout << "Running CUDA GPU implementation..." << std::endl;
        cuda_time = benchmark_function([&]() {
            generator.generate_cuda(image);
        });
        std::cout << "CUDA GPU time: " << std::fixed << std::setprecision(3) << cuda_time << " seconds" << std::endl;
        std::cout << "GPU speedup vs Serial: " << std::fixed << std::setprecision(2) << serial_time / cuda_time << "x" << std::endl;
        std::cout << "GPU speedup vs Parallel CPU: " << std::fixed << std::setprecision(2) << parallel_time / cuda_time << "x" << std::endl;
        
        std::string cuda_filename = "../output/cli_mandelbrot_cuda_" + 
                                   std::to_string(width) + "x" + std::to_string(height) + 
                                   "_iter" + std::to_string(max_iterations) + "_" + 
                                   get_timestamp() + ".bmp";
        generator.save_bmp(image, cuda_filename);
        std::cout << "Saved: " << cuda_filename << std::endl;
        
        std::cout << std::endl;
        std::cout << "=== Performance Summary ===" << std::endl;
        std::cout << "┌─────────────────────┬─────────────┬─────────────┬─────────────────┐" << std::endl;
        std::cout << "│ Implementation      │ Time (s)    │ Speedup     │ Throughput      │" << std::endl;
        std::cout << "├─────────────────────┼─────────────┼─────────────┼─────────────────┤" << std::endl;
        
        double pixels_per_second_serial = (width * height) / serial_time;
        std::cout << "│ Serial CPU          │ " << std::setw(11) << std::fixed << std::setprecision(3) << serial_time 
                  << " │ " << std::setw(11) << "1.00x" << " │ " << std::setw(11) << std::scientific << std::setprecision(2) 
                  << pixels_per_second_serial << " px/s │" << std::endl;
        
        double pixels_per_second_parallel = (width * height) / parallel_time;
        std::cout << "│ Parallel CPU        │ " << std::setw(11) << std::fixed << std::setprecision(3) << parallel_time 
                  << " │ " << std::setw(11) << std::fixed << std::setprecision(2) << serial_time / parallel_time << "x" 
                  << " │ " << std::setw(11) << std::scientific << std::setprecision(2) << pixels_per_second_parallel << " px/s │" << std::endl;
        
        double pixels_per_second_cuda = (width * height) / cuda_time;
        std::cout << "│ CUDA GPU            │ " << std::setw(11) << std::fixed << std::setprecision(3) << cuda_time 
                  << " │ " << std::setw(11) << std::fixed << std::setprecision(2) << serial_time / cuda_time << "x" 
                  << " │ " << std::setw(11) << std::scientific << std::setprecision(2) << pixels_per_second_cuda << " px/s │" << std::endl;
        
        std::cout << "└─────────────────────┴─────────────┴─────────────┴─────────────────┘" << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== Analysis ===" << std::endl;
        std::cout << "Parallel efficiency: " << std::fixed << std::setprecision(1) 
                  << (serial_time / parallel_time) / std::thread::hardware_concurrency() * 100 << "%" << std::endl;
        std::cout << "GPU acceleration factor: " << std::fixed << std::setprecision(1) 
                  << serial_time / cuda_time << "x faster than serial" << std::endl;
        std::cout << "GPU vs CPU parallel: " << std::fixed << std::setprecision(1) 
                  << parallel_time / cuda_time << "x faster than parallel CPU" << std::endl;

        generator.cleanup_cuda();
    }
    #else
    std::cout << "=== CUDA GPU Implementation ===" << std::endl;
    std::cout << "CUDA not available (compile with USE_CUDA flag)" << std::endl;
    #endif
    
    std::cout << std::endl;
    std::cout << "=== CLI Generation Complete ===" << std::endl;
    std::cout << "All images saved to ../output/ with cli_mandelbrot_* prefix" << std::endl;
    std::cout << "Generated images:" << std::endl;
    std::cout << "  - Serial CPU result" << std::endl;
    std::cout << "  - Parallel CPU result" << std::endl;
    #ifdef USE_CUDA
    std::cout << "  - CUDA GPU result" << std::endl;
    #endif
    
    return 0;
}