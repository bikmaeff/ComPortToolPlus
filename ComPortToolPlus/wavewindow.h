#ifndef WAVEWINDOW_H
#define WAVEWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChart>
#include <QTime>

#include "qcustomplot.h"
#include "qthinkgear.h"

namespace Ui {
class WaveWindow;
}

class WaveWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WaveWindow(QWidget *parent = 0);
    ~WaveWindow();

private slots:
    void getByteData(char data);

    void on_comboBoxProtocalSelector_activated(int index);
    void on_horizontalSliderXResolution_valueChanged(int value);

    void on_pushButtonCloseAutoScale_clicked();

    void on_comboBoxSignalType_currentIndexChanged(int index);

private:
    void setVersionRelatedSyle();
    void setUpPlayground();
    void initiaions();
    void addWaveData(double data);

private:
    int currentProtocal = 0;
    Ui::WaveWindow *ui;
    float xResolution;
    float yRangeMax;
    float yRangeMin;
    bool isAutoScale;
    bool isDiscOpen;

    QThinkGear thinkGear;
};

#endif // WAVEWINDOW_H
