#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setVersionRelatedStyle();
    initiations();
}

MainWindow::~MainWindow()
{
    disposes();
    delete ui;
}

//basics
void MainWindow::initiations()
{
    serialPort = new QSerialPort(this);
    receiveThread = new ReceiveThread(this);
    receiveThread->setSerialPort(serialPort);
    sendThread = new SendThread(this);
    sendThread->setSerialPort(serialPort);

    connect(receiveThread, SIGNAL(receivedDataByte(unsigned char)), this, SLOT(getByteData(unsigned char)));
    connect(sendThread,SIGNAL(sendDataOnce()),this,SLOT(sendDataOnce()));
    connect(sendThread,SIGNAL(finished()),this,SLOT(sendThreadFinished()));

    isPortSending = false;
    isWindowNormal = true;
    byteIter = 0;
    hexValidator = new QRegularExpressionValidator(QRegularExpression("^([0-9A-Fa-f]{2,2} ){0,2000}$"),this);
    ui->lineEditDataToSend->setValidator(hexValidator);

    ascValidator = new QRegularExpressionValidator(QRegularExpression("^[\\x00-\\x7F ]{0,5000}$"),this);

    baudrateValidator = new QRegularExpressionValidator(QRegularExpression("^[\\d]{1,15}$"),this);
    loadLanguage();
    ui->comboBoxBoundrate->setValidator(baudrateValidator);
    refreshPortList();
}
void MainWindow::disposes()
{
    receiveThread->terminate();
    sendThread->terminate();

    serialPort->close();
    disconnect(receiveThread, SIGNAL(receivedDataByte(unsigned char)), this, SLOT(getByteData(unsigned char)));
    disconnect(sendThread,SIGNAL(sendDataOnce()),this,SLOT(sendDataOnce()));
    disconnect(sendThread,SIGNAL(finished()),this,SLOT(sendThreadFinished()));
    delete receiveThread;
    delete sendThread;
    delete serialPort;
}
void MainWindow::setVersionRelatedStyle()
{
    //system version upper than windows 7,set to this flat style
    if(QSysInfo::windowsVersion() > QSysInfo::WV_WINDOWS7)
    {
        this->setStyleSheet("MainWindow{background-color: rgb(255, 255, 255);}");
    }
}
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_N)
    {
        if(QProcess::startDetached(qApp->applicationFilePath(),QStringList()) == false)
        {
            ui->statusBar->showMessage(tr("Can not find application, please reinstall!"),3000);
        }
    }
    if(event->key() == Qt::Key_Escape)
    {
        this->close();
    }
    if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_R)
    {
        refreshPortList();
    }
}
void MainWindow::loadLanguage()
{
    QFile lanFile(qApp->applicationDirPath() + QString("/languages/language.ini"));
    if(lanFile.open(QIODevice::ReadOnly))
    {
        QTextStream languageIni(&lanFile);
        QString currentLan = languageIni.readLine();
        lanFile.close();
        if(trans.load(currentLan,qApp->applicationDirPath() + QString("/languages")))
        {
            qApp->installTranslator(&trans);
            ui->retranslateUi(this);
            if(currentLan == QString("cn_zh"))
            {
                ui->actionChinese->setChecked(true);
                ui->actionEnglish->setChecked(false);
            }
            else
            {
                ui->actionChinese->setChecked(false);
                ui->actionEnglish->setChecked(true);
            }
        }
    }
    else
    {
        ui->actionEnglish->setChecked(true);
        ui->actionChinese->setChecked(false);
    }
}

void MainWindow::setLanguage(QString lan)
{
    QFile lanFile(qApp->applicationDirPath() + QString("/languages/language.ini"));
    if(lanFile.open(QIODevice::Truncate | QIODevice::WriteOnly))
    {
        QTextStream languageIni(&lanFile);
        languageIni<<lan;
        lanFile.close();
    }
    loadLanguage();
}

