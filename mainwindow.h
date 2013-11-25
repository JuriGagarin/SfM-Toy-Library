#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <memory>
#include <thread>

#include "MultiCameraPnP.h"
#include <opencv2/opencv.hpp>
#include <Eigen/Eigen>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //TODO move these functions to a more suitable class
    static bool isGPUSupported();
    static bool isOpenMPEnabled();
    static bool isSSBAEnabled();


    static QString convertToString(bool value);

public slots:
    void setStatusBarText(QString text);
    void browseDirectory();
    void openDirectory(QString path);

private slots:
    void on_pushButton_runSFM_clicked();

    void on_actionAbout_Qt_triggered();

    void on_actionExit_triggered();

    void on_actionAbout_triggered();

    void setNrOfImages(int nr);

private:
    Ui::MainWindow *_ui;

    MultiCameraPnP* _distance;

    std::vector<cv::Mat> 			_images;
    std::vector<std::string> 		_imageNames;
    std::thread _recoveryThread;

};

#endif // MAINWINDOW_H
