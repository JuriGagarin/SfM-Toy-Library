/*
 *  sfmviewer.cpp
 *  SfMToyLibrary
 *
 *  Created by Roy Shilkrot on 11/3/13.
 *  Copyright 2013 MIT. All rights reserved.
 *
 */

#include "sfmviewer.h"


SFMViewer::SFMViewer(QWidget *parent):QGLViewer(QGLFormat::defaultFormat(),parent),vizScale(1.0) {
    distance = new MultiCameraPnP();
    distance->attach(this);
    m_global_transform = Eigen::Affine3d::Identity();

    //register QMetaTypes for Invoking
    qRegisterMetaType< std::vector<cv::Point3d> >();
    qRegisterMetaType< std::vector<cv::Vec3b> >();
    qRegisterMetaType< std::vector<cv::Matx34d> >();
}

SFMViewer::~SFMViewer()
{
    saveStateToFile();
}

void SFMViewer::update(std::vector<cv::Point3d> pcld,
		std::vector<cv::Vec3b> pcldrgb,
		std::vector<cv::Point3d> pcld_alternate,
		std::vector<cv::Vec3b> pcldrgb_alternate,
        std::vector<cv::Matx34d> cameras)
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

void SFMViewer::run()
{
    distance->RecoverDepthFromImages();
}

void SFMViewer::openDirectory() {
    images.clear();images_names.clear();
    std::string imgs_path = QFileDialog::getExistingDirectory(this, tr("Open Images Directory"), ".").toStdString();
    double scale_factor = 1.0;
    QLineEdit* l = parentWidget()->findChild<QLineEdit*>("lineEdit_scaleFactor");
    if(l) {
        scale_factor = l->text().toFloat();
        std::cout << "downscale to " << scale_factor << std::endl;
    }
    open_imgs_dir(imgs_path.c_str(),images,images_names,scale_factor);
    if(images.size() == 0) {
        std::cerr << "can't get image files" << std::endl;
    } else {
        distance->setImages(images,images_names,imgs_path);
    }
}

void SFMViewer::setUseRichFeatures(bool b)
{
    distance->use_rich_features = b;
}

void SFMViewer::setUseGPU(bool b)
{
    distance->use_gpu = b;
}

void SFMViewer::runSFM() {
    this->setAutoDelete(false);
    m_pcld.clear();
    m_pcldrgb.clear();
    m_cameras.clear();
    m_cameras_transforms.clear();
    QThreadPool::globalInstance()->start(this);
}

void SFMViewer::setVizScale(int i) {
    vizScale = static_cast<float>(i);
    updateGL();
}

void SFMViewer::updatePointCloud(std::vector<cv::Point3d> pcld, std::vector<cv::Vec3b> pcldrgb, std::vector<cv::Point3d> pcld_alternate, std::vector<cv::Vec3b> pcldrgb_alternate, std::vector<cv::Matx34d> cameras)
{
    m_pcld = pcld;
    m_pcldrgb = pcldrgb;
    m_cameras = cameras;

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
        scale_cameras_down = 1.0 / (3.0 * sqrt(pca.eigenvalues.at<double> (0)));
//		std::cout << "emean " << mean << std::endl;
//		m_global_transform = Eigen::Translation<double,3>(-Eigen::Map<Eigen::Vector3d>(mean[0]));
    }

    //compute transformation to place cameras in world
    m_cameras_transforms.resize(m_cameras.size());
    Eigen::Vector3d c_sum(0,0,0);
    for (int i = 0; i < m_cameras.size(); ++i) {
        Eigen::Matrix<double, 3, 4> P = Eigen::Map<Eigen::Matrix<double, 3, 4,
                Eigen::RowMajor> >(m_cameras[i].val);
        Eigen::Matrix3d R = P.block(0, 0, 3, 3);
        Eigen::Vector3d t = P.block(0, 3, 3, 1);
        Eigen::Vector3d c = -R.transpose() * t;
        c_sum += c;
        m_cameras_transforms[i] =
                Eigen::Translation<double, 3>(c) *
                Eigen::Quaterniond(R) *
                Eigen::UniformScaling<double>(scale_cameras_down);
    }

    m_global_transform = Eigen::Translation<double,3>(-c_sum / (double)(m_cameras.size()));
//	m_global_transform = m_cameras_transforms[0].inverse();

    updateGL();
}

void SFMViewer::draw() {

    glPushMatrix();
    glScaled(vizScale,vizScale,vizScale);
    glMultMatrixd(m_global_transform.data());

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glBegin(GL_POINTS);
    for (int i = 0; i < m_pcld.size(); ++i) {
        glColor3b(m_pcldrgb[i][0],m_pcldrgb[i][1],m_pcldrgb[i][2]);
        glVertex3dv(&(m_pcld[i].x));
    }
    glEnd();

    //	glScaled(scale_cameras_down,scale_cameras_down,scale_cameras_down);
    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_LIGHTING);
    for (int i = 0; i < m_cameras_transforms.size(); ++i) {

        glPushMatrix();
        glMultMatrixd(m_cameras_transforms[i].data());

        glColor4f(1, 0, 0, 1);
        QGLViewer::drawArrow(qglviewer::Vec(0,0,0), qglviewer::Vec(3,0,0));
        glColor4f(0, 1, 0, 1);
	    QGLViewer::drawArrow(qglviewer::Vec(0,0,0), qglviewer::Vec(0,3,0));
        glColor4f(0, 0, 1, 1);
	    QGLViewer::drawArrow(qglviewer::Vec(0,0,0), qglviewer::Vec(0,0,3));

	    glPopMatrix();
	}

	glPopAttrib();
	glPopMatrix();
}

void SFMViewer::init() {
	// Restore previous viewer state.
	restoreStateFromFile();

    setFPSIsDisplayed();

	setSceneBoundingBox(qglviewer::Vec(-50,-50,-50), qglviewer::Vec(50,50,50));

	showEntireScene();
}


