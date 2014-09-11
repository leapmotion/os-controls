#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include "utility/Config.h"
#include <QMainWindow>
class QSystemTrayIcon;

namespace Ui {
class ConfigWindow;
}

class ConfigWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit ConfigWindow(QWidget *parent = 0);
  ~ConfigWindow();

public Q_SLOTS:

  void onExposeCheckChanged(int newState);
  void onMediaCheckChanged(int newState);

private:
  void createTrayIcon();

  void updateUIFromConfig();

  Ui::ConfigWindow *ui;
  QSystemTrayIcon* m_trayIcon;

  Config m_config;
};

#endif // CONFIGWINDOW_H
