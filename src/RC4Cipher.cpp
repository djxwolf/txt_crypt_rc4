#include "RC4Cipher.h"
#include <algorithm>
#include <QtGlobal>

QByteArray RC4Cipher::ksa(const QString &key)
{
    // 初始化 S-box (0-255)
    QByteArray S(256, 0);
    for (int i = 0; i < 256; ++i) {
        S[i] = static_cast<char>(i);
    }

    // 用密钥打乱 S-box
    QByteArray keyBytes = key.toUtf8();
    int keyLength = keyBytes.size();
    int j = 0;

    for (int i = 0; i < 256; ++i) {
        j = (j + static_cast<uchar>(S[i]) + static_cast<uchar>(keyBytes[i % keyLength])) % 256;
        std::swap(S[i], S[j]);
    }

    return S;
}

QByteArray RC4Cipher::prga(const QByteArray &S, const QByteArray &data)
{
    QByteArray result = S; // 复制 S-box
    QByteArray output;
    output.reserve(data.size());

    int i = 0;
    int j = 0;

    for (char byte : data) {
        i = (i + 1) % 256;
        j = (j + static_cast<uchar>(result[i])) % 256;
        std::swap(result[i], result[j]);

        uchar k = static_cast<uchar>(result[(static_cast<uchar>(result[i]) + static_cast<uchar>(result[j])) % 256]);
        output.append(static_cast<char>(static_cast<uchar>(byte) ^ k));
    }

    return output;
}

QByteArray RC4Cipher::encrypt(const QByteArray &data, const QString &key)
{
    QByteArray S = ksa(key);
    return prga(S, data);
}

QByteArray RC4Cipher::decrypt(const QByteArray &data, const QString &key)
{
    // RC4 是对称加密,解密与加密相同
    return encrypt(data, key);
}
