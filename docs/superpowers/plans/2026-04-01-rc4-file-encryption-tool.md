# RC4 文本文件加密工具 - 实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**目标:** 开发一个基于 Qt/C++ 的 GUI 工具，使用 RC4 算法加密文本文件，输出格式为 `时间戳@加密字符串`，时间戳作为密钥。

**架构:** 分层架构 - RC4Cipher 负责加密算法，Validator 负责超时验证，FileProcessor 协调文件操作，MainWindow 管理 GUI 交互。

**技术栈:** C++17, Qt 5.15+/6.x, CMake

---

## 文件结构

```
txt_crypt_rc4/
├── CMakeLists.txt              # CMake 构建配置
├── src/
│   ├── main.cpp                # 程序入口
│   ├── MainWindow.cpp          # 主窗口实现
│   ├── MainWindow.h            # 主窗口头文件
│   ├── RC4Cipher.cpp           # RC4 算法实现
│   ├── RC4Cipher.h             # RC4 头文件
│   ├── FileProcessor.cpp       # 文件处理协调
│   ├── FileProcessor.h         # 文件处理头文件
│   └── Validator.cpp           # 验证逻辑
│   └── Validator.h             # 验证头文件
├── tests/
│   ├── test_rc4.cpp            # RC4 单元测试
│   └── test_validator.cpp      # 验证器测试
└── resources/
    └── ui/
        └── mainwindow.ui       # Qt Designer UI 文件（可选）
```

---

### Task 1: 创建 CMake 项目配置

**文件:**
- 创建: `CMakeLists.txt`

- [ ] **步骤 1: 创建 CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.16)
project(TxtCryptRC4 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
# Or for Qt5:
# find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/RC4Cipher.cpp
    src/FileProcessor.cpp
    src/Validator.cpp
)

set(HEADERS
    src/MainWindow.h
    src/RC4Cipher.h
    src/FileProcessor.h
    src/Validator.h
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
)

if(Qt6_FOUND)
    target_link_libraries(${PROJECT_NAME}
        Qt6::Core
        Qt6::Widgets
    )
else()
    target_link_libraries(${PROJECT_NAME}
        Qt5::Core
        Qt5::Widgets
    )
endif()

# Enable testing
enable_testing()
add_subdirectory(tests)
```

- [ ] **步骤 2: 验证配置文件语法**

运行: `cmake -S . -B build`
预期: 无错误，配置成功

- [ ] **步骤 3: 提交**

```bash
git add CMakeLists.txt
git commit -m "build: add CMake project configuration"
```

---

### Task 2: 实现 RC4 加密算法

**文件:**
- 创建: `src/RC4Cipher.h`
- 创建: `src/RC4Cipher.cpp`

- [ ] **步骤 1: 创建 RC4Cipher.h 头文件**

```cpp
#ifndef RC4CIPHER_H
#define RC4CIPHER_H

#include <QString>
#include <QByteArray>

class RC4Cipher
{
public:
    // 使用密钥加密数据
    static QByteArray encrypt(const QByteArray &data, const QString &key);

    // 使用密钥解密数据 (RC4 是对称的，解密与加密相同)
    static QByteArray decrypt(const QByteArray &data, const QString &key);

private:
    // RC4 密钥调度算法 (KSA)
    static QByteArray ksa(const QString &key);

    // RC4 伪随机生成算法 (PRGA)
    static QByteArray prga(const QByteArray &S, const QByteArray &data);
};

#endif // RC4CIPHER_H
```

- [ ] **步骤 2: 创建 RC4Cipher.cpp 实现**

```cpp
#include "RC4Cipher.h"

QByteArray RC4Cipher::ksa(const QString &key)
{
    // 初始化 S-box (0-255)
    QByteArray S(256, 0);
    for (int i = 0; i < 256; ++i) {
        S[i] = static_cast<char>(i);
    }

    // 用密钥打乱 S-box
    QByteArray keyBytes = key.toUtf8();
    int keyLength = keyBytes.size();
    int j = 0;

    for (int i = 0; i < 256; ++i) {
        j = (j + static_cast<uchar>(S[i]) + static_cast<uchar>(keyBytes[i % keyLength])) % 256;
        std::swap(S[i], S[j]);
    }

    return S;
}

