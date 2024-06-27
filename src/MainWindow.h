#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "./CallController/CallController.h"
#include "./utils/NV12Widget.h"
#include "./utils/Yuv420Widget.h"
#include <QDebug>
#include <QLayout>
#include <QMainWindow>
#include <QTimer>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(std::unique_ptr<CallController> callController,
               int applicationPort, QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // handle button
    void onBtnStartCallClicked();
    void onBtnStopCallClicked();
    void renderInfo();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<Yuv420Widget> _partnerRenderWidget;
    std::shared_ptr<NV12Widget> _localRenderWidget;
    std::unique_ptr<CallController> _callController;
    int applicationPort;
    int partnerPort;
    QTimer *infoTimer;
};
#endif // MAINWINDOW_H
