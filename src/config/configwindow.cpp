#include "configwindow.h"
#include "ui_configwindow.h"

#include <qsystemtrayicon.h>

ConfigWindow::ConfigWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConfigWindow)
{
    ui->setupUi(this);
    createTrayIcon();

    m_trayIcon->show();
}

ConfigWindow::~ConfigWindow()
{
  delete ui;
  delete m_trayIcon;
}

void ConfigWindow::createTrayIcon(){
  m_trayIcon = new QSystemTrayIcon(this);

  //set icon here
  m_trayIcon->setToolTip(tr("Leap OS Controls Settings"));

}