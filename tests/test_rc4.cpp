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
