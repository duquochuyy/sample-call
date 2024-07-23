#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QLayout>
#include <QMainWindow>
#include <QTimer>
#include <memory>

#include "./CallController/CallController.h"
#include "./VideoRender/CustomWidget/YuvWidget.h"

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
    void setUpOption();

   private:
    Ui::MainWindow *ui;
    std::shared_ptr<YuvWidget> _partnerRenderWidget;
    std::shared_ptr<YuvWidget> _localRenderWidget;
    std::unique_ptr<CallController> _callController;
    int applicationPort;
    int partnerPort;
    int codec;
    QTimer *infoTimer;
};
#endif  // MAINWINDOW_H
