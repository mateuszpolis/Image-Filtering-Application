#ifndef FUNCTIONFILTERS_H
#define FUNCTIONFILTERS_H

#include <QImage>
#include <QString>
#include <functional>

// Base class for all function filters
class FunctionFilter
{
public:
    FunctionFilter(const QString &name);
    virtual ~FunctionFilter();
    
    QString getName() const;
    virtual QImage apply(const QImage &image) = 0;
    
protected:
    QString name;
    QRgb applyToPixel(QRgb pixel, std::function<int(int)> func);
};

// Inversion filter
class InversionFilter : public FunctionFilter
{
public:
    InversionFilter();
    QImage apply(const QImage &image) override;
};

// Brightness correction filter
class BrightnessFilter : public FunctionFilter
{
public:
    BrightnessFilter(double factor = 50.0);
    QImage apply(const QImage &image) override;
    void setFactor(double factor);
    double getFactor() const;
    
private:
    double factor; // Range: -255 to 255
};

// Contrast enhancement filter
class ContrastFilter : public FunctionFilter
{
public:
    ContrastFilter(double factor = 1.0);
    QImage apply(const QImage &image) override;
    void setFactor(double factor);
    double getFactor() const;
    
private:
    double factor; // Range: 0.0 to 3.0
};

// Gamma correction filter
class GammaFilter : public FunctionFilter
{
public:
    GammaFilter(double gamma = 1.0);
    QImage apply(const QImage &image) override;
    void setGamma(double gamma);
    double getGamma() const;
    
private:
    double gamma; // Range: 0.1 to 10.0
};

// Grayscale filter
class GrayscaleFilter : public FunctionFilter
{
public:
    GrayscaleFilter();
    QImage apply(const QImage &image) override;
};

// Uniform Quantization filter
class UniformQuantizationFilter : public FunctionFilter
{
public:
    UniformQuantizationFilter(int rLevels = 8, int gLevels = 8, int bLevels = 8);
    QImage apply(const QImage &image) override;
    void setLevels(int rLevels, int gLevels, int bLevels);
    int getRedLevels() const;
    int getGreenLevels() const;
    int getBlueLevels() const;
    
private:
    int rLevels; // Number of levels for red channel
    int gLevels; // Number of levels for green channel
    int bLevels; // Number of levels for blue channel
};

#endif // FUNCTIONFILTERS_H 