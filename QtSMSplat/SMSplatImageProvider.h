#ifndef SMSPLATIMAGEPROVIDER_H
#define SMSPLATIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QHash>
#include <QMutex>
#include <QImage>

class SMSplatImageProvider : public QQuickImageProvider {
   public:
    SMSplatImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

    void addImage(const QString &id, const QImage &image);
    void removeImage(const QString &id);
    void clear();

   private:
    QHash<QString, QImage> m_images;
    QMutex m_mutex;
};

#endif  // SMSPLATIMAGEPROVIDER_H