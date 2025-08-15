#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <vector>
#include <complex>
#include <string>
#include <functional>
#include <thread>

struct Color {
    unsigned char r, g, b;
    Color() : r(0), g(0), b(0) {}
    Color(unsigned char red, unsigned char green, unsigned char blue) 
        : r(red), g(green), b(blue) {}
};

class MandelbrotGenerator {
private:
    int width, height;
    int max_iterations;
    double x_min, x_max, y_min, y_max;
    
public:
    MandelbrotGenerator(int w, int h, int max_iter = 1000);
    
    void set_bounds(double xmin, double xmax, double ymin, double ymax);
    
    int mandelbrot_iterations(std::complex<double> c);
    int julia_iterations(std::complex<double> z, std::complex<double> c);
    
    Color iterations_to_color(int iterations);
    
    // CPU implementations
    void generate_serial(std::vector<Color>& image);
    void generate_parallel_threads(std::vector<Color>& image);
    void generate_julia_serial(std::vector<Color>& image, std::complex<double> julia_c);
    void generate_julia_parallel(std::vector<Color>& image, std::complex<double> julia_c);
    
    // GPU implementation (CUDA)
    #ifdef USE_CUDA
    void generate_cuda(std::vector<Color>& image);
    void generate_julia_cuda(std::vector<Color>& image, std::complex<double> julia_c);
    bool init_cuda();
    void cleanup_cuda();
    #endif
    
    void save_bmp(const std::vector<Color>& image, const std::string& filename);
    
    int get_width() const { return width; }
    int get_height() const { return height; }
    int get_max_iterations() const { return max_iterations; }
};

// Utility functions
double benchmark_function(const std::function<void()>& func);
void print_system_info();

// CUDA wrapper functions
#ifdef USE_CUDA
extern "C" void launch_mandelbrot_kernel(unsigned char* d_image, int width, int height, int max_iterations,
                                         double x_min, double x_max, double y_min, double y_max);
extern "C" void launch_julia_kernel(unsigned char* d_image, int width, int height, int max_iterations,
                                    double x_min, double x_max, double y_min, double y_max,
                                    double c_real, double c_imag);
#endif

#endif // MANDELBROT_H