QByteArray RC4Cipher::prga(const QByteArray &S, const QByteArray &data)
{
    QByteArray result = S; // 复制 S-box
    QByteArray output;
    output.reserve(data.size());

    int i = 0;
    int j = 0;

    for (char byte : data) {
        i = (i + 1) % 256;
        j = (j + static_cast<uchar>(result[i])) % 256;
        std::swap(result[i], result[j]);

        uchar k = static_cast<uchar>(result[(static_cast<uchar>(result[i]) + static_cast<uchar>(result[j])) % 256]);
        output.append(static_cast<char>(static_cast<uchar>(byte) ^ k));
    }

    return output;
}

QByteArray RC4Cipher::encrypt(const QByteArray &data, const QString &key)
{
    QByteArray S = ksa(key);
    return prga(S, data);
}

QByteArray RC4Cipher::decrypt(const QByteArray &data, const QString &key)
{
    // RC4 是对称加密，解密与加密相同
    return encrypt(data, key);
}
```

- [ ] **步骤 3: 提交**

```bash
git add src/RC4Cipher.h src/RC4Cipher.cpp
git commit -m "feat: implement RC4 cipher algorithm"
```

---

### Task 3: 实现验证器 (Validator)

**文件:**
- 创建: `src/Validator.h`
- 创建: `src/Validator.cpp`

- [ ] **步骤 1: 创建 Validator.h**

```cpp
#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <QString>

class Validator
{
public:
    // 验证结果
    struct ValidationResult {
        bool valid;
        QString errorMessage;
    };

    // 验证加密数据是否超时
    static ValidationResult validateTimeout(const QString &timestampStr, int timeoutSeconds);

    // 解析加密格式 "timestamp@encryptedData"
    static bool parseEncryptedFormat(const QString &input, QString &timestamp, QString &encryptedData);

    // 获取当前时间戳（秒）
    static qint64 getCurrentTimestamp();

    // 检查 timeout 值是否有效
    static bool isTimeoutDisabled(int timeoutSeconds);
};

#endif // VALIDATOR_H
```

- [ ] **步骤 2: 创建 Validator.cpp**

```cpp
#include "Validator.h"
#include <QDateTime>

Validator::ValidationResult Validator::validateTimeout(const QString &timestampStr, int timeoutSeconds)
{
    bool ok;
    qint64 encryptTime = timestampStr.toLongLong(&ok);

    if (!ok) {
        return {false, "无效的时间戳格式"};
    }

    // 检查是否禁用 timeout
    if (isTimeoutDisabled(timeoutSeconds)) {
        return {true, ""};
    }

    qint64 currentTime = getCurrentTimestamp();
    qint64 elapsed = currentTime - encryptTime;

    if (elapsed < 0) {
        return {false, "加密时间戳在未来，系统时间可能不正确"};
    }

    if (elapsed > timeoutSeconds) {
        return {false, QString("数据已过期（超过 %1 秒）").arg(timeoutSeconds)};
    }

    return {true, ""};
}

bool Validator::parseEncryptedFormat(const QString &input, QString &timestamp, QString &encryptedData)
{
    int atIndex = input.indexOf('@');
    if (atIndex == -1) {
        return false;
    }

    timestamp = input.left(atIndex);
    encryptedData = input.mid(atIndex + 1);

    return !timestamp.isEmpty() && !encryptedData.isEmpty();
}

qint64 Validator::getCurrentTimestamp()
{
    return QDateTime::currentSecsSinceEpoch();
}

bool Validator::isTimeoutDisabled(int timeoutSeconds)
{
    return timeoutSeconds <= 0;
}
```

- [ ] **步骤 3: 提交**

```bash
git add src/Validator.h src/Validator.cpp
git commit -m "feat: implement timeout validator"
```

---

### Task 4: 实现文件处理器 (FileProcessor)

**文件:**
- 创建: `src/FileProcessor.h`
- 创建: `src/FileProcessor.cpp`

- [ ] **步骤 1: 创建 FileProcessor.h**

```cpp
#ifndef FILEPROCESSOR_H
#define FILEPROCESSOR_H

#include <QObject>
#include <QString>
#include "RC4Cipher.h"
#include "Validator.h"

class FileProcessor : public QObject
{
    Q_OBJECT

public:
    explicit FileProcessor(QObject *parent = nullptr);

    // 处理结果
    struct ProcessResult {
        bool success;
        QString errorMessage;
        QString outputData;
    };

    // 加密文件
    ProcessResult encryptFile(const QString &inputPath, int timeoutSeconds);

    // 解密文件
    ProcessResult decryptFile(const QString &inputPath, int timeoutSeconds);

signals:
    void progressChanged(int percent);
    void statusChanged(const QString &status);

private:
    QString formatOutput(const QString &timestamp, const QByteArray &encryptedData);
    QByteArray base64Encode(const QByteArray &data);
    QByteArray base64Decode(const QString &data);
};

