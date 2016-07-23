#include "aboutwindow.h"
#include "ui_aboutwindow.h"

AboutWindow::AboutWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AboutWindow)
{
    ui->setupUi(this);
    setVersionRelatedSyle();
    setAttribute(Qt::WA_DeleteOnClose);
}

AboutWindow::~AboutWindow()
{
    delete ui;
}

//界面处理
void AboutWindow::setVersionRelatedSyle()
{
    if(QSysInfo::windowsVersion() > QSysInfo::WV_WINDOWS7)
    {
        this->setStyleSheet("AboutWindow{background-color: rgb(255, 255, 255)}");
    }
}
