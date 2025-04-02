#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QImageReader>
#include <QImageWriter>
#include <QStandardPaths>
#include <QInputDialog>
#include <QDoubleValidator>
#include <QLabel>
#include <QScrollArea>
#include <QSplitter>
#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    setupMenus();
    setupConnections();
    
    statusBar()->showMessage(tr("Ready"));
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Main layout
    mainLayout = new QHBoxLayout(centralWidget);
    
    // Left panel for controls
    QWidget *controlPanel = new QWidget(this);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlPanel);
    controlPanel->setMinimumWidth(300);
    controlPanel->setMaximumWidth(400);
    
    // Filter type selection
    QGroupBox *filterTypeGroup = new QGroupBox("Filter Type", this);
    QVBoxLayout *filterTypeLayout = new QVBoxLayout(filterTypeGroup);
    
    filterTypeComboBox = new QComboBox(this);
    filterTypeComboBox->addItem("Function Filters");
    filterTypeComboBox->addItem("Convolution Filters");
    filterTypeComboBox->addItem("Median Filter");
    filterTypeLayout->addWidget(filterTypeComboBox);
    
    // Filter selection
    QGroupBox *filterSelectionGroup = new QGroupBox("Filter Selection", this);
    QVBoxLayout *filterSelectionLayout = new QVBoxLayout(filterSelectionGroup);
    
    filterSelectionComboBox = new QComboBox(this);
    filterSelectionLayout->addWidget(filterSelectionComboBox);
    
    // Function filter parameters
    functionParamsGroup = new QGroupBox("Function Filter Parameters", this);
    QFormLayout *functionParamsLayout = new QFormLayout(functionParamsGroup);
    
    brightnessSpinBox = new QDoubleSpinBox(this);
    brightnessSpinBox->setRange(-255.0, 255.0);
    brightnessSpinBox->setValue(0.0);
    brightnessSpinBox->setSingleStep(5.0);
    functionParamsLayout->addRow("Brightness:", brightnessSpinBox);
    
    contrastSpinBox = new QDoubleSpinBox(this);
    contrastSpinBox->setRange(0.0, 3.0);
    contrastSpinBox->setValue(1.0);
    contrastSpinBox->setSingleStep(0.1);
    functionParamsLayout->addRow("Contrast:", contrastSpinBox);
    
    gammaSpinBox = new QDoubleSpinBox(this);
    gammaSpinBox->setRange(0.1, 10.0);
    gammaSpinBox->setValue(1.0);
    gammaSpinBox->setSingleStep(0.1);
    functionParamsLayout->addRow("Gamma:", gammaSpinBox);
    
    // Uniform quantization parameters
    quantizationParamsGroup = new QGroupBox("Uniform Quantization Parameters", this);
    QFormLayout *quantizationParamsLayout = new QFormLayout(quantizationParamsGroup);
    
    redLevelsSpinBox = new QSpinBox(this);
    redLevelsSpinBox->setRange(2, 256);
    redLevelsSpinBox->setValue(8);
    redLevelsSpinBox->setSingleStep(2);
    quantizationParamsLayout->addRow("Red Levels:", redLevelsSpinBox);
    
    greenLevelsSpinBox = new QSpinBox(this);
    greenLevelsSpinBox->setRange(2, 256);
    greenLevelsSpinBox->setValue(8);
    greenLevelsSpinBox->setSingleStep(2);
    quantizationParamsLayout->addRow("Green Levels:", greenLevelsSpinBox);
    
    blueLevelsSpinBox = new QSpinBox(this);
    blueLevelsSpinBox->setRange(2, 256);
    blueLevelsSpinBox->setValue(8);
    blueLevelsSpinBox->setSingleStep(2);
    quantizationParamsLayout->addRow("Blue Levels:", blueLevelsSpinBox);
    
    // Dithering parameters
    ditheringParamsGroup = new QGroupBox("Dithering Parameters", this);
    QFormLayout *ditheringParamsLayout = new QFormLayout(ditheringParamsGroup);
    
    ditherRedLevelsSpinBox = new QSpinBox(this);
    ditherRedLevelsSpinBox->setRange(2, 256);
    ditherRedLevelsSpinBox->setValue(2);
    ditherRedLevelsSpinBox->setSingleStep(1);
    ditheringParamsLayout->addRow("Red Levels:", ditherRedLevelsSpinBox);
    
    ditherGreenLevelsSpinBox = new QSpinBox(this);
    ditherGreenLevelsSpinBox->setRange(2, 256);
    ditherGreenLevelsSpinBox->setValue(2);
    ditherGreenLevelsSpinBox->setSingleStep(1);
    ditheringParamsLayout->addRow("Green Levels:", ditherGreenLevelsSpinBox);
    
    ditherBlueLevelsSpinBox = new QSpinBox(this);
    ditherBlueLevelsSpinBox->setRange(2, 256);
    ditherBlueLevelsSpinBox->setValue(2);
    ditherBlueLevelsSpinBox->setSingleStep(1);
    ditheringParamsLayout->addRow("Blue Levels:", ditherBlueLevelsSpinBox);
    
    kernelTypeComboBox = new QComboBox(this);
    kernelTypeComboBox->addItems(processor.getDitheringKernelNames());
    ditheringParamsLayout->addRow("Kernel Type:", kernelTypeComboBox);
    
    // Convolution filter parameters
    convolutionParamsGroup = new QGroupBox("Convolution Filter Parameters", this);
    QVBoxLayout *convolutionParamsLayout = new QVBoxLayout(convolutionParamsGroup);
    
    QHBoxLayout *kernelSizeLayout = new QHBoxLayout();
    QLabel *rowsLabel = new QLabel("Rows:", this);
    kernelRowsSpinBox = new QSpinBox(this);
    kernelRowsSpinBox->setRange(1, 9);
    kernelRowsSpinBox->setValue(3);
    kernelRowsSpinBox->setSingleStep(2);
    
    QLabel *colsLabel = new QLabel("Columns:", this);
    kernelColsSpinBox = new QSpinBox(this);
    kernelColsSpinBox->setRange(1, 9);
    kernelColsSpinBox->setValue(3);
    kernelColsSpinBox->setSingleStep(2);
    
    kernelSizeLayout->addWidget(rowsLabel);
    kernelSizeLayout->addWidget(kernelRowsSpinBox);
    kernelSizeLayout->addWidget(colsLabel);
    kernelSizeLayout->addWidget(kernelColsSpinBox);
    convolutionParamsLayout->addLayout(kernelSizeLayout);
    
    // Kernel table
    kernelTable = new QTableWidget(3, 3, this);
    kernelTable->horizontalHeader()->setVisible(false);
    kernelTable->verticalHeader()->setVisible(false);
    kernelTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    kernelTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // Initialize kernel table with zeros
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            QTableWidgetItem *item = new QTableWidgetItem("0");
            kernelTable->setItem(i, j, item);
        }
    }
    
    convolutionParamsLayout->addWidget(kernelTable);
    
    // Divisor and offset
    QFormLayout *filterParamsLayout = new QFormLayout();
    
    divisorSpinBox = new QDoubleSpinBox(this);
    divisorSpinBox->setRange(-1000.0, 1000.0);
    divisorSpinBox->setValue(1.0);
    divisorSpinBox->setSingleStep(1.0);
    filterParamsLayout->addRow("Divisor:", divisorSpinBox);
    
    autoDivisorCheckBox = new QCheckBox("Auto Calculate", this);
    autoDivisorCheckBox->setChecked(true);
    filterParamsLayout->addRow("", autoDivisorCheckBox);
    
    offsetSpinBox = new QDoubleSpinBox(this);
    offsetSpinBox->setRange(-255.0, 255.0);
    offsetSpinBox->setValue(0.0);
    offsetSpinBox->setSingleStep(1.0);
    filterParamsLayout->addRow("Offset:", offsetSpinBox);
    
    // Anchor point
    QHBoxLayout *anchorLayout = new QHBoxLayout();
    QLabel *anchorLabel = new QLabel("Anchor Point:", this);
    anchorXSpinBox = new QSpinBox(this);
    anchorXSpinBox->setRange(0, 8);
    anchorXSpinBox->setValue(1);
    
    anchorYSpinBox = new QSpinBox(this);
    anchorYSpinBox->setRange(0, 8);
    anchorYSpinBox->setValue(1);
    
    anchorLayout->addWidget(anchorLabel);
    anchorLayout->addWidget(anchorXSpinBox);
    anchorLayout->addWidget(anchorYSpinBox);
    filterParamsLayout->addRow("", new QWidget()); // Spacer
    filterParamsLayout->addRow("", anchorLayout);
    
    convolutionParamsLayout->addLayout(filterParamsLayout);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    loadFilterButton = new QPushButton("Load Filter", this);
    saveFilterButton = new QPushButton("Save Filter", this);
    
    buttonLayout->addWidget(loadFilterButton);
    buttonLayout->addWidget(saveFilterButton);
    
    convolutionParamsLayout->addLayout(buttonLayout);
    
    // Action buttons
    QHBoxLayout *actionButtonLayout = new QHBoxLayout();
    
    applyButton = new QPushButton("Apply Filter", this);
    undoButton = new QPushButton("Undo", this);
    resetButton = new QPushButton("Reset", this);
    saveButton = new QPushButton("Save Image", this);
    
    actionButtonLayout->addWidget(applyButton);
    actionButtonLayout->addWidget(undoButton);
    actionButtonLayout->addWidget(resetButton);
    actionButtonLayout->addWidget(saveButton);
    
    // Median filter parameters
    medianParamsGroup = new QGroupBox("Median Filter Parameters", this);
    QVBoxLayout *medianParamsLayout = new QVBoxLayout(medianParamsGroup);
    medianParamsLayout->setContentsMargins(5, 5, 5, 5);
    
    QHBoxLayout *medianSizeLayout = new QHBoxLayout();
    QLabel *medianSizeLabel = new QLabel("Size:", this);
    medianSizeSpinBox = new QSpinBox(this);
    medianSizeSpinBox->setRange(3, 99);
    medianSizeSpinBox->setValue(3);
    medianSizeSpinBox->setSingleStep(2);
    medianSizeSpinBox->setToolTip("Size of the median filter (must be odd)");
    
    medianSizeLayout->addWidget(medianSizeLabel);
    medianSizeLayout->addWidget(medianSizeSpinBox);
    medianParamsLayout->addLayout(medianSizeLayout);
    // Add spacer to push controls to the top
    medianParamsLayout->addStretch();
    
    // Add all controls to the control panel
    controlLayout->addWidget(filterTypeGroup);
    controlLayout->addWidget(filterSelectionGroup);
    controlLayout->addWidget(functionParamsGroup);
    controlLayout->addWidget(quantizationParamsGroup);
    controlLayout->addWidget(ditheringParamsGroup);
    controlLayout->addWidget(convolutionParamsGroup);
    controlLayout->addWidget(medianParamsGroup);
    controlLayout->addLayout(actionButtonLayout);
    controlLayout->addStretch();
    
    // Image display area - modified to show two images side by side
    QWidget *imagePanel = new QWidget(this);
    QVBoxLayout *imagePanelLayout = new QVBoxLayout(imagePanel); // Changed to vertical layout
    
    // Main images row
    QHBoxLayout *mainImagesLayout = new QHBoxLayout();
    
    // Original image display
    QVBoxLayout *originalImageLayout = new QVBoxLayout();
    QLabel *originalImageTitle = new QLabel("Original Image", this);
    originalImageTitle->setAlignment(Qt::AlignCenter);
    originalImageTitle->setStyleSheet("font-weight: bold;");
    
    originalImageLabel = new QLabel(this);
    originalImageLabel->setAlignment(Qt::AlignCenter);
    originalImageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    originalImageLabel->setMinimumSize(300, 300);
    originalImageLabel->setText("No image loaded");
    
    originalScrollArea = new QScrollArea(this);
    originalScrollArea->setWidget(originalImageLabel);
    originalScrollArea->setWidgetResizable(true);
    
    originalImageLayout->addWidget(originalImageTitle);
    originalImageLayout->addWidget(originalScrollArea);
    
    // Edited image display
    QVBoxLayout *editedImageLayout = new QVBoxLayout();
    QLabel *editedImageTitle = new QLabel("Edited Image", this);
    editedImageTitle->setAlignment(Qt::AlignCenter);
    editedImageTitle->setStyleSheet("font-weight: bold;");
    
    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageLabel->setMinimumSize(300, 300);
    imageLabel->setText("No image loaded");
    
    scrollArea = new QScrollArea(this);
    scrollArea->setWidget(imageLabel);
    scrollArea->setWidgetResizable(true);
    
    editedImageLayout->addWidget(editedImageTitle);
    editedImageLayout->addWidget(scrollArea);
    
    // Add both image displays to the main images row
    mainImagesLayout->addLayout(originalImageLayout);
    mainImagesLayout->addLayout(editedImageLayout);
    
    // Add main images row to the panel layout
    imagePanelLayout->addLayout(mainImagesLayout);
    
    // Add panels to main layout
    mainLayout->addWidget(controlPanel);
    mainLayout->addWidget(imagePanel, 1);
    
    // Initial state
    enableFilterControls(false);
    switchFilterType(0); // Start with function filters
    
    // Initially hide parameters
    convolutionParamsGroup->hide();
    medianParamsGroup->hide();
    quantizationParamsGroup->hide();
    ditheringParamsGroup->hide();
    
    setupHSVControls();
}

