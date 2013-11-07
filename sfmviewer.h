/*
 *  sfmviewer.h
 *  SfMToyLibrary
 *
 *  Created by Roy Shilkrot on 11/3/13.
 *  Copyright 2013 MIT. All rights reserved.
 *
 */
#pragma once


#include <QGLViewer/qglviewer.h>
#include <QFileDialog>
#include <QLineEdit>
#include <QThreadPool>
#include <Eigen/Eigen>

#include "MultiCameraPnP.h"

Q_DECLARE_METATYPE(std::vector<cv::Point3d>)
Q_DECLARE_METATYPE(std::vector<cv::Vec3b>)
Q_DECLARE_METATYPE(std::vector<cv::Matx34d>)

void open_imgs_dir(const char* dir_name, std::vector<cv::Mat>& images, std::vector<std::string>& images_names, double downscale_factor);

class SFMViewer : public QGLViewer, public SfMUpdateListener, public QRunnable
{
	Q_OBJECT
	cv::Ptr<MultiCameraPnP> 		distance;

	std::vector<cv::Mat> 			images;
	std::vector<std::string> 		images_names;
	std::vector<cv::Point3d> 		m_pcld;
	std::vector<cv::Vec3b> 			m_pcldrgb;
	std::vector<cv::Matx34d> 		m_cameras;
	std::vector<Eigen::Affine3d> 	m_cameras_transforms;
	Eigen::Affine3d 				m_global_transform;

//	QThreadPool 					qtp;

	float 							vizScale;
	double 							scale_cameras_down;

protected :
	virtual void draw();
	virtual void init();

public:
    SFMViewer(QWidget *parent = 0);
    ~SFMViewer();

    virtual void update(std::vector<cv::Point3d> pcld,
			std::vector<cv::Vec3b> pcldrgb,
			std::vector<cv::Point3d> pcld_alternate,
			std::vector<cv::Vec3b> pcldrgb_alternate,
			std::vector<cv::Matx34d> cameras);


    void run();

public slots:
    void openDirectory();
    void setUseRichFeatures(bool b);
    void setUseGPU(bool b);
    void runSFM();
    void setVizScale(int i);
    void updatePointCloud(std::vector<cv::Point3d> pcld,
                          std::vector<cv::Vec3b> pcldrgb,
                          std::vector<cv::Point3d> pcld_alternate,
                          std::vector<cv::Vec3b> pcldrgb_alternate,
                          std::vector<cv::Matx34d> cameras);
};
