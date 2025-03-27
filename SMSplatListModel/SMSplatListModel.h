#ifndef SMSPLATLISTMODEL_H
#define SMSPLATLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <memory>
#include "Type.h"

using SplatInfoPtr = std::shared_ptr<QtSMSplatGenInfo>;

class SMSplatListModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

   public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        ImagePathRole,
        ImageWidthRole,
        ImageHeightRole,
        NorthRole,
        SouthRole,
        EastRole,
        WestRole,
        ZoomLevelRole,
        TransmitterLonRole,
        TransmitterLatRole,
        CoverageRadiusRole,
        ImageRole,
        SplatInfoRole,
        IsSavedInDbRole,
        ImageIdRole
    };
    Q_ENUM(Roles)

    explicit SMSplatListModel(QObject *parent = nullptr);
    virtual ~SMSplatListModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;

    Q_INVOKABLE void clear();
    Q_INVOKABLE void append(const SplatInfoPtr &info);
    Q_INVOKABLE void insert(int index, const SplatInfoPtr &info);
    Q_INVOKABLE bool remove(int index);
    Q_INVOKABLE bool removeByName(const QString &name);
    Q_INVOKABLE SplatInfoPtr get(int index) const;
    Q_INVOKABLE SplatInfoPtr getByName(const QString &name) const;
    Q_INVOKABLE int indexOf(const QString &name) const;
    Q_INVOKABLE bool contains(const QString &name) const;
    Q_INVOKABLE bool update(int index, const SplatInfoPtr &info);
    Q_INVOKABLE bool updateByName(const QString &name, const SplatInfoPtr &info);
    Q_INVOKABLE QStringList names() const;
    Q_INVOKABLE void setItems(const QList<SplatInfoPtr> &items);
    Q_INVOKABLE QList<SplatInfoPtr> items() const;
    Q_INVOKABLE QtSMSplatGenInfo* getByNameQml(const QString &name) const;
    Q_INVOKABLE void duplicate(const QString &name);

   signals:
    void countChanged();

   private:
    QList<SplatInfoPtr> m_items;
};

#endif  // SMSPLATLISTMODEL_H
