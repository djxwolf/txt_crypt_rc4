#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <QString>

class Validator
{
public:
    // Validation result
    struct ValidationResult {
        bool valid;
        QString errorMessage;
    };

    // Validate if encrypted data has timed out
    static ValidationResult validateTimeout(const QString &timestampStr, int timeoutSeconds);

    // Parse encrypted format "timestamp@encryptedData"
    static bool parseEncryptedFormat(const QString &input, QString &timestamp, QString &encryptedData);

    // Validate if it's a valid timestamp (seconds)
    static bool isValidTimestamp(const QString &timestampStr);

    // Validate if it's a valid Base64 string
    static bool isValidBase64(const QString &str);

    // Get current timestamp (seconds)
    static qint64 getCurrentTimestamp();

    // Check if timeout value is valid
    static bool isTimeoutDisabled(int timeoutSeconds);
};

#endif // VALIDATOR_H
