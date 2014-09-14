#include "configwindow.h"
#include "ui_configwindow.h"

#include <qsystemtrayicon.h>

ConfigWindow::ConfigWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConfigWindow)
{
  m_config.Load();
  
  ui->setupUi(this);
  createTrayIcon();

  m_trayIcon->show();

  connect(ui->exposeCheckbox, &QCheckBox::stateChanged, this, &ConfigWindow::onExposeCheckChanged);
  connect(ui->mediaControlsCheckbox, &QCheckBox::stateChanged, this, &ConfigWindow::onMediaCheckChanged);
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

void ConfigWindow::onExposeCheckChanged(int newState){
  m_config.Set("exposeEnabled", newState);
  m_config.Save();
}

void ConfigWindow::onMediaCheckChanged(int newState){
  m_config.Set("mediaEnabled", newState);
  m_config.Save();
}