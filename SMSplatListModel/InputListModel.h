#ifndef INPUTLISTMODEL_H
#define INPUTLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <memory>
#include "Type.h"

using InputInfoPtr = QtSMSplatInputInfo*;

class InputListModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles {
        TransmitterNameRole = Qt::UserRole + 1,
        TransmitterLatRole,
        TransmitterLonRole,
        TransmitterAltRole,
        ItmCovTypeRole,
        ReceiverHeightRole,
        StartAngleRole,
        EndAngleRole,
        RadiusRole,
        FrequencyRole,
        FresnelZoneRole,
        InputInfoRole
    };
    Q_ENUM(Roles)

    explicit InputListModel(QObject *parent = nullptr);
    virtual ~InputListModel();


    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;

    Q_INVOKABLE void clear();
    Q_INVOKABLE void append(const InputInfoPtr &info);
    Q_INVOKABLE void insert(int index, const InputInfoPtr &info);
    Q_INVOKABLE bool remove(int index);
    Q_INVOKABLE bool removeByName(const QString &name);
    Q_INVOKABLE InputInfoPtr get(int index) const;
    Q_INVOKABLE InputInfoPtr getByName(const QString &name) const;
    Q_INVOKABLE int indexOf(const QString &name) const;
    Q_INVOKABLE bool contains(const QString &name) const;
    Q_INVOKABLE bool update(int index, const InputInfoPtr &info);
    Q_INVOKABLE bool updateByName(const QString &name, const InputInfoPtr &info);
    Q_INVOKABLE QStringList names() const;
    Q_INVOKABLE void setItems(const QList<InputInfoPtr> &items);
    Q_INVOKABLE QList<InputInfoPtr> items() const;
    Q_INVOKABLE QtSMSplatInputInfo* getByNameQml(const QString &name) const;
    Q_INVOKABLE void duplicate(const QString &name);
signals:
    void countChanged();

private:
    QList<InputInfoPtr> m_items;
};

#endif // INPUTLISTMODEL_H
