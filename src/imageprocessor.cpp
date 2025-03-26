#include "imageprocessor.h"
#include "filters/functionfilters.h"
#include "filters/convolutionfilters.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

ImageProcessor::ImageProcessor() {
    // Create directory for custom filters if it doesn't exist
    QDir dir;
    if (!dir.exists("filters")) {
        dir.mkdir("filters");
    }
}

ImageProcessor::~ImageProcessor() {}

// Function filters
QImage ImageProcessor::applyInversion(const QImage &image) {
    InversionFilter filter;
    return filter.apply(image);
}

QImage ImageProcessor::applyBrightnessCorrection(const QImage &image, double factor) {
    BrightnessFilter filter(factor);
    return filter.apply(image);
}

QImage ImageProcessor::applyContrastEnhancement(const QImage &image, double factor) {
    ContrastFilter filter(factor);
    return filter.apply(image);
}

QImage ImageProcessor::applyGammaCorrection(const QImage &image, double gamma) {
    GammaFilter filter(gamma);
    return filter.apply(image);
}

QImage ImageProcessor::applyGrayscale(const QImage &image) {
    GrayscaleFilter filter;
    return filter.apply(image);
}

// Convolution filters
QImage ImageProcessor::applyConvolutionFilter(const QImage &image, 
                                            const QVector<QVector<double>> &kernel,
                                            double divisor,
                                            double offset,
                                            int anchorX,
                                            int anchorY) {
    CustomFilter filter("Custom", kernel, divisor, offset);
    
    if (anchorX >= 0) {
        filter.setAnchorX(anchorX);
    }
    
    if (anchorY >= 0) {
        filter.setAnchorY(anchorY);
    }
    
    return filter.apply(image);
}

QImage ImageProcessor::applyBlur(const QImage &image) {
    BlurFilter filter;
    return filter.apply(image);
}

QImage ImageProcessor::applyGaussianBlur(const QImage &image) {
    GaussianBlurFilter filter;
    return filter.apply(image);
}

QImage ImageProcessor::applySharpen(const QImage &image) {
    SharpenFilter filter;
    return filter.apply(image);
}

QImage ImageProcessor::applyEdgeDetection(const QImage &image) {
    EdgeDetectionFilter filter;
    return filter.apply(image);
}

QImage ImageProcessor::applyEmboss(const QImage &image) {
    EmbossFilter filter;
    return filter.apply(image);
}

// Median filter
QImage ImageProcessor::applyMedianFilter(const QImage &image, int size) {
    MedianFilter filter(size);
    return filter.apply(image);
}

// Get predefined kernels
QVector<QVector<double>> ImageProcessor::getBlurKernel() const {
    BlurFilter filter;
    return filter.getKernel();
}

QVector<QVector<double>> ImageProcessor::getGaussianBlurKernel() const {
    GaussianBlurFilter filter;
    return filter.getKernel();
}

QVector<QVector<double>> ImageProcessor::getSharpenKernel() const {
    SharpenFilter filter;
    return filter.getKernel();
}

QVector<QVector<double>> ImageProcessor::getEdgeDetectionKernel() const {
    EdgeDetectionFilter filter;
    return filter.getKernel();
}

QVector<QVector<double>> ImageProcessor::getEmbossKernel() const {
    EmbossFilter filter;
    return filter.getKernel();
}

// Get predefined filter parameters
double ImageProcessor::getKernelDivisor(const QVector<QVector<double>> &kernel) const {
    double sum = 0.0;
    for (const auto &row : kernel) {
        for (double value : row) {
            sum += value;
        }
    }
    
    // If sum is zero or very close to zero, return 1.0 to avoid division by zero
    return (std::abs(sum) < 0.00001) ? 1.0 : sum;
}

double ImageProcessor::getKernelOffset(const QString &filterName) const {
    if (filterName == "Emboss") {
        return 128.0;
    }
    return 0.0;
}

// Helper methods
QRgb ImageProcessor::applyFunctionToPixel(QRgb pixel, std::function<int(int)> func) {
    int r = qRed(pixel);
    int g = qGreen(pixel);
    int b = qBlue(pixel);
    
    r = qBound(0, func(r), 255);
    g = qBound(0, func(g), 255);
    b = qBound(0, func(b), 255);
    
    return qRgba(r, g, b, qAlpha(pixel));
}

QRgb ImageProcessor::applyConvolutionToPixel(const QImage &image, int x, int y, 
                                           const QVector<QVector<double>> &kernel,
                                           double divisor, double offset,
                                           int anchorX, int anchorY) {
    double sumR = 0.0, sumG = 0.0, sumB = 0.0;
    
    if (anchorX < 0) {
        anchorX = kernel[0].size() / 2;
    }
    
    if (anchorY < 0) {
        anchorY = kernel.size() / 2;
    }
    
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

QRgb ImageProcessor::getPixelWithBoundary(const QImage &image, int x, int y) {
    // Handle boundary conditions (mirror at edges)
    if (x < 0) x = -x;
    if (y < 0) y = -y;
    if (x >= image.width()) x = 2 * image.width() - x - 1;
    if (y >= image.height()) y = 2 * image.height() - y - 1;
    
    return image.pixel(x, y);
}

// Save and load custom filters
bool ImageProcessor::saveCustomFilter(const QString &name, 
                                    const QVector<QVector<double>> &kernel,
                                    double divisor,
                                    double offset) {
    QJsonObject filterObj;
    filterObj["name"] = name;
    filterObj["divisor"] = divisor;
    filterObj["offset"] = offset;
    
    QJsonArray kernelArray;
    for (const auto &row : kernel) {
        QJsonArray rowArray;
        for (double value : row) {
            rowArray.append(value);
        }
        kernelArray.append(rowArray);
    }
    
    filterObj["kernel"] = kernelArray;
    
    QJsonDocument doc(filterObj);
    QFile file("filters/" + name + ".json");
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson());
    
    // Update internal storage
    customKernels[name] = kernel;
    customDivisors[name] = divisor;
    customOffsets[name] = offset;
    
    return true;
}

bool ImageProcessor::loadCustomFilter(const QString &name, 
                                    QVector<QVector<double>> &kernel,
                                    double &divisor,
                                    double &offset) {
    // Check if we already have it in memory
    if (customKernels.contains(name)) {
        kernel = customKernels[name];
        divisor = customDivisors[name];
        offset = customOffsets[name];
        return true;
    }
    
    // Otherwise load from file
    QFile file("filters/" + name + ".json");
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    
    if (doc.isNull() || !doc.isObject()) {
        return false;
    }
    
    QJsonObject filterObj = doc.object();
    
    divisor = filterObj["divisor"].toDouble();
    offset = filterObj["offset"].toDouble();
    
    QJsonArray kernelArray = filterObj["kernel"].toArray();
    kernel.clear();
    
    for (int i = 0; i < kernelArray.size(); ++i) {
        QJsonArray rowArray = kernelArray[i].toArray();
        QVector<double> row;
        
        for (int j = 0; j < rowArray.size(); ++j) {
            row.append(rowArray[j].toDouble());
        }
        
        kernel.append(row);
    }
    
    // Update internal storage
    customKernels[name] = kernel;
    customDivisors[name] = divisor;
    customOffsets[name] = offset;
    
    return true;
}

QStringList ImageProcessor::getCustomFilterNames() const {
    QDir dir("filters");
    QStringList filters;
    filters << "*.json";
    QStringList fileNames = dir.entryList(filters, QDir::Files);
    
    QStringList result;
    for (const QString &fileName : fileNames) {
        result.append(fileName.left(fileName.length() - 5)); // Remove .json extension
    }
    
    return result;
} 