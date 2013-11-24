#ifndef CONVERTER_H
#define CONVERTER_H

#include <QImage>
#include <opencv2/opencv.hpp>


class Converter
{
public:
/**
 * @brief toQImage http://stackoverflow.com/a/17137998/1130262
 * @param src
 * @return
 */
static QImage toQImage(cv::Mat const& src)
{
    //TODO color check
     cv::Mat temp; // make the same cv::Mat
     cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
     QImage dest((uchar*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
     QImage dest2(dest);
     dest2.detach(); // enforce deep copy
     return dest2;
}

/**
 * @brief toCvMat http://stackoverflow.com/a/17137998/1130262
 * @param src
 * @return
 */
static cv::Mat toCvMat(QImage const& src)
{
    //todo color check
     cv::Mat tmp(src.height(),src.width(),CV_8UC3,(uchar*)src.bits(),src.bytesPerLine());
     cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
     cvtColor(tmp, result,CV_BGR2RGB);
     return result;
}

};

#endif // CONVERTER_H
