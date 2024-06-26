#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "./CallController/CallController.h"
#include "./Utils/YuvWidget.h"
#include <memory>
#include <QDebug>
#include <QTimer>
#include <QLayout>

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
    MainWindow(std::unique_ptr<CallController> callController, int applicationPort, QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // handle button
    void onBtnStartCallClicked();
    void onBtnStopCallClicked();
    void renderInfo();

private:
    Ui::MainWindow *ui;
    // YuvWidget *videoDisplay;
    std::unique_ptr<CallController> _callController;
    int applicationPort;
    int partnerPort;
    QTimer *infoTimer;
};
#endif // MAINWINDOW_H