void MainWindow::setupMenus()
{
    // File menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    
    QAction *openAction = fileMenu->addAction(tr("&Open..."), this, &MainWindow::openImage);
    openAction->setShortcut(QKeySequence::Open);
    
    QAction *saveAction = fileMenu->addAction(tr("&Save..."), this, &MainWindow::saveImage);
    saveAction->setShortcut(QKeySequence::Save);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAction->setShortcut(QKeySequence::Quit);
    
    // Edit menu
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    
    QAction *undoAction = editMenu->addAction(tr("&Undo"), this, &MainWindow::undoFilter);
    undoAction->setShortcut(QKeySequence::Undo);
    
    QAction *resetAction = editMenu->addAction(tr("&Reset"), this, &MainWindow::resetImage);
    resetAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    
    // Filter menu
    QMenu *filterMenu = menuBar()->addMenu(tr("&Filters"));
    
    QAction *applyAction = filterMenu->addAction(tr("&Apply Filter"), this, &MainWindow::applyFilter);
    applyAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_A));
    
    // Help menu
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    
    QAction *aboutAction = helpMenu->addAction(tr("&About"), [this]() {
        QMessageBox::about(this, tr("About Image Filtering"),
                          tr("Image Filtering Application\n\n"
                             "A simple application for applying various filters to images.\n"
                             "Supports function filters and convolution filters."));
    });
}

