#include "filters/functionfilters.h"
#include <QColor>
#include <cmath>

// Base FunctionFilter implementation
FunctionFilter::FunctionFilter(const QString &name) : name(name) {}

FunctionFilter::~FunctionFilter() {}

QString FunctionFilter::getName() const {
    return name;
}

QRgb FunctionFilter::applyToPixel(QRgb pixel, std::function<int(int)> func) {
    int r = qRed(pixel);
    int g = qGreen(pixel);
    int b = qBlue(pixel);
    
    r = qBound(0, func(r), 255);
    g = qBound(0, func(g), 255);
    b = qBound(0, func(b), 255);
    
    return qRgba(r, g, b, qAlpha(pixel));
}

// InversionFilter implementation
InversionFilter::InversionFilter() : FunctionFilter("Inversion") {}

QImage InversionFilter::apply(const QImage &image) {
    QImage result = image.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            result.setPixel(x, y, applyToPixel(pixel, [](int value) {
                return 255 - value;
            }));
        }
    }
    
    return result;
}

// BrightnessFilter implementation
BrightnessFilter::BrightnessFilter(double factor) 
    : FunctionFilter("Brightness"), factor(factor) {}

QImage BrightnessFilter::apply(const QImage &image) {
    QImage result = image.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            result.setPixel(x, y, applyToPixel(pixel, [this](int value) {
                return value + static_cast<int>(factor);
            }));
        }
    }
    
    return result;
}

void BrightnessFilter::setFactor(double factor) {
    this->factor = factor;
}

double BrightnessFilter::getFactor() const {
    return factor;
}

// ContrastFilter implementation
ContrastFilter::ContrastFilter(double factor) 
    : FunctionFilter("Contrast"), factor(factor) {}

QImage ContrastFilter::apply(const QImage &image) {
    QImage result = image.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            result.setPixel(x, y, applyToPixel(pixel, [this](int value) {
                return static_cast<int>((value - 128) * factor + 128);
            }));
        }
    }
    
    return result;
}

void ContrastFilter::setFactor(double factor) {
    this->factor = factor;
}

double ContrastFilter::getFactor() const {
    return factor;
}

// GammaFilter implementation
GammaFilter::GammaFilter(double gamma) 
    : FunctionFilter("Gamma"), gamma(gamma) {}

QImage GammaFilter::apply(const QImage &image) {
    QImage result = image.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            result.setPixel(x, y, applyToPixel(pixel, [this](int value) {
                return static_cast<int>(255.0 * pow(value / 255.0, 1.0 / gamma));
            }));
        }
    }
    
    return result;
}

void GammaFilter::setGamma(double gamma) {
    this->gamma = gamma;
}

double GammaFilter::getGamma() const {
    return gamma;
}

// GrayscaleFilter implementation
GrayscaleFilter::GrayscaleFilter() : FunctionFilter("Grayscale") {}

QImage GrayscaleFilter::apply(const QImage &image) {
    QImage result = image.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            int r = qRed(pixel);
            int g = qGreen(pixel);
            int b = qBlue(pixel);
            
            // Standard grayscale conversion formula (ITU-R BT.601)
            int gray = qRound(0.299 * r + 0.587 * g + 0.114 * b);
            gray = qBound(0, gray, 255);
            
            result.setPixel(x, y, qRgba(gray, gray, gray, qAlpha(pixel)));
        }
    }
    
    return result;
}

// UniformQuantizationFilter implementation
UniformQuantizationFilter::UniformQuantizationFilter(int rLevels, int gLevels, int bLevels)
    : FunctionFilter("Uniform Quantization"), rLevels(rLevels), gLevels(gLevels), bLevels(bLevels) {}

QImage UniformQuantizationFilter::apply(const QImage &image) {
    QImage result = image.copy();
    
    // Calculate the size of each level/step for each color channel
    double rStep = 256.0 / rLevels;
    double gStep = 256.0 / gLevels;
    double bStep = 256.0 / bLevels;
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            
            int r = qRed(pixel);
            int g = qGreen(pixel);
            int b = qBlue(pixel);
            
            // Quantize each color channel
            // First determine which level the color falls into
            int rLevel = static_cast<int>(r / rStep);
            int gLevel = static_cast<int>(g / gStep);
            int bLevel = static_cast<int>(b / bStep);
            
            // Ensure we don't exceed the maximum level
            rLevel = qMin(rLevel, rLevels - 1);
            gLevel = qMin(gLevel, gLevels - 1);
            bLevel = qMin(bLevel, bLevels - 1);
            
            // Map the level back to a color value (center of the level's range)
            r = static_cast<int>((rLevel + 0.5) * rStep);
            g = static_cast<int>((gLevel + 0.5) * gStep);
            b = static_cast<int>((bLevel + 0.5) * bStep);
            
            // Ensure the values are within valid range
            r = qBound(0, r, 255);
            g = qBound(0, g, 255);
            b = qBound(0, b, 255);
            
            result.setPixel(x, y, qRgba(r, g, b, qAlpha(pixel)));
        }
    }
    
    return result;
}

