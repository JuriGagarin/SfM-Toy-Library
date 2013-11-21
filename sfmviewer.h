/*
 *  sfmviewer.h
 *  SfMToyLibrary
 *
 *  Created by Roy Shilkrot on 11/3/13.
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2013 Roy Shilkrot
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
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
