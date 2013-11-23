#include "sfmwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

SFMWidget::SFMWidget(QWidget *parent) :
    QGLWidget(parent), _vizScale(1.0), _drawCameras(true)
{

    _xRot = 0;
    _yRot = 0;
    _zRot = 0;
    _cameraDistance = 25.0;

    //register QMetaTypes for Invoking
    qRegisterMetaType< std::vector<cv::Point3d> >();
    qRegisterMetaType< std::vector<cv::Vec3b> >();
    qRegisterMetaType< std::vector<cv::Matx34d> >();

}

QSize SFMWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize SFMWidget::sizeHint() const
{
    return QSize(400, 400);
}

void SFMWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != _xRot) {
        _xRot = angle;
        updateGL();
    }
}
//! [5]

void SFMWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != _yRot) {
        _yRot = angle;
        updateGL();
    }
}

void SFMWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != _zRot) {
        _zRot = angle;
        updateGL();
    }
}

void SFMWidget::update(std::vector<cv::Point3d> pcld, std::vector<cv::Vec3b> pcldrgb, std::vector<cv::Point3d> pcld_alternate, std::vector<cv::Vec3b> pcldrgb_alternate, std::vector<cv::Matx34d> cameras)
{
    //update in main thread
    QMetaObject::invokeMethod(this, "updatePointCloud", Qt::QueuedConnection,
                              Q_ARG(std::vector<cv::Point3d>, pcld),
                              Q_ARG(std::vector<cv::Vec3b>, pcldrgb),
                              Q_ARG(std::vector<cv::Point3d>, pcld_alternate),
                              Q_ARG(std::vector<cv::Vec3b>, pcldrgb_alternate),
                              Q_ARG(std::vector<cv::Matx34d>, cameras)
                              );
}

void SFMWidget::qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void SFMWidget::initializeGL()
{
    qglClearColor(QColor(Qt::gray).lighter());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void SFMWidget::paintGL()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -_cameraDistance);
    glRotatef(_xRot / 16.0, 1.0, 0.0, 0.0);
    glRotatef(_yRot / 16.0, 0.0, 1.0, 0.0);
    glRotatef(_zRot / 16.0, 0.0, 0.0, 1.0);

    glPushMatrix();
    //BUG scaling is not zooming, use qglviewer zoom function instead
    glScaled(_vizScale,_vizScale,_vizScale);
    glMultMatrixd(_globalTransform.data());

    //draw pointcloud
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glBegin(GL_POINTS);
    for (int i = 0; i < _pcld.size(); ++i) {
        glColor3ub(_pcldrgb[i][0],_pcldrgb[i][1],_pcldrgb[i][2]);
        glVertex3dv(&(_pcld[i].x));
    }
    glEnd();

    //draw cameras
    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_LIGHTING);
    if(_drawCameras) {
        for(const Eigen::Affine3d& affine : _camerasTransforms) {

            glPushMatrix();
            glMultMatrixd(affine.data());

            //TODO write a function for this
            glColor4f(1, 0, 0, 1);
            drawArrow(QVector3D(0,0,0), QVector3D(3,0,0));
            glColor4f(0, 1, 0, 1);
            drawArrow(QVector3D(0,0,0), QVector3D(0,3,0));
            glColor4f(0, 0, 1, 1);
            drawArrow(QVector3D(0,0,0), QVector3D(0,0,3));

            glPopMatrix();
        }
    }

    glPopAttrib();
    glPopMatrix();
}

void SFMWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //BUG rethink values
    glOrtho(-50.0, +50.0, -50.0, +50.0, 0.1, +50.0);

    glMatrixMode(GL_MODELVIEW);
}

void SFMWidget::mousePressEvent(QMouseEvent *e)
{
    _lastPos = e->pos();
}

void SFMWidget::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->x() - _lastPos.x();
    int dy = e->y() - _lastPos.y();

    if (e->buttons() & Qt::LeftButton) {
        setXRotation(_xRot + 8 * dy);
        setYRotation(_yRot + 8 * dx);
    } else if (e->buttons() & Qt::RightButton) {
        setXRotation(_xRot + 8 * dy);
        setZRotation(_zRot + 8 * dx);
    }
    _lastPos = e->pos();
}

