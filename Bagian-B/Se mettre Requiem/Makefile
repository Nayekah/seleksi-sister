CC = gcc
CFLAGS = -O2 -Wall -Wextra -std=c99
TARGET = multiply
SOURCE = multiply.c

# Default target
all: $(TARGET)

# Compile the program
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)
	@echo "Compilation successful! Executable created: $(TARGET)"

# Clean compiled files
clean:
	rm -f $(TARGET) output/*.txt
	@echo "Cleaned up executable files and output files"

# Run the program
run: $(TARGET)
	./$(TARGET)

# Rebuild (clean and compile)
rebuild: clean all

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Compile the program (default)"
	@echo "  clean    - Remove compiled files and outputs"
	@echo "  run      - Compile and run the program"
	@echo "  rebuild  - Clean and recompile"
	@echo "  help     - Show this help message"

.PHONY: all clean run rebuild help