#endif // FILEPROCESSOR_H
```

- [ ] **步骤 2: 创建 FileProcessor.cpp**

```cpp
#include "FileProcessor.h"
#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QCryptographicHash>

FileProcessor::FileProcessor(QObject *parent)
    : QObject(parent)
{
}

FileProcessor::ProcessResult FileProcessor::encryptFile(const QString &inputPath, int timeoutSeconds)
{
    emit statusChanged("读取文件...");

    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {false, "无法打开输入文件: " + inputFile.errorString(), ""};
    }

    QTextStream in(&inputFile);
    QString content = in.readAll();
    inputFile.close();

    emit progressChanged(30);

    emit statusChanged("加密中...");
    QString timestamp = QString::number(Validator::getCurrentTimestamp());
    QString key = timestamp;

    QByteArray encryptedData = RC4Cipher::encrypt(content.toUtf8(), key);
    QString base64Data = QString::fromUtf8(base64Encode(encryptedData));

    emit progressChanged(70);

    QString output = formatOutput(timestamp, base64Data);

    emit progressChanged(100);
    return {true, "", output};
}

FileProcessor::ProcessResult FileProcessor::decryptFile(const QString &inputPath, int timeoutSeconds)
{
    emit statusChanged("读取文件...");

    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {false, "无法打开输入文件: " + inputFile.errorString(), ""};
    }

    QTextStream in(&inputFile);
    QString content = in.readAll().trimmed();
    inputFile.close();

    emit progressChanged(20);

    emit statusChanged("解析格式...");
    QString timestamp, encryptedData;
    if (!Validator::parseEncryptedFormat(content, timestamp, encryptedData)) {
        return {false, "无效的加密格式，应为 '时间戳@加密内容'", ""};
    }

    emit progressChanged(40);

    emit statusChanged("验证超时...");
    auto validation = Validator::validateTimeout(timestamp, timeoutSeconds);
    if (!validation.valid) {
        return {false, validation.errorMessage, ""};
    }

    emit progressChanged(60);

    emit statusChanged("解密中...");
    QString key = timestamp;
    QByteArray decodedData = base64Decode(encryptedData);
    QByteArray decryptedData = RC4Cipher::decrypt(decodedData, key);
    QString output = QString::fromUtf8(decryptedData);

    emit progressChanged(100);
    return {true, "", output};
}

QString FileProcessor::formatOutput(const QString &timestamp, const QByteArray &encryptedData)
{
    return timestamp + "@" + QString::fromUtf8(encryptedData);
}

QByteArray FileProcessor::base64Encode(const QByteArray &data)
{
    return data.toBase64();
}

QByteArray FileProcessor::base64Decode(const QString &data)
{
    return QByteArray::fromBase64(data.toUtf8());
}
```

- [ ] **步骤 3: 提交**

```bash
git add src/FileProcessor.h src/FileProcessor.cpp
git commit -m "feat: implement file processor"
```

---

### Task 5: 实现主窗口界面

**文件:**
- 创建: `src/MainWindow.h`
- 创建: `src/MainWindow.cpp`

- [ ] **步骤 1: 创建 MainWindow.h**

```cpp
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QProgressBar>
#include <QTextEdit>
#include <QLabel>
#include "FileProcessor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onBrowseInput();
    void onBrowseOutput();
    void onInPlaceToggled(bool checked);
    void onEncrypt();
    void onDecrypt();
    void onProgressChanged(int percent);
    void onStatusChanged(const QString &status);

private:
    void setupUI();
    void connectSignals();

    // UI 组件
    QLineEdit *m_inputFileEdit;
    QLineEdit *m_outputFileEdit;
    QPushButton *m_browseInputBtn;
    QPushButton *m_browseOutputBtn;
    QCheckBox *m_inPlaceCheckBox;
    QSpinBox *m_timeoutSpinBox;
    QPushButton *m_encryptBtn;
    QPushButton *m_decryptBtn;
    QProgressBar *m_progressBar;
    QTextEdit *m_originalContentEdit;
    QTextEdit *m_resultContentEdit;

    FileProcessor *m_processor;
};

