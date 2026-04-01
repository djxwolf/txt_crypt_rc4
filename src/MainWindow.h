#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QProgressBar>
#include <QTextEdit>
#include <QLabel>
#include "FileProcessor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onBrowseInput();
    void onBrowseOutput();
    void onInPlaceToggled(bool checked);
    void onEncrypt();
    void onDecrypt();
    void onProgressChanged(int percent);
    void onStatusChanged(const QString &status);

private:
    void setupUI();
    void connectSignals();
    bool isEncryptedFile(const QString &content);

    // UI 组件
    QLineEdit *m_inputFileEdit;
    QLineEdit *m_outputFileEdit;
    QPushButton *m_browseInputBtn;
    QPushButton *m_browseOutputBtn;
    QCheckBox *m_inPlaceCheckBox;
    QSpinBox *m_timeoutSpinBox;
    QPushButton *m_encryptBtn;
    QPushButton *m_decryptBtn;
    QProgressBar *m_progressBar;
    QTextEdit *m_originalContentEdit;
    QTextEdit *m_resultContentEdit;

    FileProcessor *m_processor;
};

#endif // MAINWINDOW_H
