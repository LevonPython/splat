#ifndef QTSMSPLAT_H
#define QTSMSPLAT_H

#include <QObject>
#include <QQmlEngine>
#include <QSet>
#include <memory>
#include "SMSplatDB.h"
#include "SMSplatListModel.h"
#include "InputListModel.h"
#include "sm_splat_manager.h"
#include "SMSplatImageProvider.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Type.h"

class QtSMSplat : public QObject {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(SMSplatListModel *splatModel READ splatModel CONSTANT)
    Q_PROPERTY(InputListModel *inputModel READ inputModel CONSTANT)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(bool isInitialized READ isInitialized NOTIFY initializedChanged)

   public:
    explicit QtSMSplat(QObject *parent = nullptr);
    ~QtSMSplat();

    SMSplatListModel *splatModel() const;
    InputListModel *inputModel() const;
    bool busy() const;
    QString lastError() const;
    bool isInitialized() const;

    Q_INVOKABLE bool initDatabase(const QString &dbPath);
    Q_INVOKABLE bool generate(QtSMSplatInputInfo *inputInfo);
    Q_INVOKABLE bool save(const QString &name);
    Q_INVOKABLE QStringList getAvailableSplatsFromDb();
    Q_INVOKABLE bool importFromDb(const QStringList &names);
    Q_INVOKABLE bool deleteItem(const QString &name, bool fromDbAlso);
    Q_INVOKABLE bool toggleVisibility(const QString &name);
    Q_INVOKABLE void clearList();

    Q_INVOKABLE QtSMSplatInputInfo *createInputInfo();
    Q_INVOKABLE void updateImageProvider(const QString &name, const QImage &image);
    Q_INVOKABLE SMSplatImageProvider* imageProvider() const;

    Q_INVOKABLE bool saveInputInfo(QtSMSplatInputInfo *inputInfo);
    Q_INVOKABLE QStringList getInputInfoNamesFromDb();
    Q_INVOKABLE QtSMSplatInputInfo* getInputInfoFromDb(const QString &name);
    Q_INVOKABLE QtSMSplatInputInfo* getInputInfoForGenInfo(const QString &genInfoName);
    Q_INVOKABLE bool linkInputInfoToGenInfo(const QString &inputName, const QString &genInfoName);
    Q_INVOKABLE bool duplicateSplatGenInfo(const QString &name);
    Q_INVOKABLE bool duplicateSplatInputInfo(const QString &name);

    static void registerQtSMSplat();
    static void registerQtSMSplatQmlTypes();
    static void init(QQmlApplicationEngine *engine);
    static QObject *createQtSMSplatSingleton(QQmlEngine *engine, QJSEngine *scriptEngine);

   signals:
    void busyChanged();
    void lastErrorChanged();
    void initializedChanged();
    void splatGenerated(const QString &name);
    void splatSaved(const QString &name);
    void splatDeleted(const QString &name);
    void splatImported(const QStringList &names);
    void inputImported(const QStringList &names);
    void savedInDbChanged();

   private:
    SMSplatListModel m_splatModel;
    InputListModel m_inputModel;
    SMSplatDB m_db;
    std::unique_ptr<SMSplatManager> m_splatManager;
    bool m_busy;
    QString m_lastError;
    bool m_initialized;
    SMSplatImageProvider* m_imageProvider{nullptr};

    void setBusy(bool busy);
    void setLastError(const QString &error);
};

#endif  // QTSMSPLAT_H
