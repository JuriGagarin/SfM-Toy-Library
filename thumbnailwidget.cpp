#include "thumbnailwidget.h"
#include "converter.h"

ThumbnailWidget::ThumbnailWidget(QWidget *parent) :
    QListWidget(parent)
{
    setViewMode(QListView::IconMode);
    //TODO do not set fixed values
    _iconSize = QSize(200,200);
    setIconSize(_iconSize);
    setResizeMode(QListView::Adjust);
}

void ThumbnailWidget::setImages(const std::vector<cv::Mat> &images, const std::vector<std::string> &imageNames)
{
    clear();
    if(images.size() == imageNames.size()) {
        for(int i = 0; i < images.size(); i++) {
            addItem(new QListWidgetItem(QIcon(QPixmap::fromImage(Converter::toQImage(images.at(i)))), QString::fromStdString(imageNames.at(i))));
        }
    }
}
