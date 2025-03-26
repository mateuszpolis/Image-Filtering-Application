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