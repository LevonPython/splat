#include "InputListModel.h"
#include <QDebug>

InputListModel::InputListModel(QObject *parent) : QAbstractListModel(parent) {}

InputListModel::~InputListModel() = default;

int InputListModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return m_items.count();
}

QVariant InputListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.count()) 
        return QVariant();

    const InputInfoPtr &info = m_items.at(index.row());

    switch (role) {
        case TransmitterNameRole:
            return info->transmitterName();
        case TransmitterLatRole:
            return info->transmitterLat();
        case TransmitterLonRole:
            return info->transmitterLon();
        case TransmitterAltRole:
            return info->transmitterAlt();
        case ItmCovTypeRole:
            return info->itmCovType();
        case ReceiverHeightRole:
            return info->receiverHeight();
        case StartAngleRole:
            return info->startAngle();
        case EndAngleRole:
            return info->endAngle();
        case RadiusRole:
            return info->radius();
        case FrequencyRole:
            return info->frequency();
        case FresnelZoneRole:
            return info->fresnelZone();
        case InputInfoRole:
            return QVariant::fromValue(info);
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> InputListModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TransmitterNameRole] = "transmitterName";
    roles[TransmitterLatRole] = "transmitterLat";
    roles[TransmitterLonRole] = "transmitterLon";
    roles[TransmitterAltRole] = "transmitterAlt";
    roles[ItmCovTypeRole] = "itmCovType";
    roles[ReceiverHeightRole] = "receiverHeight";
    roles[StartAngleRole] = "startAngle";
    roles[EndAngleRole] = "endAngle";
    roles[RadiusRole] = "radius";
    roles[FrequencyRole] = "frequency";
    roles[FresnelZoneRole] = "fresnelZone";
    roles[InputInfoRole] = "inputInfo";
    return roles;
}

int InputListModel::count() const {
    return m_items.count();
}

void InputListModel::clear() {
    if (m_items.isEmpty())
        return;

    beginResetModel();
    m_items.clear();
    endResetModel();
    emit countChanged();
}

void InputListModel::append(const InputInfoPtr &info) {
    if (!info)
        return;

    beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
    m_items.append(info);
    endInsertRows();
    emit countChanged();
}

void InputListModel::insert(int index, const InputInfoPtr &info) {
    if (!info || index < 0 || index > m_items.count())
        return;

    beginInsertRows(QModelIndex(), index, index);
    m_items.insert(index, info);
    endInsertRows();
    emit countChanged();
}

void InputListModel::duplicate(const QString &name) {
    InputInfoPtr info = getByName(name);
    if (info) {
        info->setTransmitterName(info->transmitterName() + "_copy");
        append(info);
    }
}

bool InputListModel::remove(int index) {
    if (index < 0 || index >= m_items.count())
        return false;

    beginRemoveRows(QModelIndex(), index, index);
    m_items.removeAt(index);
    endRemoveRows();
    emit countChanged();
    return true;
}

bool InputListModel::removeByName(const QString &name) {
    int index = indexOf(name);
    return index >= 0 ? remove(index) : false;
}

InputInfoPtr InputListModel::get(int index) const {
    return (index >= 0 && index < m_items.count()) ? m_items.at(index) : nullptr;
}

QtSMSplatInputInfo* InputListModel::getByNameQml(const QString &name) const {
    InputInfoPtr info = getByName(name);
    return info;
}

InputInfoPtr InputListModel::getByName(const QString &name) const {
    for (const auto &info : m_items) {
        if (info->transmitterName() == name) {
            return info;
        }
    }
    return nullptr;
}

int InputListModel::indexOf(const QString &name) const {
    for (int i = 0; i < m_items.count(); ++i) {
        if (m_items.at(i)->transmitterName() == name) {
            return i;
        }
    }
    return -1;
}

bool InputListModel::contains(const QString &name) const {
    return indexOf(name) >= 0;
}

bool InputListModel::update(int index, const InputInfoPtr &info) {
    if (!info || index < 0 || index >= m_items.count())
        return false;

    m_items[index] = info;
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex);
    return true;
}

bool InputListModel::updateByName(const QString &name, const InputInfoPtr &info) {
    int index = indexOf(name);
    return index >= 0 ? update(index, info) : false;
}

QStringList InputListModel::names() const {
    QStringList result;
    for (const auto &info : m_items) {
        result.append(info->transmitterName());
    }
    return result;
}

void InputListModel::setItems(const QList<InputInfoPtr> &items) {
    beginResetModel();
    if (m_items.size() == 0) {
        m_items = items;
    } else {
        for (const InputInfoPtr &info : items) {
            bool exists = false;
            for (const InputInfoPtr &existingInfo : m_items) {
                if (existingInfo->transmitterName() == info->transmitterName()) {
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

QList<InputInfoPtr> InputListModel::items() const {
    return m_items;
}
