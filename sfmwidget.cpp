#include "sfmwidget.h"

SFMWidget::SFMWidget(QWidget *parent) :
    QGLViewer(parent), _vizScale(1.0), _drawCameras(true)
{

    //register QMetaTypes for Invoking
    qRegisterMetaType< std::vector<cv::Point3d> >();
    qRegisterMetaType< std::vector<cv::Vec3b> >();
    qRegisterMetaType< std::vector<cv::Matx34d> >();

//    restoreStateFromFile();//TODO Restore previous viewer state?
    setSceneBoundingBox(qglviewer::Vec(-50,-50,-50), qglviewer::Vec(50,50,50));
    showEntireScene();
    setFPSIsDisplayed(true);

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

void SFMWidget::draw()
{
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

            glColor4f(1, 0, 0, 1);
            QGLViewer::drawArrow(qglviewer::Vec(0,0,0), qglviewer::Vec(3,0,0));
            glColor4f(0, 1, 0, 1);
            QGLViewer::drawArrow(qglviewer::Vec(0,0,0), qglviewer::Vec(0,3,0));
            glColor4f(0, 0, 1, 1);
            QGLViewer::drawArrow(qglviewer::Vec(0,0,0), qglviewer::Vec(0,0,3));

            glPopMatrix();
        }
    }

    glPopAttrib();
    glPopMatrix();
}

void SFMWidget::mousePressEvent(QMouseEvent *e)
{
    if ((e->button() == Qt::RightButton) && (e->modifiers() == Qt::NoButton)) {
        //TODO add custom menu here
        qDebug() << "Right mouse button";
        e->accept();
    } else {
        QGLViewer::mousePressEvent(e);
    }
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
