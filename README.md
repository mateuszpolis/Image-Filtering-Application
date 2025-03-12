# Image Filtering Application

A Qt-based application for applying various image filters, including function filters and convolution filters.

## Features

### Function Filters
- Inversion
- Brightness Correction
- Contrast Enhancement
- Gamma Correction

### Convolution Filters
- Blur
- Gaussian Blur
- Sharpen
- Edge Detection
- Emboss
- Custom Filters with Editable Kernel

### Additional Features
- Load and display images
- Apply multiple filters sequentially
- Undo filter operations
- Reset image to original
- Save filtered images
- Custom convolution filter editor with:
  - Adjustable kernel size (rows and columns)
  - Editable kernel coefficients
  - Adjustable divisor (with auto-calculation option)
  - Adjustable offset
  - Adjustable anchor point
  - Save and load custom filters

## Requirements

- C++17 compatible compiler
- Qt 6.x
- CMake 3.16 or higher

## Building the Application

### Using Qt Creator
1. Open the CMakeLists.txt file in Qt Creator
2. Configure the project
3. Build and run

### Using Command Line
```bash
# Create a build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# Run the application
./ImageFiltering
```

## Usage

1. **Load an Image**: Use File > Open or the Open button to load an image
2. **Select Filter Type**: Choose between Function Filters and Convolution Filters
3. **Select Filter**: Choose a specific filter from the dropdown
4. **Adjust Parameters**: Modify filter parameters as needed
5. **Apply Filter**: Click the "Apply Filter" button to apply the selected filter
6. **Undo/Reset**: Use the Undo button to revert the last filter, or Reset to return to the original image
7. **Save Result**: Use File > Save or the Save button to save the filtered image

### Custom Convolution Filters

1. Select "Convolution Filters" as the filter type
2. Select "Custom" from the filter dropdown
3. Adjust the kernel size using the Rows and Columns spinboxes
4. Edit the kernel coefficients in the table
5. Adjust the divisor, offset, and anchor point as needed
6. Click "Save Filter" to save your custom filter for later use
7. Click "Load Filter" to load a previously saved custom filter

## License

This project is licensed under the MIT License - see the LICENSE file for details. 