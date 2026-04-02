#include "MainWindow.h"
#include "Validator.h"
#include <QDir>
#include <QFileInfo>
#include <QSignalBlocker>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFontMetrics>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_processor(new FileProcessor(this))
{
    setupUI();
    connectSignals();

    // Set default values
    m_inPlaceCheckBox->setChecked(true);
    m_timeoutSpinBox->setValue(600);
}

void MainWindow::setupUI()
{
    setWindowTitle("RC4 Text File Encryption Tool");
    resize(900, 600);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Apply rounded corner style to main window
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
        QWidget {
            background-color: #f5f5f5;
            font-size: 13px;
        }
        QGroupBox {
            border: 1px solid #ccc;
            border-radius: 6px;
            margin-top: 10px;
            padding-top: 10px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 5px;
        }
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: 500;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
        QPushButton:pressed {
            background-color: #3d8b40;
        }
        QPushButton:disabled {
            background-color: #cccccc;
            color: #666666;
        }
        QLineEdit {
            border: 1px solid #ccc;
            border-radius: 4px;
            padding: 6px;
            background-color: white;
        }
        QLineEdit:focus {
            border: 1px solid #4CAF50;
        }
        QSpinBox {
            border: 1px solid #ccc;
            border-radius: 4px;
            padding: 4px;
            background-color: white;
        }
        QTextEdit {
            border: 1px solid #ccc;
            border-radius: 4px;
            background-color: white;
        }
        QCheckBox {
            spacing: 5px;
        }
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 3px;
            border: 1px solid #ccc;
        }
        QCheckBox::indicator:checked {
            background-color: #4CAF50;
            border: 1px solid #4CAF50;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // ===== Control Area =====
    QGroupBox *controlGroup = new QGroupBox("File Operations");
    QGridLayout *controlLayout = new QGridLayout(controlGroup);

    // Input file
    controlLayout->addWidget(new QLabel("Input File:"), 0, 0);
    m_inputFileEdit = new QLineEdit;
    controlLayout->addWidget(m_inputFileEdit, 0, 1);
    m_browseInputBtn = new QPushButton("Browse...");
    controlLayout->addWidget(m_browseInputBtn, 0, 2);

    // Output file
    controlLayout->addWidget(new QLabel("Output File:"), 1, 0);
    m_outputFileEdit = new QLineEdit;
    controlLayout->addWidget(m_outputFileEdit, 1, 1);
    m_browseOutputBtn = new QPushButton("Browse...");
    controlLayout->addWidget(m_browseOutputBtn, 1, 2);

    // In-place option
    m_inPlaceCheckBox = new QCheckBox("In-place (overwrite original file)");
    controlLayout->addWidget(m_inPlaceCheckBox, 2, 0, 1, 3);

    // Timeout setting
    controlLayout->addWidget(new QLabel("Timeout:"), 3, 0);
    m_timeoutSpinBox = new QSpinBox;
    m_timeoutSpinBox->setMinimum(-1);
    m_timeoutSpinBox->setMaximum(999999);
    m_timeoutSpinBox->setValue(600);
    m_timeoutSpinBox->setSuffix(" sec (0 or negative = no check)");
    controlLayout->addWidget(m_timeoutSpinBox, 3, 1, 1, 2);

    mainLayout->addWidget(controlGroup);

    // Action buttons - evenly distributed across the full width
    QHBoxLayout *btnLayout = new QHBoxLayout;
    m_encryptBtn = new QPushButton("Encrypt");
    m_decryptBtn = new QPushButton("Decrypt");

    // Make buttons expand to fill available space
    m_encryptBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_decryptBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    btnLayout->addWidget(m_encryptBtn, 1);
    btnLayout->addWidget(m_decryptBtn, 1);
    mainLayout->addLayout(btnLayout);

    // ===== Content Area =====
    QHBoxLayout *contentLayout = new QHBoxLayout;

    // Left: Original content
    QGroupBox *originalGroup = new QGroupBox("Original Content");
    QVBoxLayout *originalLayout = new QVBoxLayout(originalGroup);
    m_originalContentEdit = new QTextEdit;
    m_originalContentEdit->setReadOnly(true);
    originalLayout->addWidget(m_originalContentEdit);
    contentLayout->addWidget(originalGroup);

    // Right: Processed result
    QGroupBox *resultGroup = new QGroupBox("Processed Result");
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);
    m_resultContentEdit = new QTextEdit;
    m_resultContentEdit->setReadOnly(true);
    resultLayout->addWidget(m_resultContentEdit);
    contentLayout->addWidget(resultGroup);

    mainLayout->addLayout(contentLayout);

    // ===== Status Bar =====
    m_statusBar = new QStatusBar(this);
    setStatusBar(m_statusBar);

    // Progress bar placed on right side of status bar, chunk style
    m_progressBar = new QProgressBar;
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->setFormat("%p%");
    // Set progress bar to expand as much as possible
    m_progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // Set chunk-style progress bar
    m_progressBar->setStyleSheet(
        "QProgressBar {"
        "    border: 1px solid #ccc;"
        "    border-radius: 3px;"
        "    text-align: center;"
        "    background-color: #f0f0f0;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #4CAF50;"
        "    width: 10px;"
        "    margin: 1px;"
        "}"
    );
    // Add progress bar as permanent widget, placed on right side
    m_statusBar->addPermanentWidget(m_progressBar, 1);

    // Initial message
    m_statusBar->showMessage("Ready");
}