//definitions of all action bars
void MainWindow::on_actionRefreshPortList_triggered()
{
    refreshPortList();
}
void MainWindow::on_actionQuit_triggered()
{
    this->close();
    this->destroy();
}
void MainWindow::on_actionNewWindow_triggered()
{
    if(QProcess::startDetached(qApp->applicationFilePath(),QStringList()) == false)
    {
        ui->statusBar->showMessage(tr("Can not find application, please reinstall!"),3000);
    }
}
void MainWindow::on_actionClearDisplay_triggered()
{
    ui->textBrowserRecieved->clear();
}
void MainWindow::on_actionMiniWindow_triggered()
{
    adjustWindow();
}
void MainWindow::on_actionAbout_triggered()
{
    AboutWindow* about = new AboutWindow(this);
    about->show();
}
void MainWindow::on_actionEnglish_triggered()
{
    setLanguage(QString("en"));
}

void MainWindow::on_actionChinese_triggered()
{
    setLanguage(QString("cn_zh"));
}

//related to group box COM Port Information
void MainWindow::on_comboBoxPort_activated(const QString &arg1)
{
    if(startListenSerialPort(arg1) == false)
    {
        ui->statusBar->showMessage(tr("Open port ") + arg1 + tr("failed, please check hardware!"),3000);
    }
}
void MainWindow::on_comboBoxBoundrate_currentTextChanged(const QString &arg1)
{
    if (serialPort->setBaudRate(arg1.toInt()))//set baudrate
    {
        ui->statusBar->showMessage(tr("Baud rate set to ") + arg1, 3000);
    }
    else
    {
        ui->statusBar->showMessage(tr("Failed to set baud rate!"), 3000);
    }

}
void MainWindow::on_comboBoxDataBit_activated(const QString &arg1)
{
    if(setPortDataBit())
    {
        ui->statusBar->showMessage(tr("Data bits set to ") + arg1, 3000);
    }
    else
    {
        ui->statusBar->showMessage(tr("Failed to set data bits!"), 3000);
    }
}
void MainWindow::on_comboBoxParity_activated(const QString &arg1)
{
    if(setPortParity())
    {
        ui->statusBar->showMessage(tr("Parity set to ")+arg1, 3000);
    }
    else
    {
        ui->statusBar->showMessage(tr("Failed to set parity!"), 3000);
    }
}
void MainWindow::on_comboBoxStopBit_activated(const QString &arg1)
{
    if(setPortStopBit())
    {
        ui->statusBar->showMessage(tr("Stop bits set to ")+arg1, 3000);
    }
    else
    {
        ui->statusBar->showMessage(tr("Failed to set stop bits!"), 3000);
    }
}
void MainWindow::on_comboBoxFlowControl_activated(const QString &arg1)
{
    if(setPortFlowControl())
    {
        ui->statusBar->showMessage(tr("Flow control set to ")+arg1, 3000);
    }
    else
    {
        ui->statusBar->showMessage(tr("Failed to set flow control!"), 3000);
    }
}

bool MainWindow::setPortDataBit()
{
    switch(ui->comboBoxDataBit->currentIndex())
    {
        case 0: //8bit
            return serialPort->setDataBits(QSerialPort::Data8);
            break;
        case 1: //5bit
            return serialPort->setDataBits(QSerialPort::Data5);
            break;
        case 2: //6bit
            return serialPort->setDataBits(QSerialPort::Data6);
            break;
        case 3: //7bit
            return serialPort->setDataBits(QSerialPort::Data7);
            break;
        default:
            return false;
            break;
    }
}
bool MainWindow::setPortParity()
{
    switch(ui->comboBoxParity->currentIndex())
    {
        case 0: //no parity
            return serialPort->setParity(QSerialPort::NoParity);
            break;
        case 1: //even parity
            return serialPort->setParity(QSerialPort::EvenParity);
            break;
        case 2: //odd parity
            return serialPort->setParity(QSerialPort::OddParity);
            break;
        case 3: //space parity
            return serialPort->setParity(QSerialPort::SpaceParity);
            break;
        case 4: //mark parity
            return serialPort->setParity(QSerialPort::MarkParity);
            break;
        default:
            return false;
            break;
    }
}
bool MainWindow::setPortStopBit()
{
    switch(ui->comboBoxStopBit->currentIndex())
    {
        case 0: //1 bit
            return serialPort->setStopBits(QSerialPort::OneStop);
            break;
        case 1: //1.5 bit
            return serialPort->setStopBits(QSerialPort::OneAndHalfStop);
            break;
        case 2: //2 bit
            return serialPort->setStopBits(QSerialPort::TwoStop);
            break;
        default:
            return false;
            break;
    }
}
bool MainWindow::setPortFlowControl()
{
    switch(ui->comboBoxFlowControl->currentIndex())
    {
        case 0: //no flow control
            return serialPort->setFlowControl(QSerialPort::NoFlowControl);
            break;
        case 1: //hardware flow control
            return serialPort->setFlowControl(QSerialPort::HardwareControl);
            break;
        case 2: //software flow control
            return serialPort->setFlowControl(QSerialPort::SoftwareControl);
            break;
        default:
            return false;
            break;
    }
}
bool MainWindow::setPortBaudrate()
{
    return serialPort->setBaudRate(ui->comboBoxBoundrate->currentText().toInt());
}

