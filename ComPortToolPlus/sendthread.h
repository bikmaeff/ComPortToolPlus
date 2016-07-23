#ifndef SENDTHREAD_H
#define SENDTHREAD_H

#include <QThread>
#include <QSerialPort>

class SendThread : public QThread
{
    Q_OBJECT
public:
    explicit SendThread(QObject* parent);
    ~SendThread();
    void run();
    void setSerialPort(QSerialPort* port);
    void setRepeatInterval(long interval);
signals:
    void sendDataOnce();
private:
    QSerialPort* serialPort;
    long repeatInterval;
};

#endif // SENDTHREAD_H
