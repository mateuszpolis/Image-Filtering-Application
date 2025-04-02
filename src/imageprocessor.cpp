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

QImage ImageProcessor::applyUniformQuantization(const QImage &image, int rLevels, int gLevels, int bLevels) {
    UniformQuantizationFilter filter(rLevels, gLevels, bLevels);
    return filter.apply(image);
}

QImage ImageProcessor::applyDithering(const QImage &image, int rLevels, int gLevels, int bLevels, DitheringFilter::KernelType kernelType) {
    DitheringFilter filter(rLevels, gLevels, bLevels, kernelType);
    return filter.apply(image);
}

QStringList ImageProcessor::getDitheringKernelNames() const {
    return DitheringFilter::getKernelNames();
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

QImage ImageProcessor::convertToHSV(const QImage &image)
{
    QImage hsvImage(image.size(), QImage::Format_RGB32);
    
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            int r = qRed(pixel);
            int g = qGreen(pixel);
            int b = qBlue(pixel);
            
            // Convert RGB to HSV
            double r_norm = r / 255.0;
            double g_norm = g / 255.0;
            double b_norm = b / 255.0;
            
            double max = qMax(qMax(r_norm, g_norm), b_norm);
            double min = qMin(qMin(r_norm, g_norm), b_norm);
            double delta = max - min;
            
            double h = 0.0;
            double s = (max == 0) ? 0 : delta / max;
            double v = max;
            
            if (delta != 0) {
                if (max == r_norm) {
                    h = 60 * fmod(((g_norm - b_norm) / delta), 6);
                } else if (max == g_norm) {
                    h = 60 * (((b_norm - r_norm) / delta) + 2);
                } else {
                    h = 60 * (((r_norm - g_norm) / delta) + 4);
                }
            }
            
            if (h < 0) h += 360;
            
            // Store HSV values in RGB channels (H in R, S in G, V in B)
            hsvImage.setPixel(x, y, qRgb(
                static_cast<int>(h * 255.0 / 360.0),
                static_cast<int>(s * 255.0),
                static_cast<int>(v * 255.0)
            ));
        }
    }
    
    return hsvImage;
}

QImage ImageProcessor::convertToRGB(const QImage &hsvImage)
{
    QImage rgbImage(hsvImage.size(), QImage::Format_RGB32);
    
    for (int y = 0; y < hsvImage.height(); ++y) {
        for (int x = 0; x < hsvImage.width(); ++x) {
            QRgb pixel = hsvImage.pixel(x, y);
            double h = qRed(pixel) * 360.0 / 255.0;
            double s = qGreen(pixel) / 255.0;
            double v = qBlue(pixel) / 255.0;
            
            // Convert HSV to RGB
            double c = v * s;
            double x_val = c * (1 - std::abs(fmod(h / 60.0, 2) - 1));
            double m = v - c;
            double r = 0, g = 0, b = 0;
            
            if (h < 60) {
                r = c; g = x_val; b = 0;
            } else if (h < 120) {
                r = x_val; g = c; b = 0;
            } else if (h < 180) {
                r = 0; g = c; b = x_val;
            } else if (h < 240) {
                r = 0; g = x_val; b = c;
            } else if (h < 300) {
                r = x_val; g = 0; b = c;
            } else {
                r = c; g = 0; b = x_val;
            }
            
            rgbImage.setPixel(x, y, qRgb(
                static_cast<int>((r + m) * 255),
                static_cast<int>((g + m) * 255),
                static_cast<int>((b + m) * 255)
            ));
        }
    }
    
    return rgbImage;
}

QImage ImageProcessor::getHueChannel(const QImage &hsvImage)
{
    QImage hueImage(hsvImage.size(), QImage::Format_Grayscale8);
    
    for (int y = 0; y < hsvImage.height(); ++y) {
        for (int x = 0; x < hsvImage.width(); ++x) {
            QRgb pixel = hsvImage.pixel(x, y);
            hueImage.setPixel(x, y, qRgb(qRed(pixel), qRed(pixel), qRed(pixel)));
        }
    }
    
    return hueImage;
}

QImage ImageProcessor::getSaturationChannel(const QImage &hsvImage)
{
    QImage saturationImage(hsvImage.size(), QImage::Format_Grayscale8);
    
    for (int y = 0; y < hsvImage.height(); ++y) {
        for (int x = 0; x < hsvImage.width(); ++x) {
            QRgb pixel = hsvImage.pixel(x, y);
            saturationImage.setPixel(x, y, qRgb(qGreen(pixel), qGreen(pixel), qGreen(pixel)));
        }
    }
    
    return saturationImage;
}

QImage ImageProcessor::getValueChannel(const QImage &hsvImage)
{
    QImage valueImage(hsvImage.size(), QImage::Format_Grayscale8);
    
    for (int y = 0; y < hsvImage.height(); ++y) {
        for (int x = 0; x < hsvImage.width(); ++x) {
            QRgb pixel = hsvImage.pixel(x, y);
            valueImage.setPixel(x, y, qRgb(qBlue(pixel), qBlue(pixel), qBlue(pixel)));
        }
    }
    
    return valueImage;
} 