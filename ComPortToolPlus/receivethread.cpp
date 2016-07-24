#include "receivethread.h"

ReceiveThread::ReceiveThread(QObject* parent):QThread(parent)
{

}

ReceiveThread::~ReceiveThread()
{

}

void ReceiveThread::run()
{
    QByteArray dataArray;
    while(true)
    {
        serialPort->waitForReadyRead(-1);
        dataArray = serialPort->readAll();
        for(QByteArray::Iterator iter = dataArray.begin(); iter != dataArray.end(); iter++)
        {
            emit receivedDataByte(*iter);
        }
    }
}

void ReceiveThread::setSerialPort(QSerialPort* port)
{
    serialPort = port;
}
