#ifndef RC4CIPHER_H
#define RC4CIPHER_H

#include <QString>
#include <QByteArray>

class RC4Cipher
{
public:
    // 使用密钥加密数据
    static QByteArray encrypt(const QByteArray &data, const QString &key);

    // 使用密钥解密数据 (RC4 是对称的,解密与加密相同)
    static QByteArray decrypt(const QByteArray &data, const QString &key);

private:
    // RC4 密钥调度算法 (KSA)
    static QByteArray ksa(const QString &key);

    // RC4 伪随机生成算法 (PRGA)
    static QByteArray prga(const QByteArray &S, const QByteArray &data);
};

#endif // RC4CIPHER_H