void UniformQuantizationFilter::setLevels(int rLevels, int gLevels, int bLevels) {
    this->rLevels = qMax(2, rLevels); // Minimum 2 levels
    this->gLevels = qMax(2, gLevels);
    this->bLevels = qMax(2, bLevels);
}

int UniformQuantizationFilter::getRedLevels() const {
    return rLevels;
}

int UniformQuantizationFilter::getGreenLevels() const {
    return gLevels;
}

int UniformQuantizationFilter::getBlueLevels() const {
    return bLevels;
}

// DitheringFilter implementation
DitheringFilter::DitheringFilter(int rLevels, int gLevels, int bLevels, KernelType kernelType)
    : FunctionFilter("Dithering"), rLevels(rLevels), gLevels(gLevels), bLevels(bLevels), kernelType(kernelType) {}

QImage DitheringFilter::apply(const QImage &image) {
    // Detect if the image is grayscale
    bool isGrayscale = true;
    for (int y = 0; y < image.height() && isGrayscale; ++y) {
        for (int x = 0; x < image.width() && isGrayscale; ++x) {
            QRgb pixel = image.pixel(x, y);
            if (qRed(pixel) != qGreen(pixel) || qRed(pixel) != qBlue(pixel)) {
                isGrayscale = false;
                break;
            }
        }
    }
    
    // Apply appropriate dithering based on image type
    if (isGrayscale) {
        return applyToGrayscale(image);
    } else {
        return applyToColor(image);
    }
}

void DitheringFilter::setLevels(int rLevels, int gLevels, int bLevels) {
    this->rLevels = qMax(2, rLevels); // Minimum 2 levels
    this->gLevels = qMax(2, gLevels);
    this->bLevels = qMax(2, bLevels);
}

void DitheringFilter::setKernelType(KernelType kernelType) {
    this->kernelType = kernelType;
}

int DitheringFilter::getRedLevels() const {
    return rLevels;
}

int DitheringFilter::getGreenLevels() const {
    return gLevels;
}

int DitheringFilter::getBlueLevels() const {
    return bLevels;
}

DitheringFilter::KernelType DitheringFilter::getKernelType() const {
    return kernelType;
}

QStringList DitheringFilter::getKernelNames() {
    return {
        "Floyd-Steinberg",
        "Burkes",
        "Stucki",
        "Sierra",
        "Atkinson"
    };
}

int DitheringFilter::quantizeValue(int value, int levels) {
    if (levels <= 1) return 0;
    
    // Calculate step size for the given number of levels
    double step = 255.0 / (levels - 1);
    
    // Find the closest level
    int level = qRound(value / step);
    
    // Map the level back to a color value
    return qBound(0, static_cast<int>(level * step), 255);
}

QImage DitheringFilter::applyToGrayscale(const QImage &image) {
    // Create a copy of the image
    QImage result = image.convertToFormat(QImage::Format_RGB32);
    
    // Create error buffer with some extra space on borders to simplify algorithm
    int width = result.width();
    int height = result.height();
    QVector<QVector<double>> errors(height + 2, QVector<double>(width + 2, 0.0));
    
    // Get diffusion kernel
    QVector<DiffusionCoefficient> kernel = getDiffusionKernel();
    
    // Process the image
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QRgb pixel = result.pixel(x, y);
            int oldValue = qRed(pixel);  // For grayscale, all R,G,B are the same
            
            // Apply accumulated error
            int newValue = qBound(0, oldValue + qRound(errors[y][x]), 255);
            
            // Quantize the value
            int quantizedValue = quantizeValue(newValue, rLevels);  // Using rLevels for grayscale
            
            // Set the new pixel value
            result.setPixel(x, y, qRgb(quantizedValue, quantizedValue, quantizedValue));
            
            // Calculate the error
            int error = newValue - quantizedValue;
            
            // Distribute the error according to the kernel
            for (const auto &coeff : kernel) {
                int newX = x + coeff.x;
                int newY = y + coeff.y;
                
                // Make sure we're within bounds
                if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                    errors[newY][newX] += error * coeff.weight;
                }
            }
        }
    }
    
    return result;
}

