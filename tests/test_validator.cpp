#include <QtTest/QtTest>
#include "../src/Validator.h"

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
