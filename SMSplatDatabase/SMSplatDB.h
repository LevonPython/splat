#ifndef SMSPLATDB_H
#define SMSPLATDB_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <memory>
#include <QList>
#include "Type.h"


class SMSplatDB : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool initialized READ isInitialized NOTIFY initializedChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

   public:
    explicit SMSplatDB(QObject *parent = nullptr);
    ~SMSplatDB();

    bool initialize(const QString &dbPath, const QString &connectionName = "smsplat_connection");
    bool isInitialized() const;
    void close();

    SplatInfoPtr createSplatInfo(const SplatInfoPtr &info);
    SplatInfoPtr getSplatInfo(int id);
    SplatInfoPtr getSplatInfoByName(const QString &name);
    QStringList getAllSplatNames();
    QList<SplatInfoPtr> getAllSplatInfos();
    QList<SplatInfoPtr> findSplatInfos(const QString &searchTerm);
    bool updateSplatInfo(const SplatInfoPtr &info);
    bool deleteSplatInfo(int id);
    bool deleteSplatInfoByName(const QString &name);

    InputInfoPtr createInputInfo(const InputInfoPtr &info);
    InputInfoPtr getInputInfo(int id);
    InputInfoPtr getInputInfoByName(const QString &name);
    QStringList getAllInputNames();
    QList<InputInfoPtr> getAllInputInfos();
    bool updateInputInfo(const InputInfoPtr &info);
    bool deleteInputInfo(int id);
    bool deleteInputInfoByName(const QString &name);
    bool hasSplatInfo(const QString&);
    bool hasInputInfo(const QString&);

    bool linkInputToGenInfo(const QString &inputName, const QString &genInfoName);
    InputInfoPtr getInputInfoForGenInfo(const QString &genInfoName);

    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    QString lastError() const;

   signals:
    void initializedChanged();
    void lastErrorChanged();

   private:
    bool createTables();
    bool migrateIfNeeded();
    int getCurrentSchemaVersion();
    bool updateSchemaVersion(int version);

    QSqlDatabase *m_db{nullptr};
    bool m_initialized;
    QString m_lastError;
};

#endif  // SMSPLATDB_H
