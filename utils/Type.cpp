#include "Type.h"
#include <QBuffer>

QtSMSplatInputInfo::QtSMSplatInputInfo(QObject *parent)
    : QObject(parent), m_info(std::make_shared<SMSplatInputInfo>()) {
    m_info->transmitter_name = "";
    m_info->transmitter_lat = 0.00;
    m_info->transmitter_lon = 0.00;
    m_info->transmitter_alt = 0.00;
    m_info->itm_cov_type = "";
    m_info->receiver_height = 0.0;
    m_info->start_angle = 0.0;
    m_info->end_angle = 360.0;
    m_info->radius = 0.0;
    m_info->frequency = 0.0;
    m_info->fresnel_zone = 0.0;
}

QtSMSplatInputInfo::QtSMSplatInputInfo(const SMSplatInputInfo &info, QObject *parent)
    : QObject(parent), m_info(std::make_shared<SMSplatInputInfo>()) {
    *m_info = info;

    if (info.transmitter_name) {
        m_transmitterName = QByteArray(info.transmitter_name);
        m_info->transmitter_name = m_transmitterName.data();
     }


    if (info.itm_cov_type) {
        m_itmCovType = QByteArray(info.itm_cov_type);
        m_info->itm_cov_type = m_itmCovType.data();
    }
}

QtSMSplatInputInfo::~QtSMSplatInputInfo() = default;

SMSplatInputInfo QtSMSplatInputInfo::toStruct() const { return *m_info; }

QString QtSMSplatInputInfo::transmitterName() const {
    return m_info->transmitter_name ? QString(m_info->transmitter_name) : QString();
}

double QtSMSplatInputInfo::transmitterLat() const { return m_info->transmitter_lat; }

double QtSMSplatInputInfo::transmitterLon() const { return m_info->transmitter_lon; }

double QtSMSplatInputInfo::transmitterAlt() const { return m_info->transmitter_alt; }


QString QtSMSplatInputInfo::itmCovType() const {
    return m_info->itm_cov_type ? QString(m_info->itm_cov_type) : QString();
}

double QtSMSplatInputInfo::receiverHeight() const { return m_info->receiver_height; }

double QtSMSplatInputInfo::startAngle() const { return m_info->start_angle; }

double QtSMSplatInputInfo::endAngle() const { return m_info->end_angle; }

double QtSMSplatInputInfo::radius() const { return m_info->radius; }

double QtSMSplatInputInfo::frequency() const { return m_info->frequency; }

double QtSMSplatInputInfo::fresnelZone() const { return m_info->fresnel_zone; }

void QtSMSplatInputInfo::setTransmitterName(const QString &name) {
    m_transmitterName = name.toUtf8();
    m_info->transmitter_name = m_transmitterName.data();
    emit transmitterNameChanged();
}
void QtSMSplatInputInfo::setTransmitterLat(double lat) {
    if(m_info->transmitter_lat != lat) {
        m_info->transmitter_lat = lat;
        emit transmitterLatChanged();
    }
}

void QtSMSplatInputInfo::setTransmitterLon(double lon) {
    if(m_info->transmitter_lon != lon) {
        m_info->transmitter_lon = lon;
        emit transmitterLonChanged();
    }
}

void QtSMSplatInputInfo::setTransmitterAlt(double alt) {
    if(m_info->transmitter_alt != alt) {
        m_info->transmitter_alt = alt;
        emit transmitterAltChanged();
    }
}


void QtSMSplatInputInfo::setItmCovType(const QString &type) {
    m_itmCovType = type.toUtf8();
    m_info->itm_cov_type = m_itmCovType.data();
    emit itmCovTypeChanged();
}

void QtSMSplatInputInfo::setReceiverHeight(double height) {
    if (m_info->receiver_height != height) {
        m_info->receiver_height = height;
        emit receiverHeightChanged();
    }
}

void QtSMSplatInputInfo::setStartAngle(double angle) {
    if (m_info->start_angle != angle) {
        m_info->start_angle = angle;
        emit startAngleChanged();
    }
}

void QtSMSplatInputInfo::setEndAngle(double angle) {
    if (m_info->end_angle != angle) {
        m_info->end_angle = angle;
        emit endAngleChanged();
    }
}

void QtSMSplatInputInfo::setRadius(double value) {
    if (m_info->radius != value) {
        m_info->radius = value;
        emit radiusChanged();
    }
}

