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