#include "SMSplatListModel.h"
#include <QDebug>

SMSplatListModel::SMSplatListModel(QObject *parent) : QAbstractListModel(parent) {}

SMSplatListModel::~SMSplatListModel() = default;

int SMSplatListModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;

    return m_items.count();
}

QVariant SMSplatListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.count()) return QVariant();

    const SplatInfoPtr &info = m_items.at(index.row());

    switch (role) {
        case NameRole:
            return info->coverageName();
        case ImagePathRole:
            return info->imageFile();
        case ImageWidthRole:
            return info->imageWidth();
        case ImageHeightRole:
            return info->imageHeight();
        case NorthRole:
            return info->north();
        case SouthRole:
            return info->south();
        case EastRole:
            return info->east();
        case WestRole:
            return info->west();
        case ZoomLevelRole:
            return info->zoomLevel();
        case TransmitterLonRole:
            return info->transmitterLon();
        case TransmitterLatRole:
            return info->transmitterLat();
        case CoverageRadiusRole:
            return info->coverageRadius();
        case ImageRole:
            return info->image();
        case SplatInfoRole:
            return QVariant::fromValue(info);
        case IsSavedInDbRole:
            return info->isSavedInDb();
        case ImageIdRole:
            return info->imageId();
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> SMSplatListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ImagePathRole] = "imagePath";
    roles[ImageWidthRole] = "imageWidth";
    roles[ImageHeightRole] = "imageHeight";
    roles[NorthRole] = "north";
    roles[SouthRole] = "south";
    roles[EastRole] = "east";
    roles[WestRole] = "west";
    roles[ZoomLevelRole] = "zLevel";
    roles[TransmitterLonRole] = "transmitterLon";
    roles[TransmitterLatRole] = "transmitterLat";
    roles[CoverageRadiusRole] = "coverageRadius";
    roles[ImageRole] = "image";
    roles[SplatInfoRole] = "splatInfo";
    roles[IsSavedInDbRole] = "isSavedInDb";
    roles[ImageIdRole] = "imageId";
    return roles;
}


int SMSplatListModel::count() const { return m_items.count(); }

void SMSplatListModel::clear() {
    if (m_items.isEmpty()) return;

    beginResetModel();
    m_items.clear();
    endResetModel();
    emit countChanged();
}

void SMSplatListModel::append(const SplatInfoPtr &info) {
    if (!info) return;

    beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
    m_items.append(info);
    endInsertRows();
    emit countChanged();
}

void SMSplatListModel::insert(int index, const SplatInfoPtr &info) {
    if (!info || index < 0 || index > m_items.count()) return;

    beginInsertRows(QModelIndex(), index, index);
    m_items.insert(index, info);
    endInsertRows();
    emit countChanged();
}

bool SMSplatListModel::remove(int index) {
    if (index < 0 || index >= m_items.count()) return false;

    beginRemoveRows(QModelIndex(), index, index);
    m_items.removeAt(index);
    endRemoveRows();
    emit countChanged();
    return true;
}

bool SMSplatListModel::removeByName(const QString &name) {
    int index = indexOf(name);
    if (index >= 0) {
        return remove(index);
    }
    return false;
}

SplatInfoPtr SMSplatListModel::get(int index) const {
    if (index < 0 || index >= m_items.count()) return nullptr;

    return m_items.at(index);
}

SplatInfoPtr SMSplatListModel::getByName(const QString &name) const {
    for (const SplatInfoPtr &info : m_items) {
        if (info->coverageName() == name) return info;
    }
    return nullptr;
}

QtSMSplatGenInfo* SMSplatListModel::getByNameQml(const QString &name) const {
    SplatInfoPtr info = getByName(name);
    return info ? info.get() : nullptr;
}

int SMSplatListModel::indexOf(const QString &name) const {
    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items.at(i)->coverageName() == name) return i;
    }
    return -1;
}

void SMSplatListModel::duplicate(const QString &name) {
    SplatInfoPtr info = getByName(name);
    if (info) {
        info->setCoverageName(info->coverageName() + "_copy");
        append(info);
    }
}

bool SMSplatListModel::contains(const QString &name) const { return indexOf(name) >= 0; }

bool SMSplatListModel::update(int index, const SplatInfoPtr &info) {
    if (!info || index < 0 || index >= m_items.count()) return false;

    m_items[index] = info;
    //SplatInfoPtr newInfo = std::make_shared<QtSMSplatGenInfo>();
    //newInfo->setCoverageName(info->coverageName());
    //newInfo->setImageFile(info->imageFile());
    //newInfo->setImageWidth(info->imageWidth());
    //newInfo->setImageHeight(info->imageHeight());
    //newInfo->setNorth(info->north());
    //newInfo->setSouth(info->south());
    //newInfo->setEast(info->east());
    //newInfo->setWest(info->west());
    //newInfo->setZoomLevel(info->zoomLevel());
    //newInfo->setTransmitterLon(info->transmitterLon());
    //newInfo->setTransmitterLat(info->transmitterLat());
    //newInfo->setCoverageRadius(info->coverageRadius());
    //newInfo->setImage(info->image());

    //m_items[index] = newInfo;
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex);
    return true;
}

bool SMSplatListModel::updateByName(const QString &name, const SplatInfoPtr &info) {
    int index = indexOf(name);
    if (index >= 0) {
        return update(index, info);
    }
    return false;
}

QStringList SMSplatListModel::names() const {
    QStringList result;
    for (const SplatInfoPtr &info : m_items) {
        result.append(info->coverageName());
    }
    return result;
}

void SMSplatListModel::setItems(const QList<SplatInfoPtr> &items) {
    beginResetModel();
    if (m_items.size() == 0) {
        m_items = items;
    } else {
        for (const SplatInfoPtr &info : items) {
            bool exists = false;
            for (const SplatInfoPtr &existingInfo : m_items) {
                if (existingInfo->coverageName() == info->coverageName()) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                m_items.append(info);
            }
        }
    }
    endResetModel();
    emit countChanged();
}

QList<SplatInfoPtr> SMSplatListModel::items() const { return m_items; }