void MainWindow::connectSignals()
{
    connect(m_browseInputBtn, &QPushButton::clicked, this, &MainWindow::onBrowseInput);
    connect(m_browseOutputBtn, &QPushButton::clicked, this, &MainWindow::onBrowseOutput);
    connect(m_inPlaceCheckBox, &QCheckBox::toggled, this, &MainWindow::onInPlaceToggled);
    connect(m_inputFileEdit, &QLineEdit::textChanged, this, &MainWindow::onInputFileChanged);
    connect(m_encryptBtn, &QPushButton::clicked, this, &MainWindow::onEncrypt);
    connect(m_decryptBtn, &QPushButton::clicked, this, &MainWindow::onDecrypt);
    connect(m_processor, &FileProcessor::progressChanged, this, &MainWindow::onProgressChanged);
    connect(m_processor, &FileProcessor::statusChanged, this, &MainWindow::onStatusChanged);
    connect(m_statusBar, &QStatusBar::messageChanged, this, &MainWindow::onStatusBarMessageChanged);
}

void MainWindow::onBrowseInput()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Input File", "", "Text Files (*.txt);;All Files (*)");
    if (!fileName.isEmpty()) {
        // Clear status bar message
        clearStatus();

        // Reset progress bar
        m_progressBar->setValue(0);

        // Block signal emission to avoid duplicate processing
        QSignalBlocker blocker(m_inputFileEdit);
        m_inputFileEdit->setText(fileName);

        // Clear previous results
        m_resultContentEdit->clear();

        // Read and display original content
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            m_originalContentEdit->setText(content);
            file.close();

            // Determine if file is encrypted based on content
            bool isEncrypted = isEncryptedFile(content);
            if (isEncrypted) {
                m_encryptBtn->setEnabled(false);
                m_decryptBtn->setEnabled(true);
            } else {
                m_encryptBtn->setEnabled(true);
                m_decryptBtn->setEnabled(false);
            }
        }
    }
}

void MainWindow::onInputFileChanged(const QString &text)
{
    // Clear status bar message
    clearStatus();

    // Reset progress bar
    m_progressBar->setValue(0);

    // If input is empty, clear preview and disable buttons
    if (text.isEmpty()) {
        m_originalContentEdit->clear();
        m_resultContentEdit->clear();
        m_encryptBtn->setEnabled(false);
        m_decryptBtn->setEnabled(false);
        return;
    }

    // Parse input file path (supports paths relative to home directory)
    QString inputPath = resolveInputPath(text);

    // Clear previous results
    m_resultContentEdit->clear();

    // Read and display original content
    QFile file(inputPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        m_originalContentEdit->setText(content);
        file.close();

        // Determine if file is encrypted based on content
        bool isEncrypted = isEncryptedFile(content);
        if (isEncrypted) {
            m_encryptBtn->setEnabled(false);
            m_decryptBtn->setEnabled(true);
        } else {
            m_encryptBtn->setEnabled(true);
            m_decryptBtn->setEnabled(false);
        }
    } else {
        // File cannot be read, clear preview and disable buttons
        m_originalContentEdit->clear();
        m_encryptBtn->setEnabled(false);
        m_decryptBtn->setEnabled(false);
    }
}

void MainWindow::onBrowseOutput()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Specify Output File", "", "Text Files (*.txt);;All Files (*)");
    if (!fileName.isEmpty()) {
        m_outputFileEdit->setText(fileName);
    }
}

void MainWindow::onInPlaceToggled(bool checked)
{
    m_outputFileEdit->setEnabled(!checked);
    m_browseOutputBtn->setEnabled(!checked);

    if (checked) {
        m_outputFileEdit->setText(m_inputFileEdit->text());
    }
}

void MainWindow::onEncrypt()
{
    QString userPath = m_inputFileEdit->text();
    if (userPath.isEmpty()) {
        showStatus("Please select an input file");
        return;
    }

    // Parse input file path (supports paths relative to home directory)
    QString inputPath = resolveInputPath(userPath);

    QString outputPath;
    if (m_inPlaceCheckBox->isChecked()) {
        outputPath = inputPath;
    } else {
        QString userOutputPath = m_outputFileEdit->text();
        if (userOutputPath.isEmpty()) {
            showStatus("Please specify an output file");
            return;
        }
        // Parse output path (supports paths relative to input file)
        outputPath = resolveOutputPath(inputPath, userOutputPath);
    }

    m_resultContentEdit->clear();

    int timeout = m_timeoutSpinBox->value();
    auto result = m_processor->encryptFile(inputPath, timeout);

    if (result.success) {
        m_resultContentEdit->setText(result.outputData);

        // Write output file
        QFile outputFile(outputPath);
        if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&outputFile);
            out << result.outputData;
            outputFile.close();
            showStatus("Encryption completed");
            // Progress bar stays at 100% on success
            m_progressBar->setValue(100);
        } else {
            showStatus("Error: Cannot write output file");
            // Reset progress bar on failure
            m_progressBar->setValue(0);
        }
    } else {
        showStatus("Error: " + result.errorMessage);
        // Reset progress bar on failure
        m_progressBar->setValue(0);
    }
}

