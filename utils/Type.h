#ifndef TYPE_H
#define TYPE_H

#include <QObject>
#include <QImage>
#include <memory>
#include <sm_splat_info.h>
#include <opencv2/opencv.hpp>
#include <QString>
#include <QCryptographicHash>

class QtSMSplatInputInfo;
class QtSMSplatGenInfo;

using SplatInfoPtr = std::shared_ptr<QtSMSplatGenInfo>;
//using InputInfoPtr = std::shared_ptr<QtSMSplatInputInfo>;
using InputInfoPtr = QtSMSplatInputInfo*;

class QtSMSplatInputInfo : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString transmitterName READ transmitterName WRITE setTransmitterName NOTIFY
                   transmitterNameChanged)
    Q_PROPERTY(double transmitterLat READ transmitterLat WRITE setTransmitterLat NOTIFY
                   transmitterLatChanged)
    Q_PROPERTY(double transmitterLon READ transmitterLon WRITE setTransmitterLon NOTIFY
                   transmitterLonChanged)
    Q_PROPERTY(double transmitterAlt READ transmitterAlt WRITE setTransmitterAlt NOTIFY
                   transmitterAltChanged)

    Q_PROPERTY(QString itmCovType READ itmCovType WRITE setItmCovType NOTIFY itmCovTypeChanged)
    Q_PROPERTY(double receiverHeight READ receiverHeight WRITE setReceiverHeight NOTIFY
                   receiverHeightChanged)
    Q_PROPERTY(double startAngle READ startAngle WRITE setStartAngle NOTIFY startAngleChanged)
    Q_PROPERTY(double endAngle READ endAngle WRITE setEndAngle NOTIFY endAngleChanged)

    Q_PROPERTY(double radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(double frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(double fresnelZone READ fresnelZone WRITE setFresnelZone NOTIFY fresnelZoneChanged)

   public:
    explicit QtSMSplatInputInfo(QObject *parent = nullptr);
    explicit QtSMSplatInputInfo(const SMSplatInputInfo &info, QObject *parent = nullptr);
    ~QtSMSplatInputInfo();

    SMSplatInputInfo toStruct() const;

    QString transmitterName() const;
    double transmitterLat() const;
    double transmitterLon() const;
    double transmitterAlt() const;
    QString itmCovType() const;
    double receiverHeight() const;
    double startAngle() const;
    double endAngle() const;
    double radius() const;
    double frequency() const;
    double fresnelZone() const;
    QString generateImageId() const;

    void setTransmitterName(const QString &name);
    void setTransmitterLat(double lat);
    void setTransmitterLon(double lon);
    void setTransmitterAlt(double alt);
    void setItmCovType(const QString &type);
    void setReceiverHeight(double height);
    void setStartAngle(double angle);
    void setEndAngle(double angle);
    void setRadius(double value);
    void setFrequency(double value);
    void setFresnelZone(double value);

    Q_INVOKABLE InputInfoPtr clone() const;

   signals:
    void transmitterNameChanged();
    void transmitterLatChanged();
    void transmitterLonChanged();
    void transmitterAltChanged();
    void itmCovTypeChanged();
    void receiverHeightChanged();
    void startAngleChanged();
    void endAngleChanged();
    void radiusChanged();
    void frequencyChanged();
    void fresnelZoneChanged();

   private:
    std::shared_ptr<SMSplatInputInfo> m_info;

    QByteArray m_transmitterName;
    QByteArray m_itmCovType;
    QByteArray m_elevPath;
};

class QtSMSplatGenInfo : public QObject {
    Q_OBJECT

    Q_PROPERTY(
        QString coverageName READ coverageName WRITE setCoverageName NOTIFY coverageNameChanged)
    Q_PROPERTY(QString imageFile READ imageFile WRITE setImageFile NOTIFY imageFileChanged)
    Q_PROPERTY(bool isSavedInDb READ isSavedInDb WRITE setIsSavedInDb NOTIFY isSavedInDbChanged)
    Q_PROPERTY(QString imageId READ imageId WRITE setImageId NOTIFY imageIdChanged)
    Q_PROPERTY(int imageWidth READ imageWidth WRITE setImageWidth NOTIFY imageWidthChanged)
    Q_PROPERTY(int imageHeight READ imageHeight WRITE setImageHeight NOTIFY imageHeightChanged)

    Q_PROPERTY(double north READ north WRITE setNorth NOTIFY northChanged)
    Q_PROPERTY(double south READ south WRITE setSouth NOTIFY southChanged)
    Q_PROPERTY(double east READ east WRITE setEast NOTIFY eastChanged)
    Q_PROPERTY(double west READ west WRITE setWest NOTIFY westChanged)

    Q_PROPERTY(double zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(double transmitterLon READ transmitterLon WRITE setTransmitterLon NOTIFY
                   transmitterLonChanged)
    Q_PROPERTY(double transmitterLat READ transmitterLat WRITE setTransmitterLat NOTIFY
                   transmitterLatChanged)
    Q_PROPERTY(double coverageRadius READ coverageRadius WRITE setCoverageRadius NOTIFY
                   coverageRadiusChanged)

    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)

   public:
    explicit QtSMSplatGenInfo(QObject *parent = nullptr);
    explicit QtSMSplatGenInfo(const SMSplatGenInfo &info, QObject *parent = nullptr);
    ~QtSMSplatGenInfo();

    SMSplatGenInfo toStruct() const;
    Q_INVOKABLE SplatInfoPtr clone() const;

    QString coverageName() const;
    QString imageFile() const;
    int imageWidth() const;
    int imageHeight() const;
    double north() const;
    double south() const;
    double east() const;
    double west() const;
    double zoomLevel() const;
    double transmitterLon() const;
    double transmitterLat() const;
    double coverageRadius() const;
    QImage image() const;
    cv::Mat cvImage() const;
    bool isSavedInDb() const;
    QString imageId() const;

    void setCoverageName(const QString &name);
    void setImageId(const QString &id);
    void setImageFile(const QString &file);
    void setImageWidth(int width);
    void setImageHeight(int height);
    void setNorth(double value);
    void setSouth(double value);
    void setEast(double value);
    void setWest(double value);
    void setZoomLevel(double level);
    void setTransmitterLon(double lon);
    void setTransmitterLat(double lat);
    void setCoverageRadius(double radius);
    void setImage(const QImage &image);
    void setCvImage(const cv::Mat &image);
    void setIsSavedInDb(bool value);


   signals:
    void coverageNameChanged();
    void imageFileChanged();
    void imageWidthChanged();
    void imageHeightChanged();
    void imageIdChanged();
    void northChanged();
    void southChanged();
    void eastChanged();
    void westChanged();
    void zoomLevelChanged();
    void transmitterLonChanged();
    void transmitterLatChanged();
    void coverageRadiusChanged();
    void imageChanged();
    void isSavedInDbChanged();

   private:
    std::shared_ptr<SMSplatGenInfo> m_info;

    QByteArray m_coverageName;
    QByteArray m_imageFile;

    QImage m_image;
    bool m_isSavedInDb{false};
    QString m_imageId;
    QImage cvMatToQImage(const cv::Mat &mat) const;
    cv::Mat qImageToCvMat(const QImage &image) const;
};

#endif  // TYPE_H