#ifndef SFMWIDGET_H
#define SFMWIDGET_H

#include <QObject>
#include <QVector3D>
#include <QDebug>
#include <QGLWidget>
#include <QMouseEvent>
#include <Eigen/Eigen>
#include <GL/glu.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "SfMUpdateListener.h"

Q_DECLARE_METATYPE(std::vector<cv::Point3d>)
Q_DECLARE_METATYPE(std::vector<cv::Vec3b>)
Q_DECLARE_METATYPE(std::vector<cv::Matx34d>)


//TODO display FPS
//TODO display nr of displayed cameras
//TODO display nr of reconstructed points
//TODO cleanup!

class SFMWidget : public QGLWidget, public SfMUpdateListener
{
    Q_OBJECT
public:
    explicit SFMWidget(QWidget *parent = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    virtual void update(std::vector<cv::Point3d> pcld,
            std::vector<cv::Vec3b> pcldrgb,
            std::vector<cv::Point3d> pcld_alternate,
            std::vector<cv::Vec3b> pcldrgb_alternate,
            std::vector<cv::Matx34d> cameras);


    static void qNormalizeAngle(int &angle);

    static void drawArrow(const QVector3D &from, const QVector3D &to, float radius=-1.0f, int nbSubdivisions=12);
    static void drawArrow(float length=1.0f, float radius=-1.0f, int nbSubdivisions=12);
protected:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int width, int height);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void wheelEvent(QWheelEvent *e);

private:
    int _xRot;
    int _yRot;
    int _zRot;
    float _cameraDistance;
    QPoint _lastPos;
    float _vizScale;
    double _scaleCamerasDown;
    bool _drawCameras;
    Eigen::Affine3d _globalTransform;
    std::vector<cv::Point3d> _pcld;
    std::vector<cv::Vec3b> _pcldrgb;
    std::vector<cv::Matx34d> _cameras;
    std::vector<Eigen::Affine3d> _camerasTransforms;

signals:

public slots:

    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

    void updatePointCloud(std::vector<cv::Point3d> pcld,
                          std::vector<cv::Vec3b> pcldrgb,
                          std::vector<cv::Point3d> pcld_alternate,
                          std::vector<cv::Vec3b> pcldrgb_alternate,
                          std::vector<cv::Matx34d> cameras);
    Q_DECL_DEPRECATED void setScale(int scaleValue);
    void clearData();

};

#endif // SFMWIDGET_H
