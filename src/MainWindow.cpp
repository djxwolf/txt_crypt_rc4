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
#include <QSizePolicy>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_processor(new FileProcessor(this))
{
    setupUI();
    connectSignals();

    m_inPlaceCheckBox->setChecked(true);
    m_timeoutSpinBox->setValue(600);
}

void MainWindow::setupUI()
{
    setWindowTitle("RC4 Text File Encryption Tool");
    resize(900, 600);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Control Area
    QGroupBox *controlGroup = new QGroupBox("File Operations");
    QGridLayout *controlLayout = new QGridLayout(controlGroup);

    controlLayout->addWidget(new QLabel("Input File:"), 0, 0);
    m_inputFileEdit = new QLineEdit;
    controlLayout->addWidget(m_inputFileEdit, 0, 1);
    m_browseInputBtn = new QPushButton("Browse...");
    controlLayout->addWidget(m_browseInputBtn, 0, 2);

    controlLayout->addWidget(new QLabel("Output File:"), 1, 0);
    m_outputFileEdit = new QLineEdit;
    controlLayout->addWidget(m_outputFileEdit, 1, 1);
    m_browseOutputBtn = new QPushButton("Browse...");
    controlLayout->addWidget(m_browseOutputBtn, 1, 2);

    m_inPlaceCheckBox = new QCheckBox("In-place (overwrite original file)");
    controlLayout->addWidget(m_inPlaceCheckBox, 2, 0, 1, 3);

    controlLayout->addWidget(new QLabel("Timeout:"), 3, 0);
    m_timeoutSpinBox = new QSpinBox;
    m_timeoutSpinBox->setMinimum(-1);
    m_timeoutSpinBox->setMaximum(999999);
    m_timeoutSpinBox->setValue(600);
    m_timeoutSpinBox->setSuffix(" sec (0 or negative = no check)");
    controlLayout->addWidget(m_timeoutSpinBox, 3, 1, 1, 2);

    mainLayout->addWidget(controlGroup);

    // Action Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout;
    m_encryptBtn = new QPushButton("Encrypt");
    m_decryptBtn = new QPushButton("Decrypt");
    m_encryptBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_decryptBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    btnLayout->addWidget(m_encryptBtn, 1);
    btnLayout->addWidget(m_decryptBtn, 1);
    mainLayout->addLayout(btnLayout);

    // Progress Bar
    m_progressBar = new QProgressBar;
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->setFormat("%p%");
    m_progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
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
    mainLayout->addWidget(m_progressBar);

    // Content Area
    QHBoxLayout *contentLayout = new QHBoxLayout;

    QGroupBox *originalGroup = new QGroupBox("Original Content");
    QVBoxLayout *originalLayout = new QVBoxLayout(originalGroup);
    m_originalContentEdit = new QTextEdit;
    m_originalContentEdit->setReadOnly(true);
    originalLayout->addWidget(m_originalContentEdit);
    contentLayout->addWidget(originalGroup);

    QGroupBox *resultGroup = new QGroupBox("Processed Result");
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);
    m_resultContentEdit = new QTextEdit;
    m_resultContentEdit->setReadOnly(true);
    resultLayout->addWidget(m_resultContentEdit);
    contentLayout->addWidget(resultGroup);

    mainLayout->addLayout(contentLayout);

    // Status Bar with centered messages
    m_statusBar = new QStatusBar(this);
    setStatusBar(m_statusBar);
    m_statusBar->setStyleSheet("QStatusBar::item { border: none; }");

    QLabel *statusLabel = new QLabel;
    statusLabel->setAlignment(Qt::AlignCenter);
    m_statusBar->addPermanentWidget(statusLabel, 1);
    m_statusBar->showMessage("Ready");

    connect(m_statusBar, &QStatusBar::messageChanged, [statusLabel](const QString &message) {
        statusLabel->setText(message);
    });
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
}

void MainWindow::onBrowseInput()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Input File", "", "Text Files (*.txt);;All Files (*)");
    if (!fileName.isEmpty()) {
        clearStatus();
        m_progressBar->setValue(0);

        QSignalBlocker blocker(m_inputFileEdit);
        m_inputFileEdit->setText(fileName);
        m_resultContentEdit->clear();

        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            m_originalContentEdit->setText(content);
            file.close();

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
    clearStatus();
    m_progressBar->setValue(0);

    if (text.isEmpty()) {
        m_originalContentEdit->clear();
        m_resultContentEdit->clear();
        m_encryptBtn->setEnabled(false);
        m_decryptBtn->setEnabled(false);
        return;
    }

    QString inputPath = resolveInputPath(text);
    m_resultContentEdit->clear();

    QFile file(inputPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        m_originalContentEdit->setText(content);
        file.close();

        bool isEncrypted = isEncryptedFile(content);
        if (isEncrypted) {
            m_encryptBtn->setEnabled(false);
            m_decryptBtn->setEnabled(true);
        } else {
            m_encryptBtn->setEnabled(true);
            m_decryptBtn->setEnabled(false);
        }
    } else {
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
        outputPath = resolveOutputPath(inputPath, userOutputPath);
    }

    m_resultContentEdit->clear();

    int timeout = m_timeoutSpinBox->value();
    auto result = m_processor->encryptFile(inputPath, timeout);

    if (result.success) {
        m_resultContentEdit->setText(result.outputData);

        QFile outputFile(outputPath);
        if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&outputFile);
            out << result.outputData;
            outputFile.close();
            showStatus("Encryption completed");
            m_progressBar->setValue(100);
        } else {
            showStatus("Error: Cannot write output file");
            m_progressBar->setValue(0);
        }
    } else {
        showStatus("Error: " + result.errorMessage);
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
        outputPath = resolveOutputPath(inputPath, userOutputPath);
    }

    m_resultContentEdit->clear();

    int timeout = m_timeoutSpinBox->value();
    auto result = m_processor->decryptFile(inputPath, timeout);

    if (result.success) {
        m_resultContentEdit->setText(result.outputData);

        QFile outputFile(outputPath);
        if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&outputFile);
            out << result.outputData;
            outputFile.close();
            showStatus("Decryption completed");
            m_progressBar->setValue(100);
        } else {
            showStatus("Error: Cannot write output file");
            m_progressBar->setValue(0);
        }
    } else {
        showStatus("Error: " + result.errorMessage);
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
    QString trimmed = content.trimmed();

    if (!trimmed.contains('@')) {
        return false;
    }

    QString timestamp, encryptedData;
    if (!Validator::parseEncryptedFormat(trimmed, timestamp, encryptedData)) {
        return false;
    }

    if (!Validator::isValidTimestamp(timestamp)) {
        return false;
    }

    if (!Validator::isValidBase64(encryptedData)) {
        return false;
    }

    return true;
}

QString MainWindow::resolveOutputPath(const QString &inputPath, const QString &outputPath)
{
    QFileInfo outputFileInfo(outputPath);

    if (outputFileInfo.isAbsolute()) {
        return outputPath;
    }

    QFileInfo inputFileInfo(inputPath);
    QString inputDir = inputFileInfo.absolutePath();
    QString resolvedPath = QDir(inputDir).filePath(outputPath);

    return QDir::cleanPath(resolvedPath);
}

QString MainWindow::resolveInputPath(const QString &userPath)
{
    QFileInfo fileInfo(userPath);

    if (fileInfo.isAbsolute()) {
        return userPath;
    }

    QString homePath = QDir::homePath();
    QString resolvedPath = QDir(homePath).filePath(userPath);

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