void QtSMSplatInputInfo::setFrequency(double value) {
    if (m_info->frequency != value) {
        m_info->frequency = value;
        emit frequencyChanged();
    }
}

void QtSMSplatInputInfo::setFresnelZone(double value) {
    if (m_info->fresnel_zone != value) {
        m_info->fresnel_zone = value;
        emit fresnelZoneChanged();
    }
}

InputInfoPtr QtSMSplatInputInfo::clone() const {
    InputInfoPtr newInfo = new QtSMSplatInputInfo();
    
    newInfo->setTransmitterName(this->transmitterName());
    newInfo->setTransmitterLat(this->transmitterLat());
    newInfo->setTransmitterLon(this->transmitterLon());
    newInfo->setTransmitterAlt(this->transmitterAlt());
    newInfo->setItmCovType(this->itmCovType());
    newInfo->setReceiverHeight(this->receiverHeight());
    newInfo->setStartAngle(this->startAngle());
    newInfo->setEndAngle(this->endAngle());
    newInfo->setRadius(this->radius());
    newInfo->setFrequency(this->frequency());
    newInfo->setFresnelZone(this->fresnelZone());
    
    return newInfo;
}

QtSMSplatGenInfo::QtSMSplatGenInfo(QObject *parent)
    : QObject(parent), m_info(std::make_shared<SMSplatGenInfo>()) {
    m_info->coverage_name = nullptr;
    m_info->image_file = nullptr;
    m_info->image_size.width = 0;
    m_info->image_size.height = 0;
    m_info->coordinates.north = 0.0;
    m_info->coordinates.south = 0.0;
    m_info->coordinates.east = 0.0;
    m_info->coordinates.west = 0.0;
    m_info->zoom_level = 0;
    m_info->transmitter_lon = 0.0;
    m_info->transmitter_lat = 0.0;
    m_info->coverage_radius = 0.0;
    m_isSavedInDb = false;
}

QtSMSplatGenInfo::QtSMSplatGenInfo(const SMSplatGenInfo &info, QObject *parent)
    : QObject(parent), m_info(std::make_shared<SMSplatGenInfo>()) {
    *m_info = info;

    if (info.coverage_name) {
        m_coverageName = QByteArray(info.coverage_name);
        m_info->coverage_name = m_coverageName.data();
    }

    if (info.image_file) {
        m_imageFile = QByteArray(info.image_file);
        m_info->image_file = m_imageFile.data();
    }

    // Convert cv::Mat to QImage for the Qt side
    if (!info.image_.empty()) {
        m_image = cvMatToQImage(info.image_);
    }
}

QtSMSplatGenInfo::~QtSMSplatGenInfo() = default;

SMSplatGenInfo QtSMSplatGenInfo::toStruct() const {
    SMSplatGenInfo result = *m_info;

    // If we have a QImage but the cv::Mat might be empty or outdated,
    // convert QImage to cv::Mat and update the result
    if (!m_image.isNull()) {
        // Convert QImage to cv::Mat
        result.image_ = qImageToCvMat(m_image);
    }

    return result;
}

QImage QtSMSplatGenInfo::image() const {
    // If we already have a QImage cached, return it
    if (!m_image.isNull()) {
        return m_image;
    }
    
    // If we have a cv::Mat but no QImage, convert it
    if (!m_info->image_.empty()) {
        return cvMatToQImage(m_info->image_);
    }
    
    return QImage(); // Return empty image if neither is available
}

void QtSMSplatGenInfo::setImage(const QImage &image) {
    m_image = image;
    
    // Convert QImage to cv::Mat and update m_info
    //m_info->image_ = qImageToCvMat(image);
    
    emit imageChanged();
}

void QtSMSplatGenInfo::setCvImage(const cv::Mat &image) {
    // Store the cv::Mat in the m_info structure
    m_info->image_ = image.clone();
    
    // Update the cached QImage
    m_image = cvMatToQImage(image);
    
    emit imageChanged();
}

QString QtSMSplatGenInfo::coverageName() const {
    return m_info->coverage_name ? QString(m_info->coverage_name) : QString();
}

QString QtSMSplatGenInfo::imageFile() const {
    return m_info->image_file ? QString(m_info->image_file) : QString();
}