QImage DitheringFilter::applyToColor(const QImage &image) {
    // Create a copy of the image
    QImage result = image.convertToFormat(QImage::Format_RGB32);
    
    // Create error buffers for each channel
    int width = result.width();
    int height = result.height();
    QVector<QVector<double>> errorsR(height + 2, QVector<double>(width + 2, 0.0));
    QVector<QVector<double>> errorsG(height + 2, QVector<double>(width + 2, 0.0));
    QVector<QVector<double>> errorsB(height + 2, QVector<double>(width + 2, 0.0));
    
    // Get diffusion kernel
    QVector<DiffusionCoefficient> kernel = getDiffusionKernel();
    
    // Process the image
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QRgb pixel = result.pixel(x, y);
            
            // Get original color values
            int oldR = qRed(pixel);
            int oldG = qGreen(pixel);
            int oldB = qBlue(pixel);
            
            // Apply accumulated errors
            int newR = qBound(0, oldR + qRound(errorsR[y][x]), 255);
            int newG = qBound(0, oldG + qRound(errorsG[y][x]), 255);
            int newB = qBound(0, oldB + qRound(errorsB[y][x]), 255);
            
            // Quantize each channel
            int quantizedR = quantizeValue(newR, rLevels);
            int quantizedG = quantizeValue(newG, gLevels);
            int quantizedB = quantizeValue(newB, bLevels);
            
            // Set the new pixel value
            result.setPixel(x, y, qRgb(quantizedR, quantizedG, quantizedB));
            
            // Calculate the errors
            int errorR = newR - quantizedR;
            int errorG = newG - quantizedG;
            int errorB = newB - quantizedB;
            
            // Distribute the errors according to the kernel
            for (const auto &coeff : kernel) {
                int newX = x + coeff.x;
                int newY = y + coeff.y;
                
                // Make sure we're within bounds
                if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
                    errorsR[newY][newX] += errorR * coeff.weight;
                    errorsG[newY][newX] += errorG * coeff.weight;
                    errorsB[newY][newX] += errorB * coeff.weight;
                }
            }
        }
    }
    
    return result;
}

QVector<DitheringFilter::DiffusionCoefficient> DitheringFilter::getDiffusionKernel() {
    QVector<DiffusionCoefficient> kernel;
    
    switch (kernelType) {
        case FLOYD_STEINBERG:
            // Floyd-Steinberg kernel:
            //     *  7/16
            // 3/16 5/16 1/16
            kernel = {
                {1, 0, 7.0/16.0},
                {-1, 1, 3.0/16.0},
                {0, 1, 5.0/16.0},
                {1, 1, 1.0/16.0}
            };
            break;
            
        case BURKES:
            // Burkes kernel:
            //       *  8/32  4/32
            // 2/32 4/32 8/32 4/32 2/32
            kernel = {
                {1, 0, 8.0/32.0},
                {2, 0, 4.0/32.0},
                {-2, 1, 2.0/32.0},
                {-1, 1, 4.0/32.0},
                {0, 1, 8.0/32.0},
                {1, 1, 4.0/32.0},
                {2, 1, 2.0/32.0}
            };
            break;
            
        case STUCKI:
            // Stucki kernel:
            //        *  8/42  4/42
            // 2/42 4/42 8/42  4/42  2/42
            // 1/42 2/42 4/42  2/42  1/42
            kernel = {
                {1, 0, 8.0/42.0},
                {2, 0, 4.0/42.0},
                {-2, 1, 2.0/42.0},
                {-1, 1, 4.0/42.0},
                {0, 1, 8.0/42.0},
                {1, 1, 4.0/42.0},
                {2, 1, 2.0/42.0},
                {-2, 2, 1.0/42.0},
                {-1, 2, 2.0/42.0},
                {0, 2, 4.0/42.0},
                {1, 2, 2.0/42.0},
                {2, 2, 1.0/42.0}
            };
            break;
            
        case SIERRA:
            // Sierra kernel:
            //        *  5/32  3/32
            // 2/32 4/32 5/32  4/32  2/32
            //       2/32 3/32  2/32
            kernel = {
                {1, 0, 5.0/32.0},
                {2, 0, 3.0/32.0},
                {-2, 1, 2.0/32.0},
                {-1, 1, 4.0/32.0},
                {0, 1, 5.0/32.0},
                {1, 1, 4.0/32.0},
                {2, 1, 2.0/32.0},
                {-1, 2, 2.0/32.0},
                {0, 2, 3.0/32.0},
                {1, 2, 2.0/32.0}
            };
            break;
            
        case ATKINSON:
            // Atkinson kernel:
            //      *  1/8  1/8
            // 1/8 1/8 1/8
            //      1/8
            kernel = {
                {1, 0, 1.0/8.0},
                {2, 0, 1.0/8.0},
                {-1, 1, 1.0/8.0},
                {0, 1, 1.0/8.0},
                {1, 1, 1.0/8.0},
                {0, 2, 1.0/8.0}
            };
            break;
    }
    
    return kernel;
} 