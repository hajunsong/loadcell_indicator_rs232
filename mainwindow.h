#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "serialcomm.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // button event
    void btnConnectClicked();

    // timer event
    void updateTimeout();

    void readyRead();

private:
    Ui::MainWindow *ui;

    SerialComm *comm;

    bool connectState;

    QTimer *updateTimer;
};
#endif // MAINWINDOW_H