#endif // MAINWINDOW_H
```

- [ ] **步骤 2: 创建 MainWindow.cpp**

```cpp
#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>

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
```

- [ ] **步骤 3: 提交**

```bash
git add src/MainWindow.h src/MainWindow.cpp
git commit -m "feat: implement main window UI"
```

---

### Task 6: 创建程序入口

**文件:**
- 创建: `src/main.cpp`

- [ ] **步骤 1: 创建 main.cpp**

```cpp
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
```

- [ ] **步骤 2: 提交**

```bash
git add src/main.cpp
git commit -m "feat: add application entry point"
```

---

### Task 7: 添加测试框架和 RC4 测试

**文件:**
- 创建: `tests/CMakeLists.txt`
- 创建: `tests/test_rc4.cpp`

- [ ] **步骤 1: 创建测试 CMakeLists.txt**

```cmake
# 使用 Qt Test 框架
find_package(Qt6 REQUIRED COMPONENTS Test)
# Or for Qt5:
# find_package(Qt5 REQUIRED COMPONENTS Test)

set(TEST_SOURCES
    test_rc4.cpp
    test_validator.cpp
)

add_executable(txt_crypt_rc4_tests ${TEST_SOURCES})

target_link_libraries(txt_crypt_rc4_tests
    Qt6::Test
    Qt6::Core
)

# Add tests to CTest
add_test(NAME RC4CipherTest COMMAND txt_crypt_rc4_tests)
add_test(NAME ValidatorTest COMMAND txt_crypt_rc4_tests)
```

- [ ] **步骤 2: 创建 test_rc4.cpp**

```cpp
#include <QtTest/QtTest>
#include "../src/RC4Cipher.h"

class TestRC4Cipher : public QObject
{
    Q_OBJECT

private slots:
    void testEncryptDecrypt();
    void testDifferentKeys();
    void testEmptyInput();
};

void TestRC4Cipher::testEncryptDecrypt()
{
    QString original = "Hello, RC4!";
    QString key = "1234567890";

    QByteArray encrypted = RC4Cipher::encrypt(original.toUtf8(), key);
    QByteArray decrypted = RC4Cipher::decrypt(encrypted, key);
    QString result = QString::fromUtf8(decrypted);

    QCOMPARE(result, original);
    QVERIFY(encrypted != original.toUtf8());
}

void TestRC4Cipher::testDifferentKeys()
{
    QString original = "Test data";
    QString key1 = "1111111111";
    QString key2 = "2222222222";

    QByteArray encrypted1 = RC4Cipher::encrypt(original.toUtf8(), key1);
    QByteArray encrypted2 = RC4Cipher::encrypt(original.toUtf8(), key2);

    QVERIFY(encrypted1 != encrypted2);
}

void TestRC4Cipher::testEmptyInput()
{
    QString original = "";
    QString key = "1234567890";

    QByteArray encrypted = RC4Cipher::encrypt(original.toUtf8(), key);
    QByteArray decrypted = RC4Cipher::decrypt(encrypted, key);
    QString result = QString::fromUtf8(decrypted);

    QCOMPARE(result, original);
}

QTEST_MAIN(TestRC4Cipher)
#include "test_rc4.moc"
```

- [ ] **步骤 3: 运行测试验证 RC4 实现**

运行: `cd build && cmake .. && make && ctest --verbose`
预期: RC4 测试通过

- [ ] **步骤 4: 提交**

```bash
git add tests/CMakeLists.txt tests/test_rc4.cpp
git commit -m "test: add RC4 cipher unit tests"
```

---

### Task 8: 添加验证器测试

**文件:**
- 创建: `tests/test_validator.cpp`

- [ ] **步骤 1: 创建 test_validator.cpp**

```cpp
#include <QtTest/QtTest>
#include "../src/Validator.h>

class TestValidator : public QObject
{
    Q_OBJECT

private slots:
    void testValidateTimeoutValid();
    void testValidateTimeoutExpired();
    void testValidateTimeoutDisabled();
    void testParseEncryptedFormat();
    void testParseInvalidFormat();
};

void TestValidator::testValidateTimeoutValid()
{
    qint64 currentTime = Validator::getCurrentTimestamp();
    QString timestamp = QString::number(currentTime);

    auto result = Validator::validateTimeout(timestamp, 600);

    QVERIFY(result.valid);
    QVERIFY(result.errorMessage.isEmpty());
}

void TestValidator::testValidateTimeoutExpired()
{
    qint64 oldTime = Validator::getCurrentTimestamp() - 1000;
    QString timestamp = QString::number(oldTime);

    auto result = Validator::validateTimeout(timestamp, 600);

    QVERIFY(!result.valid);
    QVERIFY(result.errorMessage.contains("过期"));
}

void TestValidator::testValidateTimeoutDisabled()
{
    qint64 oldTime = Validator::getCurrentTimestamp() - 10000;
    QString timestamp = QString::number(oldTime);

    // timeout = 0 表示禁用
    auto result = Validator::validateTimeout(timestamp, 0);

    QVERIFY(result.valid);
}

