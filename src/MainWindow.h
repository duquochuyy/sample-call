#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "./CallController/CallController.h"

#include <memory>

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
    MainWindow(std::shared_ptr<CallController> callController, QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // handle button
    void onBtnStartCameraClicked();
    void onBtnStopCameraClicked();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<CallController> _callController;
};
#endif // MAINWINDOW_H
