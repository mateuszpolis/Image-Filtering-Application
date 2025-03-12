#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build with proper parallelism based on OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    CORES=$(sysctl -n hw.ncpu)
else
    # Linux and others
    CORES=$(nproc 2>/dev/null || echo 2)
fi

# Build
cmake --build . -- -j$CORES

echo "Build complete. Run the application with: ./ImageFiltering" 