void MainWindow::setupConnections()
{
    // Filter type selection
    connect(filterTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::switchFilterType);
    
    // Filter selection
    connect(filterSelectionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::loadPredefinedFilter);
    
    // Kernel size
    connect(kernelRowsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::updateKernelSize);
    connect(kernelColsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::updateKernelSize);
    
    // Auto divisor
    connect(autoDivisorCheckBox, &QCheckBox::toggled, [this](bool checked) {
        divisorSpinBox->setEnabled(!checked);
        if (checked) {
            calculateDivisor();
        }
    });
    
    // Kernel table changes
    connect(kernelTable, &QTableWidget::itemChanged, this, &MainWindow::updateFilterPreview);
    
    // Buttons
    connect(applyButton, &QPushButton::clicked, this, &MainWindow::applyFilter);
    connect(undoButton, &QPushButton::clicked, this, &MainWindow::undoFilter);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::resetImage);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveImage);
    
    connect(loadFilterButton, &QPushButton::clicked, [this]() {
        QStringList filters = processor.getCustomFilterNames();
        if (filters.isEmpty()) {
            QMessageBox::information(this, tr("No Custom Filters"),
                                    tr("No custom filters found. Create and save a filter first."));
            return;
        }
        
        bool ok;
        QString filterName = QInputDialog::getItem(this, tr("Load Filter"),
                                                 tr("Select a filter:"), filters, 0, false, &ok);
        if (ok && !filterName.isEmpty()) {
            QVector<QVector<double>> kernel;
            double divisor, offset;
            
            if (processor.loadCustomFilter(filterName, kernel, divisor, offset)) {
                // Update UI with loaded filter
                kernelRowsSpinBox->setValue(kernel.size());
                kernelColsSpinBox->setValue(kernel[0].size());
                
                for (int i = 0; i < kernel.size(); ++i) {
                    for (int j = 0; j < kernel[i].size(); ++j) {
                        kernelTable->item(i, j)->setText(QString::number(kernel[i][j]));
                    }
                }
                
                divisorSpinBox->setValue(divisor);
                offsetSpinBox->setValue(offset);
                
                statusBar()->showMessage(tr("Filter '%1' loaded").arg(filterName), 3000);
            } else {
                QMessageBox::warning(this, tr("Error"),
                                    tr("Failed to load filter '%1'").arg(filterName));
            }
        }
    });
    
    connect(saveFilterButton, &QPushButton::clicked, [this]() {
        bool ok;
        QString filterName = QInputDialog::getText(this, tr("Save Filter"),
                                                 tr("Filter name:"), QLineEdit::Normal,
                                                 "", &ok);
        if (ok && !filterName.isEmpty()) {
            QVector<QVector<double>> kernel;
            kernel.resize(kernelTable->rowCount());
            
            for (int i = 0; i < kernelTable->rowCount(); ++i) {
                kernel[i].resize(kernelTable->columnCount());
                for (int j = 0; j < kernelTable->columnCount(); ++j) {
                    kernel[i][j] = kernelTable->item(i, j)->text().toDouble();
                }
            }
            
            double divisor = divisorSpinBox->value();
            double offset = offsetSpinBox->value();
            
            if (processor.saveCustomFilter(filterName, kernel, divisor, offset)) {
                statusBar()->showMessage(tr("Filter '%1' saved").arg(filterName), 3000);
            } else {
                QMessageBox::warning(this, tr("Error"),
                                    tr("Failed to save filter '%1'").arg(filterName));
            }
        }
    });
}

