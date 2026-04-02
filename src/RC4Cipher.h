#ifndef RC4CIPHER_H
#define RC4CIPHER_H

#include <QString>
#include <QByteArray>

class RC4Cipher
{
public:
    // Encrypt data using key
    static QByteArray encrypt(const QByteArray &data, const QString &key);

    // Decrypt data using key (RC4 is symmetric, decryption is same as encryption)
    static QByteArray decrypt(const QByteArray &data, const QString &key);

private:
    // RC4 Key Scheduling Algorithm (KSA)
    static QByteArray ksa(const QString &key);

    // RC4 Pseudo-Random Generation Algorithm (PRGA)
    static QByteArray prga(const QByteArray &S, const QByteArray &data);
};

#endif // RC4CIPHER_H