//this file is all about the functions related to receive data
void MainWindow::on_checkBoxSingleLine_toggled(bool checked)
{
    if(checked == true)
    {
        ui->statusBar->showMessage(tr("Data will be displayed in single row."), 3000);
    }
    else
    {
        ui->statusBar->showMessage(tr("Data will be displayed in both line and row."), 3000);
    }
}
void MainWindow::on_checkBoxDisplayTime_toggled(bool checked)
{
    if(checked == true)
    {
        ui->statusBar->showMessage(tr("Time will be displayed."), 3000);
    }
    else
    {
        ui->statusBar->showMessage(tr("Time will not be displayed."), 3000);
    }
}

void MainWindow::on_pushButtonOscWindow_clicked()
{
    waveWindow = new WaveWindow(this);
    connect(receiveThread,SIGNAL(receivedDataByte(unsigned char)),waveWindow,SLOT(getByteData(unsigned char)));
    waveWindow->show();
    ui->statusBar->showMessage(tr("OSC window opened."), 3000);
}
void MainWindow::on_pushButtonSaveAsText_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as Text File"),"dataFile1.txt", tr("Text File (*.txt)"));
    if (!fileName.isNull())
    {
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly  | QIODevice::Text))
        {
            ui->statusBar->showMessage(tr("Failed to open file, please check!"), 3000);
        }
        QTextStream output(&file);
        output<<ui->textBrowserRecieved->document()->toPlainText();
        file.close();
        ui->statusBar->showMessage(tr("Data saved successfully in file ")+fileName, 3000);
    }
    else
    {
        ui->statusBar->showMessage(tr("File saving canceled."), 3000);
    }
}
void MainWindow::on_pushButtonClearDisplay_clicked()
{
    ui->textBrowserRecieved->clear();
    ui->statusBar->showMessage(tr("Display window cleared"), 3000);
}
void MainWindow::on_pushButtonMiniWindow_clicked()
{
    adjustWindow();
}

void MainWindow::on_comboBoxReceiveCodeSelection_activated(int index)
{
    switch(index)
    {
        case 0:
            ui->statusBar->showMessage(tr("Data will be decoded as hex number."), 3000);
            break;
        case 1:
            ui->statusBar->showMessage(tr("Data will be decoded as ASCII code."), 3000);
            break;
        case 2:
            ui->statusBar->showMessage(tr("Data will be decoded as decimal number."), 3000);
            break;
        default:
            ui->statusBar->showMessage(tr("Data will be decoded as hex number."), 3000);
            break;
    }
}