void MainWindow::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                                  QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
                                                  tr("Image Files (*.png *.jpg *.jpeg *.bmp *.gif)"));
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    originalImage = reader.read();
    
    if (originalImage.isNull()) {
        QMessageBox::warning(this, tr("Error"),
                            tr("Cannot load %1: %2")
                            .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return;
    }
    
    // Clear history and update display
    imageHistory.clear();
    currentImage = originalImage;
    
    // Display both original and current images
    displayOriginalImage(originalImage);
    displayImage(currentImage);
    
    // Enable filter controls
    enableFilterControls(true);
    
    statusBar()->showMessage(tr("Image loaded: %1").arg(QFileInfo(fileName).fileName()), 3000);
}

void MainWindow::saveImage()
{
    if (currentImage.isNull()) {
        QMessageBox::warning(this, tr("Error"), tr("No image to save"));
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"),
                                                  QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
                                                  tr("PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;BMP Image (*.bmp)"));
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QImageWriter writer(fileName);
    
    if (!writer.write(currentImage)) {
        QMessageBox::warning(this, tr("Error"),
                            tr("Cannot save %1: %2")
                            .arg(QDir::toNativeSeparators(fileName), writer.errorString()));
        return;
    }
    
    statusBar()->showMessage(tr("Image saved: %1").arg(QFileInfo(fileName).fileName()), 3000);
}

void MainWindow::resetImage()
{
    if (originalImage.isNull()) {
        return;
    }
    
    // Clear history and reset to original
    imageHistory.clear();
    currentImage = originalImage;
    displayImage(currentImage);
    
    statusBar()->showMessage(tr("Image reset to original"), 3000);
}

