#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <QListWidget>
#include <opencv/cv.h>

class ThumbnailWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ThumbnailWidget(QWidget *parent = 0);
    void setImages(const std::vector<cv::Mat>& images, const std::vector<std::string>& imageNames);
signals:

public slots:

protected:
    QSize _iconSize;

};

#endif // THUMBNAILWIDGET_H
