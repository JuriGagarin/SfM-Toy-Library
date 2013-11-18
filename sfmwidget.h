#ifndef SFMWIDGET_H
#define SFMWIDGET_H

#include <QObject>
#include <QGLViewer/qglviewer.h>
#include <QDebug>
#include <Eigen/Eigen>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "SfMUpdateListener.h"

Q_DECLARE_METATYPE(std::vector<cv::Point3d>)
Q_DECLARE_METATYPE(std::vector<cv::Vec3b>)
Q_DECLARE_METATYPE(std::vector<cv::Matx34d>)

class SFMWidget : public QGLViewer, public SfMUpdateListener
{
    Q_OBJECT
public:
    explicit SFMWidget(QWidget *parent = 0);

    virtual void update(std::vector<cv::Point3d> pcld,
            std::vector<cv::Vec3b> pcldrgb,
            std::vector<cv::Point3d> pcld_alternate,
            std::vector<cv::Vec3b> pcldrgb_alternate,
            std::vector<cv::Matx34d> cameras);
protected:
    virtual void draw();

private:
    float _vizScale;
    double _scaleCamerasDown;
    Eigen::Affine3d _globalTransform;
    std::vector<cv::Point3d> _pcld;
    std::vector<cv::Vec3b> _pcldrgb;
    std::vector<cv::Matx34d> _cameras;
    std::vector<Eigen::Affine3d> _camerasTransforms;

signals:

public slots:
    void updatePointCloud(std::vector<cv::Point3d> pcld,
                          std::vector<cv::Vec3b> pcldrgb,
                          std::vector<cv::Point3d> pcld_alternate,
                          std::vector<cv::Vec3b> pcldrgb_alternate,
                          std::vector<cv::Matx34d> cameras);
    void setScale(int scaleValue);
    void clearData();

};

#endif // SFMWIDGET_H
