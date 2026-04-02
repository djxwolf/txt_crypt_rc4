#include "Validator.h"
#include <QDateTime>
#include <QRegularExpression>

Validator::ValidationResult Validator::validateTimeout(const QString &timestampStr, int timeoutSeconds)
{
    bool ok;
    qint64 encryptTime = timestampStr.toLongLong(&ok);

    if (!ok) {
        return {false, "Invalid timestamp format"};
    }

    // Check if timeout is disabled
    if (isTimeoutDisabled(timeoutSeconds)) {
        return {true, ""};
    }

    qint64 currentTime = getCurrentTimestamp();
    qint64 elapsed = currentTime - encryptTime;

    if (elapsed < 0) {
        return {false, "Encryption timestamp is in the future, system time may be incorrect"};
    }

    if (elapsed > timeoutSeconds) {
        return {false, QString("Data has expired (more than %1 seconds)").arg(timeoutSeconds)};
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

    // Reasonable timestamp range: 2020-01-01 to 2030-12-31
    // 2020-01-01 00:00:00 UTC = 1577836800
    // 2030-12-31 23:59:59 UTC = 1924905599
    const qint64 MIN_TIMESTAMP = 1577836800;  // 2020-01-01
    const qint64 MAX_TIMESTAMP = 1924905599;  // 2030-12-31

    return timestamp >= MIN_TIMESTAMP && timestamp <= MAX_TIMESTAMP;
}

bool Validator::isValidBase64(const QString &str)
{
    // Base64 only contains A-Z, a-z, 0-9, +, /, = characters
    // And length should be a multiple of 4
    if (str.isEmpty()) {
        return false;
    }

    // Check if length is a multiple of 4 (Base64 encoding requirement)
    if (str.length() % 4 != 0) {
        return false;
    }

    // Use regex to validate Base64 format
    QRegularExpression base64Regex("^[A-Za-z0-9+/]*={0,2}$");
    return base64Regex.match(str).hasMatch();
}
