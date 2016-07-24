#include "wavewindow.h"
#include "ui_wavewindow.h"

WaveWindow::WaveWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WaveWindow)
{
    ui->setupUi(this);

    setVersionRelatedSyle();
    setAttribute(Qt::WA_DeleteOnClose);

    setUpPlayground();
}

WaveWindow::~WaveWindow()
{
    delete ui;
}

//界面处理
void WaveWindow::setVersionRelatedSyle()
{
    if(QSysInfo::windowsVersion() > QSysInfo::WV_WINDOWS7)
    {
        this->setStyleSheet("WaveWindow{background-color: rgb(255, 255, 255)}");
    }
}

//槽函数
void WaveWindow::on_comboBoxProtocalSelector_activated(int index)
{
    // 0 equals no protocal applied
    currentProtocal = index;
}
void WaveWindow::getByteData(char data)
{

    //apply protocal to parse data
    switch (currentProtocal)
    {
        case 0: //No protocal
            addWaveData((unsigned char) (data));//add a wave data directly
            break;
        case 1:
            thinkGear.addByteToPackage(data);
            if(thinkGear.isNewRawReady())
            {
                addWaveData(thinkGear.getRaw());
            }
            break;
        default:
            break;
    }
}
void WaveWindow::addWaveData(double data)
{
    double key= QDateTime::currentDateTime().toMSecsSinceEpoch()/1000;
    static double lastPointKey = 0;
    if(key - lastPointKey > 0.0001)//Frequency resolution, 0.0001 means upto 1KHz. Gui performance will be low if too high
    {
        if(isDiscOpen == false)
        {
            ui->widgetQCustomPlot->graph(1)->clearData();
        }

        ui->widgetQCustomPlot->graph(0)->addData(key,double(data));
        ui->widgetQCustomPlot->graph(1)->addData(key,double(data));

        ui->widgetQCustomPlot->graph(0)->removeDataBefore(key - xResolution);
        ui->widgetQCustomPlot->graph(1)->removeDataBefore(key - xResolution);

        if(isAutoScale)
        {
            ui->widgetQCustomPlot->yAxis->rescale(true);
        }
        lastPointKey = key;
    }
    ui->widgetQCustomPlot->xAxis->setRange(key+1.0/xResolution,xResolution,Qt::AlignRight);
    ui->widgetQCustomPlot->replot();
}

void WaveWindow::setUpPlayground()
{
    xResolution = 50;
    yRangeMin = 0;
    yRangeMax = 300;
    isAutoScale = true;
    isDiscOpen = true;

    ui->widgetQCustomPlot->addGraph();
    ui->widgetQCustomPlot->graph(0)->setPen(QPen(QColor(46,117,182)));
    ui->widgetQCustomPlot->graph(0)->setLineStyle(QCPGraph::lsImpulse);
    ui->widgetQCustomPlot->graph(0)->setAntialiasedFill(false);

    ui->widgetQCustomPlot->addGraph();
    ui->widgetQCustomPlot->graph(1)->setPen(QPen(QColor(46,117,182)));
    ui->widgetQCustomPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->widgetQCustomPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);
    ui->widgetQCustomPlot->yAxis->setRange(yRangeMin,yRangeMax);

    ui->widgetQCustomPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->widgetQCustomPlot->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->widgetQCustomPlot->xAxis->setAutoTickStep(true);
    ui->widgetQCustomPlot->axisRect()->setupFullAxesBox();

    ui->widgetQCustomPlot->axisRect()->setRangeDrag(Qt::Vertical);
    ui->widgetQCustomPlot->axisRect()->setRangeZoom(Qt::Vertical);
    ui->widgetQCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}


void WaveWindow::on_horizontalSliderXResolution_valueChanged(int value)
{
    xResolution = float(value);
}


void WaveWindow::on_pushButtonCloseAutoScale_clicked()
{
    if(isAutoScale == true)
    {
        isAutoScale = false;
        ui->pushButtonCloseAutoScale->setText(tr("Enable Auto Scale"));
    }
    else
    {
        isAutoScale = true;
        ui->pushButtonCloseAutoScale->setText(tr("Disable Auto Scale"));
    }
}

void WaveWindow::on_comboBoxSignalType_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0:
            ui->widgetQCustomPlot->graph(0)->setLineStyle(QCPGraph::lsImpulse);
            isDiscOpen = true;
            break;
        case 1:
            ui->widgetQCustomPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
            isDiscOpen = false;
            break;
    }
}
