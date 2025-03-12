#include "filters/convolutionfilters.h"
#include <QColor>
#include <cmath>

// Base ConvolutionFilter implementation
ConvolutionFilter::ConvolutionFilter(const QString &name, 
                                   const QVector<QVector<double>> &kernel,
                                   double divisor,
                                   double offset)
    : name(name), kernel(kernel), divisor(divisor), offset(offset)
{
    // Set default anchor to center of kernel
    anchorX = kernel.isEmpty() ? 0 : kernel[0].size() / 2;
    anchorY = kernel.size() / 2;
}

ConvolutionFilter::~ConvolutionFilter() {}

QString ConvolutionFilter::getName() const {
    return name;
}

QVector<QVector<double>> ConvolutionFilter::getKernel() const {
    return kernel;
}

void ConvolutionFilter::setKernel(const QVector<QVector<double>> &kernel) {
    this->kernel = kernel;
    // Update anchor point
    anchorX = kernel.isEmpty() ? 0 : kernel[0].size() / 2;
    anchorY = kernel.size() / 2;
}

double ConvolutionFilter::getDivisor() const {
    return divisor;
}

void ConvolutionFilter::setDivisor(double divisor) {
    this->divisor = divisor;
}

double ConvolutionFilter::getOffset() const {
    return offset;
}

void ConvolutionFilter::setOffset(double offset) {
    this->offset = offset;
}

int ConvolutionFilter::getAnchorX() const {
    return anchorX;
}

void ConvolutionFilter::setAnchorX(int x) {
    anchorX = x;
}

int ConvolutionFilter::getAnchorY() const {
    return anchorY;
}

void ConvolutionFilter::setAnchorY(int y) {
    anchorY = y;
}

double ConvolutionFilter::calculateKernelSum() const {
    double sum = 0.0;
    for (const auto &row : kernel) {
        for (double value : row) {
            sum += value;
        }
    }
    return sum;
}

QImage ConvolutionFilter::apply(const QImage &image) {
    QImage result = image.copy();
    
    for (int y = 0; y < result.height(); ++y) {
        for (int x = 0; x < result.width(); ++x) {
            result.setPixel(x, y, applyToPixel(image, x, y));
        }
    }
    
    return result;
}

QRgb ConvolutionFilter::applyToPixel(const QImage &image, int x, int y) {
    double sumR = 0.0, sumG = 0.0, sumB = 0.0;
    
    for (int ky = 0; ky < kernel.size(); ++ky) {
        for (int kx = 0; kx < kernel[ky].size(); ++kx) {
            int imageX = x + kx - anchorX;
            int imageY = y + ky - anchorY;
            
            QRgb pixel = getPixelWithBoundary(image, imageX, imageY);
            
            sumR += qRed(pixel) * kernel[ky][kx];
            sumG += qGreen(pixel) * kernel[ky][kx];
            sumB += qBlue(pixel) * kernel[ky][kx];
        }
    }
    
    int r = qBound(0, static_cast<int>(sumR / divisor + offset), 255);
    int g = qBound(0, static_cast<int>(sumG / divisor + offset), 255);
    int b = qBound(0, static_cast<int>(sumB / divisor + offset), 255);
    
    return qRgba(r, g, b, qAlpha(image.pixel(x, y)));
}

QRgb ConvolutionFilter::getPixelWithBoundary(const QImage &image, int x, int y) {
    // Handle boundary conditions (mirror at edges)
    if (x < 0) x = -x;
    if (y < 0) y = -y;
    if (x >= image.width()) x = 2 * image.width() - x - 1;
    if (y >= image.height()) y = 2 * image.height() - y - 1;
    
    return image.pixel(x, y);
}

// BlurFilter implementation
BlurFilter::BlurFilter() 
    : ConvolutionFilter("Blur", {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}
    }, 9.0, 0.0) {}

// GaussianBlurFilter implementation
GaussianBlurFilter::GaussianBlurFilter() 
    : ConvolutionFilter("Gaussian Blur", {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    }, 16.0, 0.0) {}

// SharpenFilter implementation
SharpenFilter::SharpenFilter() 
    : ConvolutionFilter("Sharpen", {
        {0, -1, 0},
        {-1, 5, -1},
        {0, -1, 0}
    }, 1.0, 0.0) {}

// EdgeDetectionFilter implementation
EdgeDetectionFilter::EdgeDetectionFilter() 
    : ConvolutionFilter("Edge Detection", {
        {-1, -1, -1},
        {-1, 8, -1},
        {-1, -1, -1}
    }, 1.0, 0.0) {}

// EmbossFilter implementation
EmbossFilter::EmbossFilter() 
    : ConvolutionFilter("Emboss", {
        {-2, -1, 0},
        {-1, 1, 1},
        {0, 1, 2}
    }, 1.0, 128.0) {}

// CustomFilter implementation
CustomFilter::CustomFilter(const QString &name, 
                         const QVector<QVector<double>> &kernel,
                         double divisor,
                         double offset)
    : ConvolutionFilter(name, kernel, divisor, offset) {} 