void MainWindow::refreshPortList()
{
    ui->comboBoxPort->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->comboBoxPort->addItem(info.portName());
    }

    if(ui->comboBoxPort->currentIndex() == -1)
    {
        ui->labelReceiveStatus->setText(tr("Not Listening"));
    }
    else
    {
        startListenSerialPort(ui->comboBoxPort->currentText());
    }
}
void MainWindow::displayText(const QString& text)
{
    if (ui->checkBoxDisplayTime->isChecked()==false)
    {
        if(ui->checkBoxSingleLine->isChecked() == false)
        {
            ui->textBrowserRecieved->setText(ui->textBrowserRecieved->document()->toPlainText()+text);
        }
        else
        {
            ui->textBrowserRecieved->append(text);
        }
    }
    else
    {
        if(ui->checkBoxSingleLine->isChecked() == false)
        {
            time = QTime::currentTime();
            ui->textBrowserRecieved->setText(ui->textBrowserRecieved->document()->toPlainText()+QString(" ")+time.toString("hh:mm:ss ")+text);
        }
        else
        {
            time = QTime::currentTime();
            ui->textBrowserRecieved->append(time.toString("hh:mm:ss ")+text);
        }
    }
    ui->textBrowserRecieved->moveCursor(QTextCursor::End);
}
void MainWindow::getByteData(unsigned char byteData)
{
    ui->labelReceiveCount->setText(transformCount(receiveCount++));
    QString strByteData;
    switch(ui->comboBoxReceiveCodeSelection->currentIndex())
    {
        case 0:
            strByteData.setNum(byteData,16);
            if(byteData <= 0x0F)
            {
                strByteData = QString("0")+strByteData;
            }
            displayText(strByteData.toUpper()+QString(" "));
            break;
        case 1:
            displayText(strByteData.append(QChar(byteData)));
            break;
        case 2:
            strByteData.setNum(byteData,10);
            displayText(strByteData+QString(" "));
            break;
        default:
            break;
    }
}
QString MainWindow::transformCount(float count)
{
    if(count < 1024)
    {
        return QString::number(count) + QString("B");
    }
    else if(count>=1024 && count < 1048576)
    {
        count = count/1024;
        return QString::number(count) + QString("KB");
    }
    else if(count>=1048576 && count < 1073741824)
    {
        count = count/1048576;
        return QString::number(count) + QString("MB");
    }
    else
    {
        count = count/1073741824;
        return QString::number(count) + QString("GB");
    }
}
bool MainWindow::startListenSerialPort(QString port)
{
    receiveThread->terminate();
    serialPort->close();
    serialPort->setPortName(port);
    if(serialPort->open(QIODevice::ReadWrite) == true) //打开成功
    {
        //configure port
        if(serialPort->setBaudRate(ui->comboBoxBoundrate->currentText().toInt()) == false)
        {
            return false;
        }
        if(setPortDataBit() == false)
        {
            return false;
        }
        if(setPortParity() == false)
        {
            return false;
        }
        if(setPortStopBit() == false)
        {
            return false;
        }
        if(setPortFlowControl() == false)
        {
            return false;
        }

        //start receive thread
        receiveThread->start();
        ui->statusBar->showMessage(tr("Listening port ")+port, 3000);
        ui->labelReceiveStatus->setText(tr("Listening ")+port);
        isPortValid = true;
        return true;
    }
    else
    {
        ui->statusBar->showMessage(tr("Failed to start listening! Please check hardware."), 3000);
        ui->labelReceiveStatus->setText(tr("Not Listening"));
        isPortValid = false;
        return false;
    }
}
void MainWindow::adjustWindow()
{
    if(isWindowNormal)
    {
        this->resize(480,720);
        isWindowNormal = false;
        ui->pushButtonMiniWindow->setText(tr("Norm-Window"));
        ui->actionMiniWindow->setText(tr("Normal Window"));
    }
    else
    {
        this->resize(1024,768);
        isWindowNormal = true;
        ui->pushButtonMiniWindow->setText(tr("Mini-Window"));
        ui->actionMiniWindow->setText(tr("Minimum Window"));
    }
}

//related to group box sending settings, input lineEdit and send pushButton
void MainWindow::on_comboBoxSendCodeSelection_currentIndexChanged(int index)
{
    ui->lineEditDataToSend->clear();
    switch(index)
    {
        case 0:
            ui->lineEditDataToSend->setValidator(hexValidator);
            ui->statusBar->showMessage(tr("Sending encoding set to HEX, hex number should be seprated with space such as 0A CB F6...."), 3000);
            break;
        case 1:
            ui->lineEditDataToSend->setValidator(ascValidator);
            ui->statusBar->showMessage(tr("Sending encoding set to ASCII, please input freely."));
            break;
        default:
            ui->lineEditDataToSend->setValidator(hexValidator);
            ui->statusBar->showMessage(tr("Sending encoding set to HEX, hex number should be seprated with space such as 0A CB F6...."), 3000);
            break;
    }
}

void MainWindow::on_checkBoxRepeatTrans_toggled(bool checked)
{
    if(checked == true)
    {
        ui->statusBar->showMessage(tr("Data will be circularly sent."), 3000);
    }
    else
    {
        ui->checkBoxSeparation->setChecked(false);
        ui->statusBar->showMessage(tr("Data will be sent only once."), 3000);
    }
}

