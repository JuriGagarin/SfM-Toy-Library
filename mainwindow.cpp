#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    const bool gpuSupport = isGPUSupported();
    _ui->checkBox_useGPU->setChecked(gpuSupport);
    _ui->checkBox_useGPU->setEnabled(gpuSupport);


    _distance = new MultiCameraPnP();
    _distance->attach(_ui->sfmWidget);

}

MainWindow::~MainWindow()
{
    //TODO stop recovery thread
    if(_recoveryThread.joinable()) {
        _recoveryThread.join();
    }
    delete _ui;
}

void MainWindow::on_pushButton_runSFM_clicked()
{
    _distance->use_gpu = _ui->checkBox_useGPU->isChecked();
    _distance->use_rich_features = _ui->checkBox_useRich->isChecked();
    qDebug() << "run SFM";
    _ui->sfmWidget->clearData();
//    std::thread recoveryThread(_distance->RecoverDepthFromImages);

    _recoveryThread = std::thread([&]() {_distance->RecoverDepthFromImages();});

//    t.join();
//    _distance->RecoverDepthFromImages();
}

void MainWindow::on_pushButton_openDirectory_clicked()
{
    openDirectory();
}

void MainWindow::on_actionOpen_triggered()
{
    openDirectory();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionAbout_triggered()
{
    //TODO write a useful description
    QMessageBox::about(this, "SfMToyViewer", "SfMToy Lib ...");
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::openDirectory()
{
    _images.clear();
    _imageNames.clear();

    std::string imgs_path = QFileDialog::getExistingDirectory(this, tr("Open Images Directory"), ".").toStdString();

    const double scale_factor = _ui->doubleSpinBox_scaleFactor->value();

    qDebug() << "Downscale image to: " << scale_factor;

    open_imgs_dir(imgs_path.c_str(), _images, _imageNames, scale_factor);

    if(_images.empty()) {
        qWarning() << "can't get image files";
        _ui->pushButton_runSFM->setEnabled(false);
    } else {
        _distance->setImages(_images,_imageNames,imgs_path);
        _ui->pushButton_runSFM->setEnabled(true);
    }


}

bool MainWindow::isGPUSupported()
{
#ifdef HAVE_OPENCV_GPU
    return true;
#endif
    return false;
}
