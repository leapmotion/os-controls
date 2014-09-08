#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

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

private:
  void createTrayIcon();

  Ui::ConfigWindow *ui;
  QSystemTrayIcon* m_trayIcon;
};

#endif // CONFIGWINDOW_H