int QtSMSplatGenInfo::imageWidth() const { return m_info->image_size.width; }

int QtSMSplatGenInfo::imageHeight() const { return m_info->image_size.height; }

double QtSMSplatGenInfo::north() const { return m_info->coordinates.north; }

double QtSMSplatGenInfo::south() const { return m_info->coordinates.south; }

double QtSMSplatGenInfo::east() const { return m_info->coordinates.east; }

double QtSMSplatGenInfo::west() const { return m_info->coordinates.west; }

double QtSMSplatGenInfo::zoomLevel() const { 
    std::cout << "Zoom level--------------------------: " <<  std::setprecision(14) << m_info->zoom_level << std::endl;
    return m_info->zoom_level;
}

double QtSMSplatGenInfo::transmitterLon() const { return m_info->transmitter_lon; }

double QtSMSplatGenInfo::transmitterLat() const { return m_info->transmitter_lat; }

double QtSMSplatGenInfo::coverageRadius() const { return m_info->coverage_radius; }

bool QtSMSplatGenInfo::isSavedInDb() const { return m_isSavedInDb; }

void QtSMSplatGenInfo::setIsSavedInDb(bool value) {
    m_isSavedInDb = value;
    emit isSavedInDbChanged();
}
void QtSMSplatGenInfo::setCoverageName(const QString &name) {
    m_coverageName = name.toUtf8();
    m_info->coverage_name = m_coverageName.data();
    emit coverageNameChanged();
}

void QtSMSplatGenInfo::setImageFile(const QString &file) {
    m_imageFile = file.toUtf8();
    m_info->image_file = m_imageFile.data();
    emit imageFileChanged();
}

void QtSMSplatGenInfo::setImageWidth(int width) {
    if (m_info->image_size.width != width) {
        m_info->image_size.width = width;
        emit imageWidthChanged();
    }
}

void QtSMSplatGenInfo::setImageHeight(int height) {
    if (m_info->image_size.height != height) {
        m_info->image_size.height = height;
        emit imageHeightChanged();
    }
}

void QtSMSplatGenInfo::setNorth(double value) {
    if (m_info->coordinates.north != value) {
        m_info->coordinates.north = value;
        emit northChanged();
    }
}

void QtSMSplatGenInfo::setSouth(double value) {
    if (m_info->coordinates.south != value) {
        m_info->coordinates.south = value;
        emit southChanged();
    }
}

void QtSMSplatGenInfo::setEast(double value) {
    if (m_info->coordinates.east != value) {
        m_info->coordinates.east = value;
        emit eastChanged();
    }
}

void QtSMSplatGenInfo::setWest(double value) {
    if (m_info->coordinates.west != value) {
        m_info->coordinates.west = value;
        emit westChanged();
    }
}

void QtSMSplatGenInfo::setZoomLevel(double level) {
    if (m_info->zoom_level != level) {
        m_info->zoom_level = level;
        emit zoomLevelChanged();
    }
}

void QtSMSplatGenInfo::setTransmitterLon(double lon) {
    if (m_info->transmitter_lon != lon) {
        m_info->transmitter_lon = lon;
        emit transmitterLonChanged();
    }
}

void QtSMSplatGenInfo::setTransmitterLat(double lat) {
    if (m_info->transmitter_lat != lat) {
        m_info->transmitter_lat = lat;
        emit transmitterLatChanged();
    }
}

void QtSMSplatGenInfo::setCoverageRadius(double radius) {
    if (m_info->coverage_radius != radius) {
        m_info->coverage_radius = radius;
        emit coverageRadiusChanged();
    }
}

QImage QtSMSplatGenInfo::cvMatToQImage(const cv::Mat &mat) const {
    if (mat.empty())
        return QImage();
        
    QImage::Format format;
    cv::Mat temp;
    
    if (mat.channels() == 1) {
        format = QImage::Format_Grayscale8;
        temp = mat;
    } else if (mat.channels() == 3) {
        format = QImage::Format_RGB888;
        cv::cvtColor(mat, temp, cv::COLOR_BGR2RGB);
    } else if (mat.channels() == 4) {
        format = QImage::Format_RGBA8888;
        cv::cvtColor(mat, temp, cv::COLOR_BGRA2RGBA);
    } else {
        return QImage();
    }
    
    QImage image(temp.data, temp.cols, temp.rows, static_cast<int>(temp.step), format);
    
    return image.copy();
}

