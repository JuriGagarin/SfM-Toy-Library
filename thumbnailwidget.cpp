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

    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DropOnly);
    setSelectionMode(QAbstractItemView::SingleSelection);

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

void ThumbnailWidget::dragEnterEvent(QDragEnterEvent *event)
{
    //TODO display drop indicator
    qDebug() << "Drag enter event";
    event->acceptProposedAction();
}

void ThumbnailWidget::dragMoveEvent(QDragMoveEvent *e)
{
    qDebug() << "Drag move event";
    e->acceptProposedAction();
}

void ThumbnailWidget::dragLeaveEvent(QDragLeaveEvent *e)
{
    qDebug() << "Drag leave event";
    e->accept();
}

void ThumbnailWidget::dropEvent(QDropEvent *event)
{

    const QMimeData* mimeData = event->mimeData();
    if(mimeData->urls().size() == 1) { //check if directory got dropped
        if(mimeData->hasUrls()) {
            const QFileInfo urlInfo(mimeData->urls().first().path());
            if(urlInfo.isReadable() && urlInfo.isDir()) {
                emit directoryDropped(urlInfo.canonicalFilePath());
                qDebug() << "It's a dir!!!!!!!!";
            }
        }

    } else { //check for images
        //TODO check for multiple images
        qWarning() << "unimplemented";
    }

    event->acceptProposedAction();
}
