#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <QListWidget>
#include <opencv/cv.h>
#include <QDebug>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QMimeData>
#include <QFileInfo>

class ThumbnailWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ThumbnailWidget(QWidget *parent = 0);
    void setImages(const std::vector<cv::Mat>& images, const std::vector<std::string>& imageNames);
signals:
    void directoryDropped(QString dir);

public slots:

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *e);
    virtual void dragLeaveEvent(QDragLeaveEvent *e);
    virtual void dropEvent(QDropEvent *event);
    QSize _iconSize;

};

#endif // THUMBNAILWIDGET_H