void MainWindow::onDecrypt()
{
    QString userPath = m_inputFileEdit->text();
    if (userPath.isEmpty()) {
        showStatus("Please select an input file");
        return;
    }

    // Parse input file path (supports paths relative to home directory)
    QString inputPath = resolveInputPath(userPath);

    QString outputPath;
    if (m_inPlaceCheckBox->isChecked()) {
        outputPath = inputPath;
    } else {
        QString userOutputPath = m_outputFileEdit->text();
        if (userOutputPath.isEmpty()) {
            showStatus("Please specify an output file");
            return;
        }
        // Parse output path (supports paths relative to input file)
        outputPath = resolveOutputPath(inputPath, userOutputPath);
    }

    m_resultContentEdit->clear();

    int timeout = m_timeoutSpinBox->value();
    auto result = m_processor->decryptFile(inputPath, timeout);

    if (result.success) {
        m_resultContentEdit->setText(result.outputData);

        // Write output file
        QFile outputFile(outputPath);
        if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&outputFile);
            out << result.outputData;
            outputFile.close();
            showStatus("Decryption completed");
            // Progress bar stays at 100% on success
            m_progressBar->setValue(100);
        } else {
            showStatus("Error: Cannot write output file");
            // Reset progress bar on failure
            m_progressBar->setValue(0);
        }
    } else {
        showStatus("Error: " + result.errorMessage);
        // Reset progress bar on failure
        m_progressBar->setValue(0);
    }
}

void MainWindow::onProgressChanged(int percent)
{
    m_progressBar->setValue(percent);
}

void MainWindow::onStatusChanged(const QString &status)
{
    m_statusBar->showMessage(status);
}

bool MainWindow::isEncryptedFile(const QString &content)
{
    // Check if content matches encrypted format: "timestamp@encryptedData"
    QString trimmed = content.trimmed();

    // Basic format check: must contain @ symbol
    if (!trimmed.contains('@')) {
        return false;
    }

    // Use Validator's parsing method to validate format
    QString timestamp, encryptedData;
    if (!Validator::parseEncryptedFormat(trimmed, timestamp, encryptedData)) {
        return false;
    }

    // Validate timestamp is a reasonable second-level timestamp (not random numbers)
    if (!Validator::isValidTimestamp(timestamp)) {
        return false;
    }

    // Validate encrypted data is valid Base64 string
    if (!Validator::isValidBase64(encryptedData)) {
        return false;
    }

    return true;
}

QString MainWindow::resolveOutputPath(const QString &inputPath, const QString &outputPath)
{
    QFileInfo outputFileInfo(outputPath);

    // If output path is absolute, use directly
    if (outputFileInfo.isAbsolute()) {
        return outputPath;
    }

    // Output path is relative, resolve based on input file's directory
    QFileInfo inputFileInfo(inputPath);
    QString inputDir = inputFileInfo.absolutePath();

    // Combine input file directory and output relative path
    QString resolvedPath = QDir(inputDir).filePath(outputPath);

    // Return normalized absolute path
    return QDir::cleanPath(resolvedPath);
}

QString MainWindow::resolveInputPath(const QString &userPath)
{
    QFileInfo fileInfo(userPath);

    // If already absolute path, return directly
    if (fileInfo.isAbsolute()) {
        return userPath;
    }

    // Relative path: relative to user's home directory
    QString homePath = QDir::homePath();
    QString resolvedPath = QDir(homePath).filePath(userPath);

    // Return normalized absolute path
    return QDir::cleanPath(resolvedPath);
}

void MainWindow::showStatus(const QString &message)
{
    m_statusBar->showMessage(message);
}

void MainWindow::clearStatus()
{
    m_statusBar->clearMessage();
}

void MainWindow::onStatusBarMessageChanged(const QString &message)
{
    // Calculate required width for message text
    QFontMetrics fm(m_statusBar->font());

    if (message.isEmpty()) {
        // When message is empty, progress bar fills entire status bar
        m_progressBar->setMinimumWidth(0);
        m_progressBar->setMaximumWidth(16777215); // QWIDGETSIZE_MAX
    } else {
        // When message is not empty, calculate message width and set progress bar to take remaining space
        int messageWidth = fm.horizontalAdvance(message) + 30; // Add some margin
        int totalWidth = m_statusBar->width();

        if (totalWidth > messageWidth) {
            int availableWidth = totalWidth - messageWidth;
            m_progressBar->setMinimumWidth(availableWidth);
            m_progressBar->setMaximumWidth(availableWidth);
        } else {
            // Status bar too narrow, give progress bar at least 100px
            m_progressBar->setMinimumWidth(100);
            m_progressBar->setMaximumWidth(16777215);
        }
    }
}
