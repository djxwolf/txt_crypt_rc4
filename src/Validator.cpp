#include "Validator.h"
#include <QDateTime>
#include <QRegularExpression>

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

bool Validator::isValidTimestamp(const QString &timestampStr)
{
    bool ok;
    qint64 timestamp = timestampStr.toLongLong(&ok);

    if (!ok) {
        return false;
    }

    // 合理的时间戳范围：2020-01-01 到 2030-12-31
    // 2020-01-01 00:00:00 UTC = 1577836800
    // 2030-12-31 23:59:59 UTC = 1924905599
    const qint64 MIN_TIMESTAMP = 1577836800;  // 2020-01-01
    const qint64 MAX_TIMESTAMP = 1924905599;  // 2030-12-31

    return timestamp >= MIN_TIMESTAMP && timestamp <= MAX_TIMESTAMP;
}

bool Validator::isValidBase64(const QString &str)
{
    // Base64 只包含 A-Z, a-z, 0-9, +, /, = 字符
    // 并且长度应该是 4 的倍数
    if (str.isEmpty()) {
        return false;
    }

    // 检查长度是否是 4 的倍数（Base64 编码的要求）
    if (str.length() % 4 != 0) {
        return false;
    }

    // 使用正则表达式验证 Base64 格式
    QRegularExpression base64Regex("^[A-Za-z0-9+/]*={0,2}$");
    return base64Regex.match(str).hasMatch();
}
