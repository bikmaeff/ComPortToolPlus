#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QString>
#include <QTextcodec>
#include <QValidator>
#include <QTime>
#include <QTimer>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDir>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QTranslator>

#include "wavewindow.h"
#include "receivethread.h"
#include "sendthread.h"
#include "aboutwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionRefreshPortList_triggered();
    void on_actionQuit_triggered();
    void on_actionNewWindow_triggered();
    void on_actionClearDisplay_triggered();
    void on_actionMiniWindow_triggered();
    void on_actionAbout_triggered();
    void on_actionEnglish_triggered();
    void on_actionChinese_triggered();


    void on_pushButtonOscWindow_clicked();
    void on_pushButtonSaveAsText_clicked();
    void on_pushButtonClearDisplay_clicked();
    void on_pushButtonSendData_clicked();
    void on_pushButtonMiniWindow_clicked();

    void on_comboBoxPort_activated(const QString &arg1);
    void on_comboBoxBoundrate_currentTextChanged(const QString &arg1);
    void on_comboBoxDataBit_activated(const QString &arg1);
    void on_comboBoxParity_activated(const QString &arg1);
    void on_comboBoxStopBit_activated(const QString &arg1);
    void on_comboBoxFlowControl_activated(const QString &arg1);
    void on_comboBoxSendCodeSelection_currentIndexChanged(int index);
    void on_comboBoxReceiveCodeSelection_activated(int index);

    void on_checkBoxSingleLine_toggled(bool checked);
    void on_checkBoxDisplayTime_toggled(bool checked);
    void on_checkBoxRepeatTrans_toggled(bool checked);
    void on_checkBoxSeparation_toggled(bool checked);
    void on_spinBoxLoopInterval_valueChanged(int arg1);

    void on_lineEditDataToSend_returnPressed();
    void on_lineEditDataToSend_textEdited(const QString &arg1);

    void getByteData(char);
    void sendDataOnce();
    void sendThreadFinished();

private:
    void refreshPortList();
    void displayText(const QString& text);
    void comboBoxBoundrateValidate();
    void setVersionRelatedStyle();
    void initiations();
    void disposes();
    bool startListenSerialPort(QString port);
    QString transformCount(float count);
    void parseDataToSend();
    void parseAsHex(QString strData);
    void sendData();
    void adjustWindow();

    bool setPortBaudrate();
    bool setPortDataBit();
    bool setPortParity();
    bool setPortStopBit();
    bool setPortFlowControl();

    void loadLanguage();
    void setLanguage(QString lan);

    void keyPressEvent(QKeyEvent *event);

private:
    Ui::MainWindow *ui;

    WaveWindow* waveWindow;

    ReceiveThread* receiveThread;
    SendThread* sendThread;
    float receiveCount;
    float sendCount;
    QByteArray dataToSend;

    QTime time;

    QSerialPort* serialPort;
    bool isPortValid;
    bool isPortSending;
    bool isWindowNormal;
    long long byteIter;
    QRegularExpressionValidator* hexValidator;
    QRegularExpressionValidator* ascValidator;
    QRegularExpressionValidator* baudrateValidator;

    QTranslator trans;
};

#endif // MAINWINDOW_H
