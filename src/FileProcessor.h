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
