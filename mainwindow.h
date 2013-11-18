#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <memory>
#include <thread>

#include "MultiCameraPnP.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void openDirectory();
    static bool isGPUSupported();
private slots:
    void on_pushButton_runSFM_clicked();

    void on_actionAbout_Qt_triggered();

    void on_actionExit_triggered();

    void on_pushButton_openDirectory_clicked();

    void on_actionOpen_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *_ui;

    MultiCameraPnP* _distance;

    std::vector<cv::Mat> 			_images;
    std::vector<std::string> 		_imageNames;
    std::thread _recoveryThread;

};

#endif // MAINWINDOW_H
