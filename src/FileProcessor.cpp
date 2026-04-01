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
    QByteArray base64Data = base64Encode(encryptedData);

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