void TestValidator::testParseEncryptedFormat()
{
    QString input = "1712345678@SGVsbG8gV29ybGQ=";
    QString timestamp, encryptedData;

    bool success = Validator::parseEncryptedFormat(input, timestamp, encryptedData);

    QVERIFY(success);
    QCOMPARE(timestamp, QString("1712345678"));
    QCOMPARE(encryptedData, QString("SGVsbG8gV29ybGQ="));
}

void TestValidator::testParseInvalidFormat()
{
    QString input = "InvalidFormatWithoutAtSymbol";
    QString timestamp, encryptedData;

    bool success = Validator::parseEncryptedFormat(input, timestamp, encryptedData);

    QVERIFY(!success);
}

QTEST_MAIN(TestValidator)
#include "test_validator.moc"
```

- [ ] **步骤 2: 更新 tests/CMakeLists.txt**

```cmake
# 使用 Qt Test 框架
find_package(Qt6 REQUIRED COMPONENTS Test)
# Or for Qt5:
# find_package(Qt5 REQUIRED COMPONENTS Test)

set(TEST_SOURCES
    test_rc4.cpp
    test_validator.cpp
)

# 分别添加测试
add_executable(test_rc4 test_rc4.cpp)
target_link_libraries(test_rc4 Qt6::Test Qt6::Core)
add_test(NAME RC4CipherTest COMMAND test_rc4)

add_executable(test_validator test_validator.cpp)
target_link_libraries(test_validator Qt6::Test Qt6::Core)
add_test(NAME ValidatorTest COMMAND test_validator)
```

- [ ] **步骤 3: 运行所有测试**

运行: `cd build && ctest --verbose`
预期: 所有测试通过

- [ ] **步骤 4: 提交**

```bash
git add tests/test_validator.cpp tests/CMakeLists.txt
git commit -m "test: add validator unit tests"
```

---

### Task 9: 创建 README 文档

**文件:**
- 创建: `README.md`

- [ ] **步骤 1: 创建 README.md**

```markdown
# RC4 文本文件加密工具

一个基于 Qt/C++ 的简单文本文件加密解密工具，使用 RC4 算法。

## 功能特性

- 使用 RC4 算法加密/解密文本文件
- 时间戳作为加密密钥，无需额外输入
- 输出格式: `时间戳@加密字符串`
- 可配置的超时验证
- In-place 模式（直接覆盖原文件）
- 图形界面操作

## 构建

### 依赖

- Qt 5.15+ 或 Qt 6.x
- CMake 3.16+
- C++17 编译器

### 编译步骤

```bash
mkdir build
cd build
cmake ..
make
```

## 使用

1. 选择输入文件
2. 选择输出文件（或勾选 In-place 覆盖原文件）
3. 设置 Timeout 值（0 或负值表示不检查）
4. 点击"加密"或"解密"按钮

## 加密格式

加密后的文件内容格式为:

```
时间戳@Base64编码的密文
```

例如:
```
1712345678@SGVsbG8gV29ybGQ=
```

解密时，程序会自动提取时间戳作为密钥，并验证是否超时。

## License

MIT
```

- [ ] **步骤 2: 提交**

```bash
git add README.md
git commit -m "docs: add README documentation"
```

---

### Task 10: 构建和运行测试

- [ ] **步骤 1: 构建项目**

运行: `cd build && make`
预期: 编译成功，无错误

- [ ] **步骤 2: 运行单元测试**

运行: `cd build && ctest --verbose`
预期: 所有测试通过

- [ ] **步骤 3: 手动功能测试**

1. 创建测试文件 `test_input.txt`，内容: `Hello, World!`
2. 运行程序: `./build/TxtCryptRC4`
3. 测试加密流程
4. 测试解密流程
5. 测试 timeout 验证
6. 测试 In-place 模式

- [ ] **步骤 4: 最终提交**

```bash
git add -A
git commit -m "chore: final project setup complete"
```

---

## 总结

实现计划包含以下任务:

1. ✅ 创建 CMake 项目配置
2. ✅ 实现 RC4 加密算法
3. ✅ 实现验证器 (Validator)
4. ✅ 实现文件处理器 (FileProcessor)
5. ✅ 实现主窗口界面 (MainWindow)
6. ✅ 创建程序入口
7. ✅ 添加 RC4 单元测试
8. ✅ 添加验证器测试
9. ✅ 创建 README 文档
10. ✅ 构建和运行测试

总计: **10 个任务**, **~40 个步骤**