void MainWindow::applyFilter()
{
    if (currentImage.isNull()) {
        QMessageBox::information(this, tr("No Image"),
                                tr("Please open an image first."));
        return;
    }
    
    // Save current image for undo
    imageHistory.push(currentImage);
    
    // Apply selected filter
    QImage result;
    int filterType = filterTypeComboBox->currentIndex();
    int filterIndex = filterSelectionComboBox->currentIndex();
    
    // Define kernel type outside of the switch to avoid variable initialization bypass error
    DitheringFilter::KernelType kernelType = DitheringFilter::FLOYD_STEINBERG;
    
    if (filterType == 0) { // Function filters
        switch (filterIndex) {
            case 0: // Inversion
                result = processor.applyInversion(currentImage);
                break;
            case 1: // Brightness
                result = processor.applyBrightnessCorrection(currentImage, brightnessSpinBox->value());
                break;
            case 2: // Contrast
                result = processor.applyContrastEnhancement(currentImage, contrastSpinBox->value());
                break;
            case 3: // Gamma
                result = processor.applyGammaCorrection(currentImage, gammaSpinBox->value());
                break;
            case 4: // Grayscale
                result = processor.applyGrayscale(currentImage);
                break;
            case 5: // Uniform Quantization
                result = processor.applyUniformQuantization(currentImage, 
                                                           redLevelsSpinBox->value(),
                                                           greenLevelsSpinBox->value(),
                                                           blueLevelsSpinBox->value());
                break;
            case 6: // Dithering
                // Convert combobox index to kernel type
                kernelType = static_cast<DitheringFilter::KernelType>(kernelTypeComboBox->currentIndex());
                
                result = processor.applyDithering(currentImage,
                                                 ditherRedLevelsSpinBox->value(),
                                                 ditherGreenLevelsSpinBox->value(),
                                                 ditherBlueLevelsSpinBox->value(),
                                                 kernelType);
                break;
            default:
                result = currentImage;
                break;
        }
    } else if (filterType == 1) { // Convolution filters
        if (filterIndex < 5) { // Predefined filters
            switch (filterIndex) {
                case 0: // Blur
                    result = processor.applyBlur(currentImage);
                    break;
                case 1: // Gaussian blur
                    result = processor.applyGaussianBlur(currentImage);
                    break;
                case 2: // Sharpen
                    result = processor.applySharpen(currentImage);
                    break;
                case 3: // Edge detection
                    result = processor.applyEdgeDetection(currentImage);
                    break;
                case 4: // Emboss
                    result = processor.applyEmboss(currentImage);
                    break;
                default:
                    result = currentImage;
                    break;
            }
        } else { // Custom filter
            QVector<QVector<double>> kernel;
            kernel.resize(kernelTable->rowCount());
            
            for (int i = 0; i < kernelTable->rowCount(); ++i) {
                kernel[i].resize(kernelTable->columnCount());
                for (int j = 0; j < kernelTable->columnCount(); ++j) {
                    kernel[i][j] = kernelTable->item(i, j)->text().toDouble();
                }
            }
            
            double divisor = divisorSpinBox->value();
            double offset = offsetSpinBox->value();
            int anchorX = anchorXSpinBox->value();
            int anchorY = anchorYSpinBox->value();
            
            result = processor.applyConvolutionFilter(currentImage, kernel, divisor, offset, anchorX, anchorY);
        }
    } else if (filterType == 2) { // Median filter
        result = processor.applyMedianFilter(currentImage, medianSizeSpinBox->value());
    }
    
    // Update display
    updateImage(result);
    
    statusBar()->showMessage(tr("Filter applied: %1").arg(filterSelectionComboBox->currentText()), 3000);
}

void MainWindow::undoFilter()
{
    if (imageHistory.isEmpty()) {
        QMessageBox::information(this, tr("Undo"), tr("Nothing to undo"));
        return;
    }
    
    // Restore previous image
    currentImage = imageHistory.pop();
    displayImage(currentImage);
    
    statusBar()->showMessage(tr("Undo applied"), 3000);
}

void MainWindow::updateKernelSize()
{
    // Block signals to prevent recursive calls
    kernelTable->blockSignals(true);
    
    int rows = kernelRowsSpinBox->value();
    int cols = kernelColsSpinBox->value();
    
    // Ensure odd dimensions
    if (rows % 2 == 0) {
        rows += 1;
        kernelRowsSpinBox->setValue(rows);
    }
    
    if (cols % 2 == 0) {
        cols += 1;
        kernelColsSpinBox->setValue(cols);
    }
    
    // Update anchor range
    anchorXSpinBox->setRange(0, cols - 1);
    anchorYSpinBox->setRange(0, rows - 1);
    
    // Set default anchor to center
    anchorXSpinBox->setValue(cols / 2);
    anchorYSpinBox->setValue(rows / 2);
    
    // Get current values to preserve them
    QVector<QVector<QString>> currentValues;
    int oldRows = kernelTable->rowCount();
    int oldCols = kernelTable->columnCount();
    
    // Save current values
    currentValues.resize(oldRows);
    for (int i = 0; i < oldRows; ++i) {
        currentValues[i].resize(oldCols);
        for (int j = 0; j < oldCols; ++j) {
            QTableWidgetItem *item = kernelTable->item(i, j);
            if (item) {
                currentValues[i][j] = item->text();
            } else {
                currentValues[i][j] = "0";
            }
        }
    }
    
    // Resize table
    kernelTable->setRowCount(rows);
    kernelTable->setColumnCount(cols);
    
    // Fill cells with appropriate values
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            QString value = "0";
            
            // Use existing value if available
            if (i < oldRows && j < oldCols) {
                value = currentValues[i][j];
            }
            
            QTableWidgetItem *item = kernelTable->item(i, j);
            if (!item) {
                item = new QTableWidgetItem(value);
                kernelTable->setItem(i, j, item);
            } else {
                item->setText(value);
            }
        }
    }
    
    // Update divisor if auto-calculate is enabled
    if (autoDivisorCheckBox->isChecked()) {
        calculateDivisor();
    }
    
    // Restore signals
    kernelTable->blockSignals(false);
}

