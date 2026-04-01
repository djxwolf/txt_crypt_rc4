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

    // ===== 状态栏 =====
    m_statusBar = new QStatusBar(this);
    setStatusBar(m_statusBar);
    m_statusBar->showMessage("就绪");
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
    QString fileName = QFileDialog::getOpenFileName(this, "选择输入文件", "", "文本文件 (*.txt);;所有文件 (*)");
    if (!fileName.isEmpty()) {
        // 清除状态栏消息
        clearStatus();

        // 阻止信号发射，避免重复处理
        QSignalBlocker blocker(m_inputFileEdit);
        m_inputFileEdit->setText(fileName);

        // 清除之前的结果
        m_resultContentEdit->clear();

        // 读取并显示原始内容
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString content = in.readAll();
            m_originalContentEdit->setText(content);
            file.close();

            // 根据文件内容判断是否为加密文件
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
    // 清除状态栏信息
    clearStatus();

    // 如果输入为空，清空预览并禁用按钮
    if (text.isEmpty()) {
        m_originalContentEdit->clear();
        m_resultContentEdit->clear();
        m_encryptBtn->setEnabled(false);
        m_decryptBtn->setEnabled(false);
        return;
    }

    // 解析输入文件路径（支持相对于 home 目录的路径）
    QString inputPath = resolveInputPath(text);

    // 清除之前的结果
    m_resultContentEdit->clear();

    // 读取并显示原始内容
    QFile file(inputPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        m_originalContentEdit->setText(content);
        file.close();

        // 根据文件内容判断是否为加密文件
        bool isEncrypted = isEncryptedFile(content);
        if (isEncrypted) {
            m_encryptBtn->setEnabled(false);
            m_decryptBtn->setEnabled(true);
        } else {
            m_encryptBtn->setEnabled(true);
            m_decryptBtn->setEnabled(false);
        }
    } else {
        // 文件无法读取，清空预览并禁用按钮
        m_originalContentEdit->clear();
        m_encryptBtn->setEnabled(false);
        m_decryptBtn->setEnabled(false);
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
    QString userPath = m_inputFileEdit->text();
    if (userPath.isEmpty()) {
        showStatus("请选择输入文件");
        return;
    }

    // 解析输入文件路径（支持相对于 home 目录的路径）
    QString inputPath = resolveInputPath(userPath);

    QString outputPath;
    if (m_inPlaceCheckBox->isChecked()) {
        outputPath = inputPath;
    } else {
        QString userOutputPath = m_outputFileEdit->text();
        if (userOutputPath.isEmpty()) {
            showStatus("请指定输出文件");
            return;
        }
        // 解析输出路径（支持相对于输入文件的路径）
        outputPath = resolveOutputPath(inputPath, userOutputPath);
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
            showStatus("加密完成");
        } else {
            showStatus("错误：无法写入输出文件");
        }
    } else {
        showStatus("错误：" + result.errorMessage);
    }

    m_progressBar->setValue(0);
}

void MainWindow::onDecrypt()
{
    QString userPath = m_inputFileEdit->text();
    if (userPath.isEmpty()) {
        showStatus("请选择输入文件");
        return;
    }

    // 解析输入文件路径（支持相对于 home 目录的路径）
    QString inputPath = resolveInputPath(userPath);

    QString outputPath;
    if (m_inPlaceCheckBox->isChecked()) {
        outputPath = inputPath;
    } else {
        QString userOutputPath = m_outputFileEdit->text();
        if (userOutputPath.isEmpty()) {
            showStatus("请指定输出文件");
            return;
        }
        // 解析输出路径（支持相对于输入文件的路径）
        outputPath = resolveOutputPath(inputPath, userOutputPath);
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
            showStatus("解密完成");
        } else {
            showStatus("错误：无法写入输出文件");
        }
    } else {
        showStatus("错误：" + result.errorMessage);
    }

    m_progressBar->setValue(0);
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
    // 检查内容是否符合加密格式: "timestamp@encryptedData"
    QString trimmed = content.trimmed();

    // 基本格式检查：必须包含 @ 符号
    if (!trimmed.contains('@')) {
        return false;
    }

    // 使用 Validator 的解析方法来验证格式
    QString timestamp, encryptedData;
    if (!Validator::parseEncryptedFormat(trimmed, timestamp, encryptedData)) {
        return false;
    }

    // 进一步验证：时间戳应该是数字
    bool ok;
    timestamp.toLongLong(&ok);
    if (!ok) {
        return false;
    }

    // 加密数据不应该是空
    if (encryptedData.isEmpty()) {
        return false;
    }

    return true;
}

QString MainWindow::resolveOutputPath(const QString &inputPath, const QString &outputPath)
{
    QFileInfo outputFileInfo(outputPath);

    // 如果输出路径是绝对路径，直接使用
    if (outputFileInfo.isAbsolute()) {
        return outputPath;
    }

    // 输出路径是相对路径，基于输入文件的目录解析
    QFileInfo inputFileInfo(inputPath);
    QString inputDir = inputFileInfo.absolutePath();

    // 组合输入文件目录和输出相对路径
    QString resolvedPath = QDir(inputDir).filePath(outputPath);

    // 返回规范化的绝对路径
    return QDir::cleanPath(resolvedPath);
}

QString MainWindow::resolveInputPath(const QString &userPath)
{
    QFileInfo fileInfo(userPath);

    // 如果已经是绝对路径，直接返回
    if (fileInfo.isAbsolute()) {
        return userPath;
    }

    // 相对路径：相对于用户的 home 目录
    QString homePath = QDir::homePath();
    QString resolvedPath = QDir(homePath).filePath(userPath);

    // 返回规范化的绝对路径
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
