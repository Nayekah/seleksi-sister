#include "mandelbrot_gui.h"
#include <iostream>

int main() {
    std::cout << "=== Mandelbrot Fractal Generator GUI ===" << std::endl;
    std::cout << "Features:" << std::endl;
    std::cout << "- Input panel for resolution and iterations" << std::endl;
    std::cout << "- Automatic benchmarking (Serial + Parallel + CUDA)" << std::endl;
    std::cout << "- 3-page results view with method comparison" << std::endl;
    std::cout << "- Save individual or all results to ../output/" << std::endl;
    std::cout << "- Interactive zoom, pan, and reset controls" << std::endl;
    std::cout << std::endl;
    
    try {
        MandelbrotGUI gui;
        
        if (!gui.initialize()) {
            std::cerr << "Failed to initialize GUI" << std::endl;
            return -1;
        }
        
        std::cout << "GUI initialized successfully." << std::endl;
        std::cout << "Starting main event loop..." << std::endl;
        std::cout << std::endl;
        
        gui.run();
        
        std::cout << "GUI session ended." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}