void MainWindow::updateKernelTable()
{
    int filterIndex = filterSelectionComboBox->currentIndex();
    
    if (filterTypeComboBox->currentIndex() == 1 && filterIndex < 5) { // Convolution filters
        QVector<QVector<double>> kernel;
        double divisor = 1.0;
        double offset = 0.0;
        
        // Get the appropriate kernel based on filter index
        switch (filterIndex) {
            case 0: // Blur
                kernel = processor.getBlurKernel();
                divisor = 9.0;
                break;
            case 1: // Gaussian blur
                kernel = processor.getGaussianBlurKernel();
                divisor = 16.0;
                break;
            case 2: // Sharpen
                kernel = processor.getSharpenKernel();
                break;
            case 3: // Edge detection
                kernel = processor.getEdgeDetectionKernel();
                break;
            case 4: // Emboss
                kernel = processor.getEmbossKernel();
                offset = 128.0;
                break;
            default:
                // Default to a 3x3 identity kernel
                kernel = {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}};
                break;
        }
        
        // Ensure kernel is valid
        if (kernel.isEmpty()) {
            kernel = {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}}; // Default to identity kernel
        }
        
        // Ensure all rows have the same number of columns
        int maxCols = 0;
        for (const auto& row : kernel) {
            maxCols = qMax(maxCols, row.size());
        }
        
        for (auto& row : kernel) {
            while (row.size() < maxCols) {
                row.append(0.0);
            }
        }
        
        // Get dimensions
        int rows = kernel.size();
        int cols = rows > 0 ? kernel[0].size() : 3;
        
        // Update spin boxes (without triggering their signals)
        kernelRowsSpinBox->blockSignals(true);
        kernelColsSpinBox->blockSignals(true);
        kernelRowsSpinBox->setValue(rows);
        kernelColsSpinBox->setValue(cols);
        kernelRowsSpinBox->blockSignals(false);
        kernelColsSpinBox->blockSignals(false);
        
        // Resize the kernel table
        kernelTable->blockSignals(true);
        kernelTable->setRowCount(rows);
        kernelTable->setColumnCount(cols);
        
        // Update kernel table
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                QTableWidgetItem *item = kernelTable->item(i, j);
                double value = (i < kernel.size() && j < kernel[i].size()) ? kernel[i][j] : 0.0;
                
                if (!item) {
                    item = new QTableWidgetItem(QString::number(value));
                    kernelTable->setItem(i, j, item);
                } else {
                    item->setText(QString::number(value));
                }
            }
        }
        kernelTable->blockSignals(false);
        
        // Update divisor and offset
        divisorSpinBox->setValue(divisor);
        offsetSpinBox->setValue(offset);
        
        // Update anchor point
        anchorXSpinBox->setRange(0, cols - 1);
        anchorYSpinBox->setRange(0, rows - 1);
        anchorXSpinBox->setValue(cols / 2);
        anchorYSpinBox->setValue(rows / 2);
    }
}

void MainWindow::calculateDivisor()
{
    double sum = 0.0;
    
    for (int i = 0; i < kernelTable->rowCount(); ++i) {
        for (int j = 0; j < kernelTable->columnCount(); ++j) {
            QTableWidgetItem *item = kernelTable->item(i, j);
            if (item) {
                bool ok;
                double value = item->text().toDouble(&ok);
                if (ok) {
                    sum += value;
                }
            }
        }
    }
    
    // If sum is zero or very close to zero, use 1.0 to avoid division by zero
    if (std::abs(sum) < 0.00001) {
        sum = 1.0;
    }
    
    divisorSpinBox->setValue(sum);
}

void MainWindow::updateFilterPreview()
{
    // If auto-calculate divisor is enabled, update divisor
    if (autoDivisorCheckBox->isChecked()) {
        calculateDivisor();
    }
}

void MainWindow::loadPredefinedFilter(int index)
{
    // Check if we need to show quantization or dithering controls
    if (filterTypeComboBox->currentIndex() == 0) { // Function filters
        bool showQuantization = (index == 5); // Uniform Quantization is at index 5
        bool showDithering = (index == 6); // Dithering is at index 6
        
        // Hide all parameter groups first
        functionParamsGroup->hide();
        quantizationParamsGroup->hide();
        ditheringParamsGroup->hide();
        
        // Show the appropriate parameter group
        if (showQuantization) {
            quantizationParamsGroup->show();
        } else if (showDithering) {
            ditheringParamsGroup->show();
        } else {
            functionParamsGroup->show();
        }
    }
    
    // Only update if we're in convolution filter mode
    if (filterTypeComboBox->currentIndex() == 1) {
        // Make sure the index is valid
        if (index >= 0 && index < filterSelectionComboBox->count()) {
            // Update the kernel table
            updateKernelTable();
            
            // If it's a custom filter (last item), enable all controls
            bool isCustom = (index == filterSelectionComboBox->count() - 1);
            kernelRowsSpinBox->setEnabled(isCustom);
            kernelColsSpinBox->setEnabled(isCustom);
            loadFilterButton->setEnabled(isCustom);
            saveFilterButton->setEnabled(isCustom);
        }
    }
}

void MainWindow::updateImage(const QImage &image)
{
    currentImage = image;
    displayImage(currentImage);
}

void MainWindow::displayImage(const QImage &image)
{
    imageLabel->setPixmap(QPixmap::fromImage(image));
    imageLabel->adjustSize();
}

