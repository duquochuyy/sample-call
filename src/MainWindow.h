#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "./CallController/CallController.h"
#include <memory>
#include <qDebug>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(std::shared_ptr<CallController> callController, int applicationPort, QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // handle button
    void onBtnStartCallClicked();
    void onBtnStopCallClicked();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<CallController> _callController;
    int applicationPort;
    int partnerPort;
};
#endif // MAINWINDOW_H