void MainWindow::on_spinBoxLoopInterval_valueChanged(int arg1)
{
    ui->statusBar->showMessage(tr("circulating interval set to ")+QString::number(arg1), 3000);
}

void MainWindow::on_pushButtonSendData_clicked()
{
    if(isPortValid)
    {
        serialPort->clear();
        sendData();
    }
    else
    {
        ui->statusBar->showMessage(tr("Port is currently unavaliable!"), 3000);
    }

}

void MainWindow::on_lineEditDataToSend_returnPressed()
{
    sendData();
}
void MainWindow::on_lineEditDataToSend_textEdited(const QString &arg1)
{
    if(ui->comboBoxSendCodeSelection->currentIndex() == 0)
    {
        ui->lineEditDataToSend->setText(arg1.toUpper());
        ui->statusBar->showMessage(tr("Sending encoding set to HEX, hex number should be seprated with space such as 0A CB F6...."), 3000);
    }
}

void MainWindow::sendData()
{
    if(isPortSending == true)
    {
        sendThread->terminate();
        byteIter = 0;
    }
    else
    {
        sendCount = 0;
        ui->groupBoxSerialSettings->setDisabled(true);
        ui->groupBoxSendSettings->setDisabled(true);
        ui->lineEditDataToSend->setDisabled(true);
        //prepare data
        parseDataToSend();
        //start sending
        if(ui->checkBoxRepeatTrans->isChecked())
        {
            sendThread->setRepeatInterval(ui->spinBoxLoopInterval->text().toLong());
            sendThread->start();
            isPortSending = true;
            ui->pushButtonSendData->setText(tr("Stop"));
        }
        else
        {
            ui->pushButtonSendData->setEnabled(false);
            sendThread->setRepeatInterval(-1);
            sendThread->start();
        }
    }
}
void MainWindow::sendDataOnce()
{
    if(ui->checkBoxRepeatTrans->isChecked() && ui->checkBoxSeparation->isChecked())
    {
        if(dataToSend[byteIter] != '\0')
        {
            char tempData[2];
            tempData[0] = dataToSend[byteIter];
            tempData[1] = '\0';
            sendCount = sendCount + float(serialPort->write(tempData));
            byteIter++;
        }
        else
        {
            byteIter = 0;
        }
    }
    else
    {
        sendCount = sendCount + float(serialPort->write(dataToSend));
        ui->labelSendCount->setText(transformCount(sendCount));
    }

}

void MainWindow::sendThreadFinished()
{
    ui->groupBoxSerialSettings->setDisabled(false);
    ui->groupBoxSendSettings->setDisabled(false);
    ui->lineEditDataToSend->setDisabled(false);
    isPortSending = false;
    ui->pushButtonSendData->setText(tr("Send"));
    ui->pushButtonSendData->setDisabled(false);
}
void MainWindow::parseAsHex(char *data, QString strData)
{
    strData.remove(QChar(' '));
    int iterChar = 0;
    QString tempString;
    if(strData.length()%2 != 0)
    {
        ui->statusBar->showMessage(tr("Hex data is not correct, please check!"), 3000);
        return;
    }
    for(QString::Iterator iter = strData.begin(); iter != strData.end(); iter = iter+2)
    {
        tempString.append(*iter);
        tempString.append(*(iter+1));
        data[iterChar] = tempString.toInt(Q_NULLPTR,16);
        iterChar++;
        tempString.clear();
    }
    data[iterChar] = '\0';
}
void MainWindow::parseDataToSend()
{
    switch(ui->comboBoxSendCodeSelection->currentIndex())
    {
        case 0://HEX
            parseAsHex(dataToSend,ui->lineEditDataToSend->text());
            break;
        case 1://ASCII
            strcpy(dataToSend,ui->lineEditDataToSend->text().toStdString().c_str());
            break;
        default://HEX
            strcpy(dataToSend,ui->lineEditDataToSend->text().toStdString().c_str());
            break;
    }
}

void MainWindow::on_checkBoxSeparation_toggled(bool checked)
{
    if(checked == true)
    {
        ui->checkBoxRepeatTrans->setChecked(true);
        ui->statusBar->showMessage(tr("Data will be sent circularly and separately."), 3000);
    }
}