void MainWindow::enableFilterControls(bool enable)
{
    filterTypeComboBox->setEnabled(enable);
    filterSelectionComboBox->setEnabled(enable);
    functionParamsGroup->setEnabled(enable);
    quantizationParamsGroup->setEnabled(enable);
    ditheringParamsGroup->setEnabled(enable);
    convolutionParamsGroup->setEnabled(enable);
    medianParamsGroup->setEnabled(enable);
    applyButton->setEnabled(enable);
    undoButton->setEnabled(enable);
    resetButton->setEnabled(enable);
    saveButton->setEnabled(enable);
}

void MainWindow::setupFunctionFilterControls()
{
    // Clear and add function filter options
    filterSelectionComboBox->clear();
    filterSelectionComboBox->addItem("Inversion");
    filterSelectionComboBox->addItem("Brightness Correction");
    filterSelectionComboBox->addItem("Contrast Enhancement");
    filterSelectionComboBox->addItem("Gamma Correction");
    filterSelectionComboBox->addItem("Grayscale");
    filterSelectionComboBox->addItem("Uniform Quantization");
    filterSelectionComboBox->addItem("Dithering");
    
    // Show function parameters, hide other parameters
    functionParamsGroup->setVisible(true);
    quantizationParamsGroup->setVisible(false);
    ditheringParamsGroup->setVisible(false);
    convolutionParamsGroup->setVisible(false);
    medianParamsGroup->setVisible(false);
}

void MainWindow::setupConvolutionFilterControls()
{
    // Clear and add convolution filter options
    filterSelectionComboBox->clear();
    filterSelectionComboBox->addItem("Blur");
    filterSelectionComboBox->addItem("Gaussian Blur");
    filterSelectionComboBox->addItem("Sharpen");
    filterSelectionComboBox->addItem("Edge Detection");
    filterSelectionComboBox->addItem("Emboss");
    filterSelectionComboBox->addItem("Custom");
    
    // Show convolution parameters, hide function parameters
    functionParamsGroup->setVisible(false);
    convolutionParamsGroup->setVisible(true);
    
    // Make sure the kernel table has at least a 3x3 grid
    if (kernelTable->rowCount() < 3 || kernelTable->columnCount() < 3) {
        kernelTable->setRowCount(3);
        kernelTable->setColumnCount(3);
        
        // Initialize with zeros if needed
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                QTableWidgetItem *item = kernelTable->item(i, j);
                if (!item) {
                    item = new QTableWidgetItem("0");
                    kernelTable->setItem(i, j, item);
                }
            }
        }
    }
    
    // Set the filter selection to the first item (Blur)
    filterSelectionComboBox->setCurrentIndex(0);
    
    // Update kernel table with selected filter
    // Call directly instead of relying on signal/slot
    updateKernelTable();
}

void MainWindow::setupMedianFilterControls()
{
    // Hide other parameter groups
    functionParamsGroup->hide();
    convolutionParamsGroup->hide();
    medianParamsGroup->show();
    
    // Clear and set up filter selection combo box
    filterSelectionComboBox->clear();
    filterSelectionComboBox->addItem("Median Filter");
    
    // Enable filter controls
    enableFilterControls(true);
}

void MainWindow::setupQuantizationFilterControls()
{
    // Hide other parameter groups
    functionParamsGroup->hide();
    convolutionParamsGroup->hide();
    medianParamsGroup->hide();
    quantizationParamsGroup->show();
    
    // Enable filter controls
    enableFilterControls(true);
}

void MainWindow::setupDitheringFilterControls()
{
    // Hide other parameter groups
    functionParamsGroup->hide();
    convolutionParamsGroup->hide();
    medianParamsGroup->hide();
    quantizationParamsGroup->hide();
    ditheringParamsGroup->show();
    
    // Enable filter controls
    enableFilterControls(true);
}

void MainWindow::switchFilterType(int index)
{
    // Block signals to prevent recursive calls
    filterTypeComboBox->blockSignals(true);
    filterSelectionComboBox->blockSignals(true);
    
    if (index == 0) {
        setupFunctionFilterControls();
        // Also check if we need to show quantization or dithering controls based on current selection
        if (filterSelectionComboBox->count() > 6) {
            if (filterSelectionComboBox->currentIndex() == 5) {
                functionParamsGroup->hide();
                quantizationParamsGroup->show();
            } else if (filterSelectionComboBox->currentIndex() == 6) {
                functionParamsGroup->hide();
                ditheringParamsGroup->show();
            }
        }
    } else if (index == 1) {
        setupConvolutionFilterControls();
    } else if (index == 2) {
        setupMedianFilterControls();
    }
    
    // Restore signals
    filterTypeComboBox->blockSignals(false);
    filterSelectionComboBox->blockSignals(false);
}

// Add a new method to display the original image
void MainWindow::displayOriginalImage(const QImage &image)
{
    originalImageLabel->setPixmap(QPixmap::fromImage(image));
    originalImageLabel->adjustSize();
}

