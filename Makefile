# Compiler
CXX = clang++

# Compiler flags
CXXFLAGS = -Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include -std=c++14
LDFLAGS = -L/usr/local/opt/libomp/lib -lomp

# Default source file (can be overridden by specifying SRC)
SRC ?= omp.cpp

# Extract the target name from the source file name
TARGET = $(basename $(SRC))

# Object file
OBJ = $(TARGET).o

# Default rule to build the target
all: $(TARGET)

# Rule to link the target
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Rule to compile the source file into an object file
$(OBJ): $(SRC)
	$(CXX) -c $(SRC) -o $(OBJ) $(CXXFLAGS)

# Clean rule to remove generated files
clean:
	rm -f $(OBJ) $(TARGET)

# Run rule to compile and run the program
run: all
	./$(TARGET)
