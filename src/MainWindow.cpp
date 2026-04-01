#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_processor(new FileProcessor(this))
{
    setupUI();
    connectSignals();

    // 设置默认值
    m_inPlaceCheckBox->setChecked(true);
    m_timeoutSpinBox->setValue(600);
}

void MainWindow::setupUI()
{
    setWindowTitle("RC4 文本文件加密工具");
    resize(900, 600);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // ===== 上方控制区 =====
    QGroupBox *controlGroup = new QGroupBox("文件操作");
    QGridLayout *controlLayout = new QGridLayout(controlGroup);

    // 输入文件
    controlLayout->addWidget(new QLabel("输入文件:"), 0, 0);
    m_inputFileEdit = new QLineEdit;
    controlLayout->addWidget(m_inputFileEdit, 0, 1);
    m_browseInputBtn = new QPushButton("浏览...");
    controlLayout->addWidget(m_browseInputBtn, 0, 2);

    // 输出文件
    controlLayout->addWidget(new QLabel("输出文件:"), 1, 0);
    m_outputFileEdit = new QLineEdit;
    controlLayout->addWidget(m_outputFileEdit, 1, 1);
    m_browseOutputBtn = new QPushButton("浏览...");
    controlLayout->addWidget(m_browseOutputBtn, 1, 2);

    // In-place 选项
    m_inPlaceCheckBox = new QCheckBox("In-place（覆盖原文件）");
    controlLayout->addWidget(m_inPlaceCheckBox, 2, 0, 1, 3);

    // Timeout 设置
    controlLayout->addWidget(new QLabel("Timeout:"), 3, 0);
    m_timeoutSpinBox = new QSpinBox;
    m_timeoutSpinBox->setMinimum(-1);
    m_timeoutSpinBox->setMaximum(999999);
    m_timeoutSpinBox->setValue(600);
    m_timeoutSpinBox->setSuffix(" 秒 (0或负值=不检查)");
    controlLayout->addWidget(m_timeoutSpinBox, 3, 1, 1, 2);

    mainLayout->addWidget(controlGroup);

    // 操作按钮
    QHBoxLayout *btnLayout = new QHBoxLayout;
    m_encryptBtn = new QPushButton("加密");
    m_decryptBtn = new QPushButton("解密");
    btnLayout->addWidget(m_encryptBtn);
    btnLayout->addWidget(m_decryptBtn);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    // 进度条
    m_progressBar = new QProgressBar;
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    mainLayout->addWidget(m_progressBar);

    // ===== 下方内容区 =====
    QHBoxLayout *contentLayout = new QHBoxLayout;

    // 左侧：原始内容
    QGroupBox *originalGroup = new QGroupBox("原始内容");
    QVBoxLayout *originalLayout = new QVBoxLayout(originalGroup);
    m_originalContentEdit = new QTextEdit;
    m_originalContentEdit->setReadOnly(true);
    originalLayout->addWidget(m_originalContentEdit);
    contentLayout->addWidget(originalGroup);

    // 右侧：处理结果
    QGroupBox *resultGroup = new QGroupBox("处理结果");
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);
    m_resultContentEdit = new QTextEdit;
    m_resultContentEdit->setReadOnly(true);
    resultLayout->addWidget(m_resultContentEdit);
    contentLayout->addWidget(resultGroup);

    mainLayout->addLayout(contentLayout);
}

void MainWindow::connectSignals()
{
    connect(m_browseInputBtn, &QPushButton::clicked, this, &MainWindow::onBrowseInput);
    connect(m_browseOutputBtn, &QPushButton::clicked, this, &MainWindow::onBrowseOutput);
    connect(m_inPlaceCheckBox, &QCheckBox::toggled, this, &MainWindow::onInPlaceToggled);
    connect(m_encryptBtn, &QPushButton::clicked, this, &MainWindow::onEncrypt);
    connect(m_decryptBtn, &QPushButton::clicked, this, &MainWindow::onDecrypt);
    connect(m_processor, &FileProcessor::progressChanged, this, &MainWindow::onProgressChanged);
    connect(m_processor, &FileProcessor::statusChanged, this, &MainWindow::onStatusChanged);
}

void MainWindow::onBrowseInput()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择输入文件", "", "文本文件 (*.txt);;所有文件 (*)");
    if (!fileName.isEmpty()) {
        m_inputFileEdit->setText(fileName);

        // 清除之前的结果
        m_resultContentEdit->clear();

        // 读取并显示原始内容
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            m_originalContentEdit->setText(in.readAll());
            file.close();
        }
    }
}

void MainWindow::onBrowseOutput()
{
    QString fileName = QFileDialog::getSaveFileName(this, "指定输出文件", "", "文本文件 (*.txt);;所有文件 (*)");
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
    QString inputPath = m_inputFileEdit->text();
    if (inputPath.isEmpty()) {
        QMessageBox::warning(this, "错误", "请选择输入文件");
        return;
    }

    QString outputPath = m_inPlaceCheckBox->isChecked() ? inputPath : m_outputFileEdit->text();
    if (outputPath.isEmpty()) {
        QMessageBox::warning(this, "错误", "请指定输出文件");
        return;
    }

    m_progressBar->setValue(0);
    m_resultContentEdit->clear();

    int timeout = m_timeoutSpinBox->value();
    auto result = m_processor->encryptFile(inputPath, timeout);

    if (result.success) {
        m_resultContentEdit->setText(result.outputData);

        // 写入输出文件
        QFile outputFile(outputPath);
        if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&outputFile);
            out << result.outputData;
            outputFile.close();
            QMessageBox::information(this, "成功", "加密完成！");
        } else {
            QMessageBox::warning(this, "错误", "无法写入输出文件");
        }
    } else {
        QMessageBox::critical(this, "错误", result.errorMessage);
    }

    m_progressBar->setValue(0);
}

void MainWindow::onDecrypt()
{
    QString inputPath = m_inputFileEdit->text();
    if (inputPath.isEmpty()) {
        QMessageBox::warning(this, "错误", "请选择输入文件");
        return;
    }

    QString outputPath = m_inPlaceCheckBox->isChecked() ? inputPath : m_outputFileEdit->text();
    if (outputPath.isEmpty()) {
        QMessageBox::warning(this, "错误", "请指定输出文件");
        return;
    }

    m_progressBar->setValue(0);
    m_resultContentEdit->clear();

    int timeout = m_timeoutSpinBox->value();
    auto result = m_processor->decryptFile(inputPath, timeout);

    if (result.success) {
        m_resultContentEdit->setText(result.outputData);

        // 写入输出文件
        QFile outputFile(outputPath);
        if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&outputFile);
            out << result.outputData;
            outputFile.close();
            QMessageBox::information(this, "成功", "解密完成！");
        } else {
            QMessageBox::warning(this, "错误", "无法写入输出文件");
        }
    } else {
        QMessageBox::critical(this, "错误", result.errorMessage);
    }

    m_progressBar->setValue(0);
}

void MainWindow::onProgressChanged(int percent)
{
    m_progressBar->setValue(percent);
}

void MainWindow::onStatusChanged(const QString &status)
{
    setWindowTitle("RC4 文本文件加密工具 - " + status);
}
