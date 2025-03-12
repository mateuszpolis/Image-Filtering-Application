#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QImage>
#include <QVector>
#include <QString>
#include <QMap>
#include <functional>

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