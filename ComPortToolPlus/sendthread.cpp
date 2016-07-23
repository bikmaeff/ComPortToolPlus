#include "sendthread.h"

SendThread::SendThread(QObject* parent):QThread(parent)
{

}

SendThread::~SendThread()
{

}

void SendThread::run()
{
    if(repeatInterval > 0)
    {
        while(true)
        {
            if(serialPort->isWritable())
            {
                emit sendDataOnce();
                QThread::msleep((unsigned long) (repeatInterval));
            }
        }
    }
    if(repeatInterval <= 0)
    {
        serialPort->waitForBytesWritten(-1);
        emit sendDataOnce();
    }
}
void SendThread::setSerialPort(QSerialPort *port)
{
    serialPort = port;
}

void SendThread::setRepeatInterval(long interval)
{
    repeatInterval = interval;
}