void MainWindow::setupHSVControls()
{
    hsvGroup = new QGroupBox("HSV Color Space", this);
    QVBoxLayout *hsvLayout = new QVBoxLayout();
    
    // Create button
    convertToHSVButton = new QPushButton("Convert to HSV", this);
    
    // Create labels and scroll areas for channels
    hueChannelLabel = new QLabel("Hue Channel", this);
    saturationChannelLabel = new QLabel("Saturation Channel", this);
    valueChannelLabel = new QLabel("Value Channel", this);
    convertedRGBLabel = new QLabel("Converted RGB", this);
    
    hueScrollArea = new QScrollArea(this);
    saturationScrollArea = new QScrollArea(this);
    valueScrollArea = new QScrollArea(this);
    convertedRGBScrollArea = new QScrollArea(this);
    
    // Set minimum sizes for scroll areas
    QSize minSize(200, 200);
    hueScrollArea->setMinimumSize(minSize);
    saturationScrollArea->setMinimumSize(minSize);
    valueScrollArea->setMinimumSize(minSize);
    convertedRGBScrollArea->setMinimumSize(minSize);
    
    // Create grid layout for channel displays
    QGridLayout *channelsLayout = new QGridLayout();
    channelsLayout->addWidget(hueChannelLabel, 0, 0);
    channelsLayout->addWidget(saturationChannelLabel, 0, 1);
    channelsLayout->addWidget(valueChannelLabel, 0, 2);
    channelsLayout->addWidget(convertedRGBLabel, 0, 3);
    
    channelsLayout->addWidget(hueScrollArea, 1, 0);
    channelsLayout->addWidget(saturationScrollArea, 1, 1);
    channelsLayout->addWidget(valueScrollArea, 1, 2);
    channelsLayout->addWidget(convertedRGBScrollArea, 1, 3);
    
    // Add widgets to main layout
    hsvLayout->addWidget(convertToHSVButton);
    hsvLayout->addLayout(channelsLayout);
    
    hsvGroup->setLayout(hsvLayout);
    
    // Add to image panel layout instead of control panel
    QWidget *imagePanel = mainLayout->itemAt(1)->widget(); // Get the image panel widget
    QVBoxLayout *imagePanelLayout = qobject_cast<QVBoxLayout*>(imagePanel->layout());
    if (imagePanelLayout) {
        imagePanelLayout->addWidget(hsvGroup);
    }
    
    // Connect signal
    connect(convertToHSVButton, &QPushButton::clicked, this, &MainWindow::convertToHSV);
}

void MainWindow::convertToHSV()
{
    if (currentImage.isNull()) {
        QMessageBox::warning(this, "Error", "No image loaded!");
        return;
    }
    
    // Store original image for comparison
    QImage originalRGB = currentImage;
    
    // Convert to HSV
    QImage hsvImage = processor.convertToHSV(currentImage);
    QImage hueImage = processor.getHueChannel(hsvImage);
    QImage saturationImage = processor.getSaturationChannel(hsvImage);
    QImage valueImage = processor.getValueChannel(hsvImage);
    
    // Display HSV channels
    QLabel *hueLabel = new QLabel();
    hueLabel->setPixmap(QPixmap::fromImage(hueImage));
    hueScrollArea->setWidget(hueLabel);
    
    QLabel *saturationLabel = new QLabel();
    saturationLabel->setPixmap(QPixmap::fromImage(saturationImage));
    saturationScrollArea->setWidget(saturationLabel);
    
    QLabel *valueLabel = new QLabel();
    valueLabel->setPixmap(QPixmap::fromImage(valueImage));
    valueScrollArea->setWidget(valueLabel);
    
    // Automatically convert back to RGB
    QImage convertedRGB = processor.convertToRGB(hsvImage);
    
    // Display converted RGB image
    QLabel *rgbLabel = new QLabel();
    rgbLabel->setPixmap(QPixmap::fromImage(convertedRGB));
    convertedRGBScrollArea->setWidget(rgbLabel);
    
    // Compare original and converted images
    bool identical = true;
    int diffPixels = 0;
    const int threshold = 1;
    int maxDiff = 0;
    
    for (int y = 0; y < originalRGB.height(); ++y) {
        for (int x = 0; x < originalRGB.width(); ++x) {
            QRgb origPixel = originalRGB.pixel(x, y);
            QRgb convPixel = convertedRGB.pixel(x, y);
            
            // Calculate differences for each channel
            int rDiff = std::abs(qRed(origPixel) - qRed(convPixel));
            int gDiff = std::abs(qGreen(origPixel) - qGreen(convPixel));
            int bDiff = std::abs(qBlue(origPixel) - qBlue(convPixel));
            
            // Update maximum difference
            maxDiff = qMax(maxDiff, qMax(rDiff, qMax(gDiff, bDiff)));
            
            // Check if any channel difference exceeds the threshold
            if (rDiff > threshold || gDiff > threshold || bDiff > threshold) {
                identical = false;
                diffPixels++;
            }
        }
    }
    
    // Show comparison results
    QString message;
    if (identical) {
        message = "RGB -> HSV -> RGB conversion is perfect (identical images within threshold)";
    } else {
        message = QString("RGB -> HSV -> RGB conversion has %1 different pixels (threshold: ±%2, max diff: %3)")
                    .arg(diffPixels)
                    .arg(threshold)
                    .arg(maxDiff);
    }
    statusBar()->showMessage(message, 5000);
    
    // Update current image to the converted RGB for further processing
    currentImage = convertedRGB;
} 