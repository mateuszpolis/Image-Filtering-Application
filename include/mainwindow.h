#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>
#include <QScrollArea>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QStack>

#include "imageprocessor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openImage();
    void saveImage();
    void resetImage();
    void applyFilter();
    void undoFilter();
    void updateKernelSize();
    void updateKernelTable();
    void calculateDivisor();
    void updateFilterPreview();
    void loadPredefinedFilter(int index);

private:
    Ui::MainWindow *ui;
    
    // Image display
    QImage originalImage;
    QImage currentImage;
    QStack<QImage> imageHistory;
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    QLabel *originalImageLabel;
    QScrollArea *originalScrollArea;
    
    // Filter selection
    QComboBox *filterTypeComboBox;
    QComboBox *filterSelectionComboBox;
    
    // Function filter parameters
    QGroupBox *functionParamsGroup;
    QDoubleSpinBox *brightnessSpinBox;
    QDoubleSpinBox *contrastSpinBox;
    QDoubleSpinBox *gammaSpinBox;
    
    // Uniform quantization parameters
    QGroupBox *quantizationParamsGroup;
    QSpinBox *redLevelsSpinBox;
    QSpinBox *greenLevelsSpinBox;
    QSpinBox *blueLevelsSpinBox;
    
    // Dithering parameters
    QGroupBox *ditheringParamsGroup;
    QSpinBox *ditherRedLevelsSpinBox;
    QSpinBox *ditherGreenLevelsSpinBox;
    QSpinBox *ditherBlueLevelsSpinBox;
    QComboBox *kernelTypeComboBox;
    
    // Convolution filter parameters
    QGroupBox *convolutionParamsGroup;
    QSpinBox *kernelRowsSpinBox;
    QSpinBox *kernelColsSpinBox;
    QTableWidget *kernelTable;
    QDoubleSpinBox *divisorSpinBox;
    QCheckBox *autoDivisorCheckBox;
    QDoubleSpinBox *offsetSpinBox;
    QSpinBox *anchorXSpinBox;
    QSpinBox *anchorYSpinBox;
    
    // Median filter parameters
    QGroupBox *medianParamsGroup;
    QSpinBox *medianSizeSpinBox;
    
    // Buttons
    QPushButton *applyButton;
    QPushButton *undoButton;
    QPushButton *resetButton;
    QPushButton *saveButton;
    QPushButton *loadFilterButton;
    QPushButton *saveFilterButton;
    
    // Image processor
    ImageProcessor processor;
    
    // Layouts
    QHBoxLayout *mainLayout;
    
    // HSV conversion UI elements
    QGroupBox *hsvGroup;
    QPushButton *convertToHSVButton;
    QLabel *hueChannelLabel;
    QLabel *saturationChannelLabel;
    QLabel *valueChannelLabel;
    QScrollArea *hueScrollArea;
    QScrollArea *saturationScrollArea;
    QScrollArea *valueScrollArea;
    QLabel *convertedRGBLabel;
    QScrollArea *convertedRGBScrollArea;
    
    void setupUI();
    void setupMenus();
    void setupConnections();
    void updateImage(const QImage &image);
    void displayImage(const QImage &image);
    void displayOriginalImage(const QImage &image);
    void enableFilterControls(bool enable);
    void setupFunctionFilterControls();
    void setupConvolutionFilterControls();
    void setupMedianFilterControls();
    void setupQuantizationFilterControls();
    void setupDitheringFilterControls();
    void switchFilterType(int index);
    void setupHSVControls();
    void convertToHSV();
};

#endif // MAINWINDOW_H 