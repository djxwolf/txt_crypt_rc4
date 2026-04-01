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
