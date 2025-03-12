#ifndef CONVOLUTIONFILTERS_H
#define CONVOLUTIONFILTERS_H

#include <QImage>
#include <QString>
#include <QVector>

// Base class for all convolution filters
class ConvolutionFilter
{
public:
    ConvolutionFilter(const QString &name, 
                     const QVector<QVector<double>> &kernel,
                     double divisor = 1.0,
                     double offset = 0.0);
    virtual ~ConvolutionFilter();
    
    QString getName() const;
    QVector<QVector<double>> getKernel() const;
    void setKernel(const QVector<QVector<double>> &kernel);
    
    double getDivisor() const;
    void setDivisor(double divisor);
    
    double getOffset() const;
    void setOffset(double offset);
    
    int getAnchorX() const;
    void setAnchorX(int x);
    
    int getAnchorY() const;
    void setAnchorY(int y);
    
    virtual QImage apply(const QImage &image);
    
    // Calculate sum of kernel elements
    double calculateKernelSum() const;
    
protected:
    QString name;
    QVector<QVector<double>> kernel;
    double divisor;
    double offset;
    int anchorX;
    int anchorY;
    
    // Helper methods
    QRgb applyToPixel(const QImage &image, int x, int y);
    QRgb getPixelWithBoundary(const QImage &image, int x, int y);
};

// Blur filter
class BlurFilter : public ConvolutionFilter
{
public:
    BlurFilter();
};

// Gaussian blur filter
class GaussianBlurFilter : public ConvolutionFilter
{
public:
    GaussianBlurFilter();
};

// Sharpen filter
class SharpenFilter : public ConvolutionFilter
{
public:
    SharpenFilter();
};

// Edge detection filter
class EdgeDetectionFilter : public ConvolutionFilter
{
public:
    EdgeDetectionFilter();
};

// Emboss filter
class EmbossFilter : public ConvolutionFilter
{
public:
    EmbossFilter();
};

// Custom filter
class CustomFilter : public ConvolutionFilter
{
public:
    CustomFilter(const QString &name, 
                const QVector<QVector<double>> &kernel,
                double divisor = 1.0,
                double offset = 0.0);
};

#endif // CONVOLUTIONFILTERS_H 