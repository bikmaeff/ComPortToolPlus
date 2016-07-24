#ifndef RECEIVETHREAD_H
#define RECEIVETHREAD_H

#include <QThread>
#include <QSerialPort>
#include <QByteArray>
#include <QDebug>

class ReceiveThread : public QThread
{
    Q_OBJECT
public:
    explicit ReceiveThread(QObject* parent = 0);
    ~ReceiveThread();
    void run();
    void setSerialPort(QSerialPort* port);
signals:
    void receivedDataByte(char);
private:
    QSerialPort* serialPort;


};

#endif // RECEIVETHREAD_H
