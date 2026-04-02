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

    // 验证是否为有效的时间戳（秒级）
    static bool isValidTimestamp(const QString &timestampStr);

    // 验证是否为有效的 Base64 字符串
    static bool isValidBase64(const QString &str);

    // 获取当前时间戳（秒）
    static qint64 getCurrentTimestamp();

    // 检查 timeout 值是否有效
    static bool isTimeoutDisabled(int timeoutSeconds);
};

#endif // VALIDATOR_H
