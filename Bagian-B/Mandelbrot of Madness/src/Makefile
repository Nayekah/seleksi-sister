CXX = g++
NVCC = nvcc # export PATH=/usr/local/cuda/bin:$PATH, adjust it based on your env
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra
CUDAFLAGS = -O3 -arch=sm_50 -std=c++17
SFMLFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Source files
CLI_SOURCES = main.cpp mandelbrot.cpp
GUI_SOURCES = main_gui.cpp mandelbrot_gui.cpp mandelbrot.cpp
HEADERS = mandelbrot.h mandelbrot_gui.h
CUDA_KERNEL = mandelbrot_kernel.cu

# Target executables
CLI_TARGET = mandelbrot
GUI_TARGET = mandelbrot_gui

# Check for CUDA availability
CUDA_AVAILABLE := $(shell command -v nvcc >/dev/null 2>&1 && echo yes || echo no)

# Default target
all: cli gui

# CLI version (CUDA required)
cli: $(CLI_TARGET)

$(CLI_TARGET): check-cuda output-dir
ifeq ($(CUDA_AVAILABLE),yes)
	@echo "Building CLI version with CUDA..."
	$(CXX) $(CXXFLAGS) -DUSE_CUDA -I/usr/local/cuda/include -c $(CLI_SOURCES)
	$(NVCC) $(CUDAFLAGS) -c $(CUDA_KERNEL)
	$(CXX) $(CXXFLAGS) -o $(CLI_TARGET) *.o -lcudart -L/usr/local/cuda/lib64
	@rm -f *.o
	@echo "CLI build complete: $(CLI_TARGET)"
	@echo "Usage: ./$(CLI_TARGET) [width] [height] [iterations]"
	@echo "Max resolution: 8000x8000, Max iterations: 10000"
else
	@echo "Error: CUDA required for CLI version"
	@echo "Install CUDA: sudo apt install nvidia-cuda-toolkit"
	@exit 1
endif

# GUI version (CUDA required)
gui: $(GUI_TARGET)

$(GUI_TARGET): check-sfml check-cuda output-dir
ifeq ($(CUDA_AVAILABLE),yes)
	@echo "Building GUI version with CUDA..."
	$(CXX) $(CXXFLAGS) -DUSE_CUDA -I/usr/local/cuda/include -c $(GUI_SOURCES)
	$(NVCC) $(CUDAFLAGS) -c $(CUDA_KERNEL)
	$(CXX) $(CXXFLAGS) -o $(GUI_TARGET) *.o $(SFMLFLAGS) -lcudart -L/usr/local/cuda/lib64
	@rm -f *.o
	@echo "GUI build complete: $(GUI_TARGET)"
else
	@echo "Error: CUDA required for GUI version"
	@echo "Install CUDA: sudo apt install nvidia-cuda-toolkit"
	@exit 1
endif

# Create output directory
output-dir:
	@mkdir -p ../output

# Run targets
run-cli: cli
	@echo "Running CLI with default settings (1920x1080, 1000 iterations)..."
	./$(CLI_TARGET) 1920 1080 1000

run-gui: gui
	./$(GUI_TARGET)

# Quick test runs
test-cli: cli
	@echo "Testing CLI with small image (800x600, 500 iterations)..."
	./$(CLI_TARGET) 800 600 500

test-gui: gui
	./$(GUI_TARGET)

# Debug builds
debug-cli: CXXFLAGS += -g -DDEBUG
debug-cli: CUDAFLAGS += -g -G
debug-cli: 
	@echo "Building CLI debug version..."
	$(CXX) $(CXXFLAGS) -DUSE_CUDA -I/usr/local/cuda/include -c $(CLI_SOURCES)
	$(NVCC) $(CUDAFLAGS) -c $(CUDA_KERNEL)
	$(CXX) $(CXXFLAGS) -o $(CLI_TARGET) *.o -lcudart -L/usr/local/cuda/lib64
	@echo "CLI debug build complete"

debug-gui: CXXFLAGS += -g -DDEBUG
debug-gui: CUDAFLAGS += -g -G
debug-gui:
	@echo "Building GUI debug version..."
	$(CXX) $(CXXFLAGS) -DUSE_CUDA -I/usr/local/cuda/include -c $(GUI_SOURCES)
	$(NVCC) $(CUDAFLAGS) -c $(CUDA_KERNEL)
	$(CXX) $(CXXFLAGS) -o $(GUI_TARGET) *.o $(SFMLFLAGS) -lcudart -L/usr/local/cuda/lib64
	@echo "GUI debug build complete"

# Dependency checks
check-sfml:
	@pkg-config --exists sfml-graphics sfml-window sfml-system || \
	(echo "Error: SFML not found. Install with: sudo apt install libsfml-dev" && exit 1)

check-cuda:
	@command -v nvcc >/dev/null 2>&1 || \
	(echo "Error: CUDA not found. Install with: sudo apt install nvidia-cuda-toolkit" && exit 1)

# Clean targets
clean:
	rm -f *.o $(CLI_TARGET) $(GUI_TARGET) *.bmp

clean-all: clean
	rm -rf ../output

# Help
help:
	@echo "Mandelbrot Fractal Generator - CLI and GUI"
	@echo "=========================================="
	@echo ""
	@echo "Build targets:"
	@echo "  make cli        - Build CLI version (mandelbrot)"
	@echo "  make gui        - Build GUI version (mandelbrot_gui)"
	@echo "  make all        - Build both CLI and GUI"
	@echo ""
	@echo "Run targets:"
	@echo "  make run-cli    - Build and run CLI (1920x1080, 1000 iter)"
	@echo "  make run-gui    - Build and run GUI"
	@echo "  make test-cli   - Quick CLI test (800x600, 500 iter)"
	@echo "  make test-gui   - Quick GUI test"
	@echo ""
	@echo "CLI Usage:"
	@echo "  ./mandelbrot [width] [height] [iterations]"
	@echo "  Max resolution: 8000x8000, Max iterations: 10000"
	@echo "  Output: Auto-saved to ../output/cli_mandelbrot_*.bmp"
	@echo ""
	@echo "GUI Usage:"
	@echo "  ./mandelbrot_gui"
	@echo "  Interactive interface with real-time Julia preview"
	@echo "  Output: Saved to ../output/ on user request"
	@echo ""
	@echo "Requirements:"
	@echo "  CUDA: nvidia-cuda-toolkit (REQUIRED for both)"
	@echo "  SFML: libsfml-dev (for GUI only)"

.PHONY: all cli gui run-cli run-gui test-cli test-gui debug-cli debug-gui check-sfml check-cuda clean clean-all help output-dir