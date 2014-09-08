#include "configwindow.h"
#include "ui_configwindow.h"

#include <qsystemtrayicon.h>

ConfigWindow::ConfigWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConfigWindow)
{
    ui->setupUi(this);
    createTrayIcon();
}

ConfigWindow::~ConfigWindow()
{
    delete ui;
}

void ConfigWindow::createTrayIcon(){
  m_trayIcon = new QSystemTrayIcon(this);

}