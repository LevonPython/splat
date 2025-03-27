#include "SMSplatImageProvider.h"

SMSplatImageProvider::SMSplatImageProvider() : QQuickImageProvider(QQuickImageProvider::Image) {}

QImage SMSplatImageProvider::requestImage(const QString &id, QSize *size,
                                          const QSize &requestedSize) {
    QMutexLocker locker(&m_mutex);

    if (!m_images.contains(id)) {
        if (size) {
            *size = QSize(0, 0);
        }
        return QImage();
    }

    QImage image = m_images[id];

    if (size) {
        *size = image.size();
    }

    if (requestedSize.isValid() && requestedSize != image.size()) {
        image = image.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    return image;
}

void SMSplatImageProvider::addImage(const QString &id, const QImage &image) {
    if (image.isNull()) {
        return;
    }

    QMutexLocker locker(&m_mutex);
    m_images[id] = image;
}

void SMSplatImageProvider::removeImage(const QString &id) {
    QMutexLocker locker(&m_mutex);
    m_images.remove(id);
}

void SMSplatImageProvider::clear() {
    QMutexLocker locker(&m_mutex);
    m_images.clear();
}