void SFMWidget::wheelEvent(QWheelEvent *e)
{

    const float zoomFactor = 0.8;
    //BUG this does not work as expected
    _cameraDistance += e->angleDelta().y()/120.0 *zoomFactor;
    updateGL();

}

void SFMWidget::updatePointCloud(std::vector<cv::Point3d> pcld, std::vector<cv::Vec3b> pcldrgb, std::vector<cv::Point3d> pcld_alternate, std::vector<cv::Vec3b> pcldrgb_alternate, std::vector<cv::Matx34d> cameras)
{
    _pcld = pcld;
    _pcldrgb = pcldrgb;
    _cameras = cameras;

    //get the scale of the result cloud using PCA
    {
        cv::Mat_<double> cldm(pcld.size(), 3);
        for (unsigned int i = 0; i < pcld.size(); i++) {
            cldm.row(i)(0) = pcld[i].x;
            cldm.row(i)(1) = pcld[i].y;
            cldm.row(i)(2) = pcld[i].z;
        }
        cv::Mat_<double> mean; //cv::reduce(cldm,mean,0,CV_REDUCE_AVG);
        cv::PCA pca(cldm, mean, CV_PCA_DATA_AS_ROW);
        _scaleCamerasDown = 1.0 / (3.0 * sqrt(pca.eigenvalues.at<double> (0)));
//		std::cout << "emean " << mean << std::endl;
//		m_global_transform = Eigen::Translation<double,3>(-Eigen::Map<Eigen::Vector3d>(mean[0]));
    }

    //compute transformation to place cameras in world
    _camerasTransforms.resize(_cameras.size());
    Eigen::Vector3d c_sum(0,0,0);
    for (int i = 0; i < _cameras.size(); ++i) {
        Eigen::Matrix<double, 3, 4> P = Eigen::Map<Eigen::Matrix<double, 3, 4,
                Eigen::RowMajor> >(_cameras[i].val);
        Eigen::Matrix3d R = P.block(0, 0, 3, 3);
        Eigen::Vector3d t = P.block(0, 3, 3, 1);
        Eigen::Vector3d c = -R.transpose() * t;
        c_sum += c;
        _camerasTransforms[i] =
                Eigen::Translation<double, 3>(c) *
                Eigen::Quaterniond(R) *
                Eigen::UniformScaling<double>(_scaleCamerasDown);
    }

    _globalTransform = Eigen::Translation<double,3>(-c_sum / (double)(_cameras.size()));
//	m_global_transform = m_cameras_transforms[0].inverse();

    updateGL();
}

void SFMWidget::setScale(int scaleValue)
{
    _vizScale = static_cast<float>(scaleValue);
    updateGL();
}

void SFMWidget::clearData()
{
    _pcld.clear();
    _pcldrgb.clear();
    _cameras.clear();
    _camerasTransforms.clear();
    _vizScale = 1.0;
    updateGL();
}

void SFMWidget::drawArrow(float length, float radius, int nbSubdivisions)
{
    static GLUquadric* quadric = gluNewQuadric();

    if (radius < 0.0)
        radius = 0.05 * length;

    const float head = 2.5*(radius / length) + 0.1;
    const float coneRadiusCoef = 4.0 - 5.0 * head;

    gluCylinder(quadric, radius, radius, length * (1.0 - head/coneRadiusCoef), nbSubdivisions, 1);
    glTranslatef(0.0, 0.0, length * (1.0 - head));
    gluCylinder(quadric, coneRadiusCoef * radius, 0.0, head * length, nbSubdivisions, 1);
    glTranslatef(0.0, 0.0, -length * (1.0 - head));
}

void SFMWidget::drawArrow(const QVector3D& from, const QVector3D& to, float radius, int nbSubdivisions)
{
    glPushMatrix();
    glTranslatef(from.x(),from.y(),from.z());
    const QVector3D dir = to-from;
    //BUG replace this calculation with Eigen types
//    glMultMatrixd(Quaternion(Vec(0,0,1), dir).matrix());
    drawArrow(dir.length(), radius, nbSubdivisions);
    glPopMatrix();
}
