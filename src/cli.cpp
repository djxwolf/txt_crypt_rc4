#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QCommandLineParser>
#include <QFileInfo>
#include "RC4Cipher.h"
#include "Validator.h"

void printUsage(const QString &appName)
{
    QTextStream(stderr) << "Usage: " << appName << " <command> <options>\n\n"
                       << "Commands:\n"
                       << "  encrypt <input-file> [output-file]  Encrypt a file\n"
                       << "  decrypt <input-file> [output-file]  Decrypt a file\n\n"
                       << "Options:\n"
                       << "  -t, --timeout <seconds>               Timeout in seconds (default: 600)\n"
                       << "                                       Use 0 or negative to disable timeout\n"
                       << "  -o, --output <file>                  Specify output file\n"
                       << "  -i, --in-place                       Overwrite input file\n"
                       << "  -h, --help                          Show this help message\n\n"
                       << "Examples:\n"
                       << "  " << appName << " encrypt document.txt\n"
                       << "  " << appName << " decrypt document.txt document_decrypted.txt\n"
                       << "  " << appName << " encrypt -i document.txt\n"
                       << "  " << appName << " decrypt -t 0 encrypted.txt\n";
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("txt_crypt_rc4");
    QCoreApplication::setApplicationVersion("3.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("RC4 Text File Encryption Tool - Command Line Interface");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("command", "Command to execute (encrypt/decrypt)");
    parser.addPositionalArgument("input-file", "Input file path");

    QCommandLineOption timeoutOption(QStringList() << "t" << "timeout",
        "Timeout in seconds (default: 600, use 0 or negative to disable)", "seconds", "600");
    parser.addOption(timeoutOption);

    QCommandLineOption outputOption(QStringList() << "o" << "output",
        "Output file path", "file");
    parser.addOption(outputOption);

    QCommandLineOption inPlaceOption(QStringList() << "i" << "in-place",
        "Overwrite input file (in-place mode)");
    parser.addOption(inPlaceOption);

    parser.process(app);

    QStringList args = parser.positionalArguments();
    if (args.isEmpty()) {
        parser.showHelp();
        return 1;
    }

    QString command = args[0];
    if (command != "encrypt" && command != "decrypt") {
        QTextStream(stderr) << "Error: Invalid command '" << command << "'\n";
        QTextStream(stderr) << "Valid commands are: encrypt, decrypt\n";
        return 1;
    }

    if (args.size() < 2) {
        QTextStream(stderr) << "Error: Input file is required\n";
        printUsage(QCoreApplication::applicationName());
        return 1;
    }

    QString inputPath = args[1];
    QString outputPath;

    // Determine output path
    if (parser.isSet(inPlaceOption)) {
        outputPath = inputPath;
    } else if (parser.isSet(outputOption)) {
        outputPath = parser.value(outputOption);
    } else {
        // Default: add suffix to input file
        QFileInfo fileInfo(inputPath);
        QString baseName = fileInfo.completeBaseName();
        QString extension = fileInfo.suffix();
        QString suffix = command == "encrypt" ? "_encrypted" : "_decrypted";
        if (extension.isEmpty()) {
            outputPath = fileInfo.path() + "/" + baseName + suffix;
        } else {
            outputPath = fileInfo.path() + "/" + baseName + suffix + "." + extension;
        }
    }

    // Read input file
    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream(stderr) << "Error: Cannot open input file: " << inputPath << "\n";
        return 1;
    }

    QTextStream in(&inputFile);
    QString content = in.readAll();
    inputFile.close();

    QString result;
    bool success = true;
    QString errorMessage;

    int timeout = parser.value(timeoutOption).toInt();

    if (command == "encrypt") {
        QString timestamp = QString::number(Validator::getCurrentTimestamp());
        QString key = timestamp;

        QByteArray encryptedData = RC4Cipher::encrypt(content.toUtf8(), key);
        QString base64Data = QString::fromUtf8(encryptedData.toBase64());
        result = timestamp + "@" + base64Data;

        QTextStream(stdout) << "Encrypting with timestamp: " << timestamp << "\n";
    } else {
        QString trimmed = content.trimmed();
        QString timestamp, encryptedData;

        if (!Validator::parseEncryptedFormat(trimmed, timestamp, encryptedData)) {
            QTextStream(stderr) << "Error: Invalid encrypted format. Expected 'timestamp@encryptedData'\n";
            return 1;
        }

        if (!Validator::isValidTimestamp(timestamp)) {
            QTextStream(stderr) << "Error: Invalid timestamp\n";
            return 1;
        }

        auto validation = Validator::validateTimeout(timestamp, timeout);
        if (!validation.valid) {
            QTextStream(stderr) << "Error: " << validation.errorMessage << "\n";
            return 1;
        }

        QString key = timestamp;
        QByteArray decodedData = QByteArray::fromBase64(encryptedData.toUtf8());
        QByteArray decryptedData = RC4Cipher::decrypt(decodedData, key);
        result = QString::fromUtf8(decryptedData);

        QTextStream(stdout) << "Decrypting with timestamp: " << timestamp << "\n";
    }

    // Write output file
    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream(stderr) << "Error: Cannot write output file: " << outputPath << "\n";
        return 1;
    }

    QTextStream out(&outputFile);
    out << result;
    outputFile.close();

    QTextStream(stdout) << "Success! Output written to: " << outputPath << "\n";

    return 0;
}
