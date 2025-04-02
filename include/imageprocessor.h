#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QImage>
#include <QVector>
#include <QString>
#include <QMap>
#include <functional>
#include "filters/functionfilters.h" // Include to access DitheringFilter::KernelType

// Forward declarations
class FunctionFilter;
class ConvolutionFilter;

class ImageProcessor
{
public:
    ImageProcessor();
    ~ImageProcessor();

    // Function filters
    QImage applyInversion(const QImage &image);
    QImage applyBrightnessCorrection(const QImage &image, double factor);
    QImage applyContrastEnhancement(const QImage &image, double factor);
    QImage applyGammaCorrection(const QImage &image, double gamma);
    QImage applyGrayscale(const QImage &image);
    QImage applyUniformQuantization(const QImage &image, int rLevels, int gLevels, int bLevels);
    QImage applyDithering(const QImage &image, int rLevels, int gLevels, int bLevels, DitheringFilter::KernelType kernelType);
    
    // Get dithering kernel names
    QStringList getDitheringKernelNames() const;

    // Convolution filters
    QImage applyConvolutionFilter(const QImage &image, 
                                 const QVector<QVector<double>> &kernel,
                                 double divisor = 1.0,
                                 double offset = 0.0,
                                 int anchorX = -1,
                                 int anchorY = -1);

    // Predefined convolution filters
    QImage applyBlur(const QImage &image);
    QImage applyGaussianBlur(const QImage &image);
    QImage applySharpen(const QImage &image);
    QImage applyEdgeDetection(const QImage &image);
    QImage applyEmboss(const QImage &image);
    
    // Median filter
    QImage applyMedianFilter(const QImage &image, int size = 3);

    // Get predefined kernels
    QVector<QVector<double>> getBlurKernel() const;
    QVector<QVector<double>> getGaussianBlurKernel() const;
    QVector<QVector<double>> getSharpenKernel() const;
    QVector<QVector<double>> getEdgeDetectionKernel() const;
    QVector<QVector<double>> getEmbossKernel() const;

    // Get predefined filter parameters
    double getKernelDivisor(const QVector<QVector<double>> &kernel) const;
    double getKernelOffset(const QString &filterName) const;

    // Save and load custom filters
    bool saveCustomFilter(const QString &name, 
                         const QVector<QVector<double>> &kernel,
                         double divisor,
                         double offset);
    bool loadCustomFilter(const QString &name, 
                         QVector<QVector<double>> &kernel,
                         double &divisor,
                         double &offset);
    QStringList getCustomFilterNames() const;

    // HSV conversion methods
    QImage convertToHSV(const QImage &image);
    QImage convertToRGB(const QImage &hsvImage);
    QImage getHueChannel(const QImage &hsvImage);
    QImage getSaturationChannel(const QImage &hsvImage);
    QImage getValueChannel(const QImage &hsvImage);

private:
    // Helper methods
    QRgb applyFunctionToPixel(QRgb pixel, std::function<int(int)> func);
    QRgb applyConvolutionToPixel(const QImage &image, int x, int y, 
                                const QVector<QVector<double>> &kernel,
                                double divisor, double offset,
                                int anchorX, int anchorY);
    
    // Boundary handling for convolution
    QRgb getPixelWithBoundary(const QImage &image, int x, int y);
    
    // Storage for custom filters
    QMap<QString, QVector<QVector<double>>> customKernels;
    QMap<QString, double> customDivisors;
    QMap<QString, double> customOffsets;
};

#endif // IMAGEPROCESSOR_H 