cv::Mat QtSMSplatGenInfo::qImageToCvMat(const QImage &image) const {
    if (image.isNull())
        return cv::Mat();
        
    cv::Mat mat;
    
    switch (image.format()) {
        case QImage::Format_RGB888:
            mat = cv::Mat(image.height(), image.width(), CV_8UC3, 
                          const_cast<uchar*>(image.bits()), image.bytesPerLine());
            cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
            break;
            
        case QImage::Format_RGBA8888:
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
            {
                QImage converted = image.convertToFormat(QImage::Format_RGBA8888);
                mat = cv::Mat(converted.height(), converted.width(), CV_8UC4, 
                              const_cast<uchar*>(converted.bits()), converted.bytesPerLine());
                cv::cvtColor(mat, mat, cv::COLOR_RGBA2BGRA);
            }
            break;
            
        case QImage::Format_Grayscale8:
            mat = cv::Mat(image.height(), image.width(), CV_8UC1, 
                          const_cast<uchar*>(image.bits()), image.bytesPerLine());
            break;
            
        default:
            QImage converted = image.convertToFormat(QImage::Format_RGBA8888);
            mat = cv::Mat(converted.height(), converted.width(), CV_8UC4, 
                          const_cast<uchar*>(converted.bits()), converted.bytesPerLine());
            cv::cvtColor(mat, mat, cv::COLOR_RGBA2BGRA);
            break;
    }
    
    return mat.clone();
}

cv::Mat QtSMSplatGenInfo::cvImage() const {
    if (!m_info->image_.empty()) {
        return m_info->image_;
    }
    
    if (!m_image.isNull()) {
        return qImageToCvMat(m_image);
    }
    
    return cv::Mat();
}

void QtSMSplatGenInfo::setImageId(const QString &id) {
    m_imageId = id;
    std::cout << "IMAGE ID: " << m_imageId.toStdString() << std::endl;
    emit imageIdChanged();
}

QString QtSMSplatGenInfo::imageId() const {
    return m_imageId;
}

QString QtSMSplatInputInfo::generateImageId() const {
    // Create a hash from all the fields in QtSMSplatInputInfo
    QString dataToHash =
        QString("%1_%2_%3_%4_%5_%6_%7_%8_%9_%10_%11")
            .arg(transmitterName())                      // transmitter name
            .arg(QString::number(transmitterLat(), 'f', 14))  // transmitter latitude
            .arg(QString::number(transmitterLon(), 'f', 14))  // transmitter longitude
            .arg(QString::number(transmitterAlt(), 'f', 14))  // transmitter altitude
            .arg(itmCovType())                      // ITM coverage type
            .arg(QString::number(receiverHeight(), 'f', 14))  // receiver height
            .arg(QString::number(startAngle(), 'f', 14))      // start angle
            .arg(QString::number(endAngle(), 'f', 14))        // end angle
            .arg(QString::number(radius(), 'f', 14))          // radius
            .arg(QString::number(frequency(), 'f', 14))       // frequency
            .arg(QString::number(fresnelZone(), 'f', 14));     // fresnel zone

    QByteArray hash = QCryptographicHash::hash(dataToHash.toUtf8(), QCryptographicHash::Sha256);

    return QString("splat_%1").arg(QString(hash.toHex()).left(16));
}

SplatInfoPtr QtSMSplatGenInfo::clone() const {
    SplatInfoPtr newInfo = std::make_shared<QtSMSplatGenInfo>();
    
    newInfo->setCoverageName(this->coverageName());
    newInfo->setImageFile(this->imageFile());
    newInfo->setImageWidth(this->imageWidth());
    newInfo->setImageHeight(this->imageHeight());
    newInfo->setNorth(this->north());
    newInfo->setSouth(this->south());
    newInfo->setEast(this->east());
    newInfo->setWest(this->west());
    newInfo->setZoomLevel(this->zoomLevel());
    newInfo->setTransmitterLon(this->transmitterLon());
    newInfo->setTransmitterLat(this->transmitterLat());
    newInfo->setCoverageRadius(this->coverageRadius());
    newInfo->setImageId(this->imageId());
    newInfo->setImage(this->image());
    newInfo->setIsSavedInDb(this->isSavedInDb());
    
    return newInfo;
}