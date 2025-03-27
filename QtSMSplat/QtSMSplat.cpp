#include "QtSMSplat.h"
#include <QDir>
#include <QStandardPaths>
#include <QFileInfo>
#include <QDebug>
#include <QDateTime>

QObject *QtSMSplat::createQtSMSplatSingleton(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine)

    static QtSMSplat *instance = new QtSMSplat(nullptr);

    return instance;
}

void QtSMSplat::registerQtSMSplatQmlTypes() {
    qmlRegisterSingletonType<QtSMSplat>("SMSplat", 1, 0, "SMSplat",
                                        QtSMSplat::createQtSMSplatSingleton);
}

void QtSMSplat::registerQtSMSplat() {
    qmlRegisterType<QtSMSplat>("SMSplat", 1, 0, "SMSplat");
}

QtSMSplat::QtSMSplat(QObject *parent)
    : QObject(parent),
      m_busy(false),
      m_initialized(false),
      m_imageProvider(new SMSplatImageProvider()) {
    m_splatManager = std::make_unique<SMSplatManager>();

   // connect(&m_splatModel, &SMSplatListModel::countChanged, this, [this]() {
   //     for (const auto &name : m_splatModel.names()) {
   //         auto inputInfo = m_inputModel.getByName(name);
   //         auto info = m_splatModel.getByName(name);
   //         info->setImageId(inputInfo->generateImageId());
   //         if (info && inputInfo) {
   //             updateImageProvider(info->imageId(), info->image());
   //         }
   //     }
   // });
}

QtSMSplat::~QtSMSplat() = default;

SMSplatListModel *QtSMSplat::splatModel() const {
    return const_cast<SMSplatListModel *>(&m_splatModel);
}

InputListModel *QtSMSplat::inputModel() const {
    return const_cast<InputListModel *>(&m_inputModel);
}

bool QtSMSplat::busy() const { return m_busy; }

SMSplatImageProvider* QtSMSplat::imageProvider() const { return m_imageProvider; }

QString QtSMSplat::lastError() const { return m_lastError; }

bool QtSMSplat::isInitialized() const { return m_initialized; }

void QtSMSplat::init(QQmlApplicationEngine *engine) {
    Q_ASSERT(engine);
    auto smsplat = qobject_cast<QtSMSplat *>(QtSMSplat::createQtSMSplatSingleton(nullptr, nullptr));
    smsplat->initDatabase("");
    engine->addImageProvider("memory", smsplat->imageProvider());
}

bool QtSMSplat::initDatabase(const QString &dbPath) {
    if (m_initialized) {
        m_db.close();
        m_initialized = false;
        emit initializedChanged();
    }

    QString actualDbPath = dbPath;
    if (actualDbPath.isEmpty()) {
        QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        if (cacheDir.isEmpty()) {
            cacheDir = QDir::homePath() + "/.cache/qt-smsplat";
        }
        QDir().mkpath(cacheDir);
        actualDbPath = cacheDir + "/smsplat.db";
    }

    setBusy(true);
    bool success = m_db.initialize(actualDbPath, "qt_smsplat_connection");
    setBusy(false);

    if (!success) {
        setLastError("Failed to initialize database: " + m_db.lastError());
        return false;
    }

    m_initialized = true;
    emit initializedChanged();
    return true;
}

bool QtSMSplat::generate(QtSMSplatInputInfo *inputInfo) {
    if (!inputInfo) {
        setLastError("Invalid input info");
        return false;
    }

    if (!m_splatManager) {
        setLastError("SPLAT manager not initialized");
        return false;
    }

    setBusy(true);

    try {
        SMSplatInputInfo nativeInput = inputInfo->toStruct();
        auto genInfo = m_splatManager->generate(nativeInput);
        auto resultInfo = std::make_shared<QtSMSplatGenInfo>(genInfo);
        resultInfo->setIsSavedInDb(false);
        resultInfo->setImageId(inputInfo->generateImageId());

        QString name = resultInfo->coverageName();
        updateImageProvider(resultInfo->imageId(), resultInfo->image());

        QString inputName = inputInfo->transmitterName();
        if (m_inputModel.contains(inputName)) {
            m_inputModel.updateByName(inputName, inputInfo);
        } else {
            m_inputModel.append(inputInfo);
        }

        if (m_splatModel.contains(name)) {
            m_splatModel.updateByName(name, resultInfo);
        } else {
            m_splatModel.append(resultInfo);
        }



        setBusy(false);
        emit splatGenerated(name);
        return true;
    } catch (const std::exception &e) {
        setBusy(false);
        setLastError(QString("Error generating SPLAT: %1").arg(e.what()));
        return false;
    } catch (...) {
        setBusy(false);
        setLastError("Unknown error generating SPLAT");
        return false;
    }
}

bool QtSMSplat::save(const QString &name) {
    if (!m_initialized) {
        setLastError("Database not initialized");
        return false;
    }

    if (!m_splatModel.contains(name)) {
        setLastError("Item not found in model" + name);
        return false;
    }

    auto info = m_splatModel.getByName(name);
    auto inputInfo = m_inputModel.getByName(name);

    if (!info) {
        setLastError("Failed to get item from model");
        return false;
    }

    setBusy(true);

    bool success = false;
    if (m_db.hasSplatInfo(name)) {
        success = m_db.updateSplatInfo(info);
        success &= m_db.updateInputInfo(inputInfo);
    } else {
        auto savedInfo = m_db.createSplatInfo(info);
        auto savedInputInfo = m_db.createInputInfo(inputInfo);
        success = (savedInfo != nullptr) && (savedInputInfo != nullptr);
    }

    setBusy(false);

    if (!success) {
        setLastError("Failed to save to database: " + m_db.lastError());
        return false;
    }

    info->setIsSavedInDb(true);
    m_splatModel.updateByName(name, info);
    m_inputModel.updateByName(name, inputInfo);
    emit splatSaved(name);
    return true;
}

QStringList QtSMSplat::getAvailableSplatsFromDb() {
    if (!m_initialized) {
        setLastError("Database not initialized");
        return QStringList();
    }

    setBusy(true);
    QStringList names = m_db.getAllSplatNames();
    setBusy(false);

    if (names.isEmpty() && !m_db.lastError().isEmpty()) {
        setLastError("Failed to get names: " + m_db.lastError());
    }

    return names;
}

bool QtSMSplat::importFromDb(const QStringList &names) {
    if (!m_initialized) {
        setLastError("Database not initialized");
        return false;
    }

    if (names.isEmpty()) {
        return true;
    }

    setBusy(true);
    QList<SplatInfoPtr> importedItems;
    QList<InputInfoPtr> importedInputItems;

    try {
        for (const QString &name : names) {
            auto info = m_db.getSplatInfoByName(name);
            auto inputInfo = m_db.getInputInfoByName(name);
            info->setImageId(inputInfo->generateImageId());
            if (inputInfo) {
                importedInputItems.append(inputInfo);
            }
            if (info) {
                info->setIsSavedInDb(true);
                importedItems.append(info);
                updateImageProvider(info->imageId(), info->image());
            }
        }

        if (!importedInputItems.isEmpty()) {
            m_inputModel.setItems(importedInputItems);
            emit inputImported(names);
        }

        if (!importedItems.isEmpty()) {
            m_splatModel.setItems(importedItems);
            emit splatImported(names);
        }

        setBusy(false);
        return true;
    } catch (const std::exception &e) {
        setBusy(false);
        setLastError(QString("Error importing from database: %1").arg(e.what()));
        return false;
    } catch (...) {
        setBusy(false);
        setLastError("Unknown error importing from database");
        return false;
    }
}

bool QtSMSplat::deleteItem(const QString &name, bool fromDbAlso) {
    if (name.isEmpty()) {
        return false;
    }

    bool removed = m_splatModel.removeByName(name);
    removed &= m_inputModel.removeByName(name);
    if (removed) {
        m_imageProvider->removeImage(name);

        if (fromDbAlso && m_initialized) {
            m_db.deleteSplatInfoByName(name);
            m_db.deleteInputInfoByName(name);
        }

        emit splatDeleted(name);
    }

    return removed;
}

bool QtSMSplat::toggleVisibility(const QString &name) {
    // This would normally update a visibility property in the model
    // Since there's no explicit visibility in the current model,
    // we could add this functionality later
    setLastError("Visibility toggling not implemented yet");
    return false;
}

void QtSMSplat::clearList() {
    m_splatModel.clear();
    m_inputModel.clear();
    m_imageProvider->clear();
}

QtSMSplatInputInfo *QtSMSplat::createInputInfo() {
    return new QtSMSplatInputInfo();
}

void QtSMSplat::setBusy(bool busy) {
    if (m_busy != busy) {
        m_busy = busy;
        emit busyChanged();
    }
}

void QtSMSplat::setLastError(const QString &error) {
    m_lastError = error;
    if (!error.isEmpty()) {
        qWarning() << "QtSMSplat error:" << error;
    }
    emit lastErrorChanged();
}


bool QtSMSplat::saveInputInfo(QtSMSplatInputInfo *inputInfo) {
    if (!m_initialized) {
        setLastError("SMSplat not initialized");
        return false;
    }
    
    if (!inputInfo) {
        setLastError("Invalid input info");
        return false;
    }

    setBusy(true);

    try {
        auto info = new QtSMSplatInputInfo();
        info->setTransmitterName(inputInfo->transmitterName());
        info->setTransmitterLat(inputInfo->transmitterLat());
        info->setTransmitterLon(inputInfo->transmitterLon());
        info->setTransmitterAlt(inputInfo->transmitterAlt());
        info->setItmCovType(inputInfo->itmCovType());
        info->setReceiverHeight(inputInfo->receiverHeight());
        info->setStartAngle(inputInfo->startAngle());
        info->setEndAngle(inputInfo->endAngle());
        info->setRadius(inputInfo->radius());
        info->setFrequency(inputInfo->frequency());
        info->setFresnelZone(inputInfo->fresnelZone());

        auto savedInfo = m_db.createInputInfo(info);
        if (!savedInfo) {
            if (!m_db.updateInputInfo(info)) {
                setBusy(false);
                setLastError(m_db.lastError());
                return false;
            }
        }

        setBusy(false);
        return true;
    }
    catch (const std::exception &e) {
        setBusy(false);
        setLastError(QString("Error saving input info: %1").arg(e.what()));
        return false;
    }
    catch (...) {
        setBusy(false);
        setLastError("Unknown error saving input info");
        return false;
    }
}

QStringList QtSMSplat::getInputInfoNamesFromDb() {
    if (!m_initialized) {
        setLastError("SMSplat not initialized");
        return QStringList();
    }

    setBusy(true);
    QStringList result = m_db.getAllInputNames();
    setBusy(false);
    return result;
}

QtSMSplatInputInfo* QtSMSplat::getInputInfoFromDb(const QString &name) {
    if (!m_initialized) {
        setLastError("SMSplat not initialized");
        return nullptr;
    }

    setBusy(true);
    auto info = m_db.getInputInfoByName(name);
    setBusy(false);

    if (!info) {
        setLastError(m_db.lastError());
        return nullptr;
    }

    return info;
}

QtSMSplatInputInfo* QtSMSplat::getInputInfoForGenInfo(const QString &genInfoName) {
    if (!m_initialized) {
        setLastError("SMSplat not initialized");
        return nullptr;
    }

    setBusy(true);
    auto info = m_db.getInputInfoForGenInfo(genInfoName);
    setBusy(false);

    if (!info) {
        setLastError(m_db.lastError());
        return nullptr;
    }

    return info;
}

bool QtSMSplat::linkInputInfoToGenInfo(const QString &inputName, const QString &genInfoName) {
    if (!m_initialized) {
        setLastError("SMSplat not initialized");
        return false;
    }

    setBusy(true);
    bool result = m_db.linkInputToGenInfo(inputName, genInfoName);
    setBusy(false);

    if (!result) {
        setLastError(m_db.lastError());
    }

    return result;
}

bool QtSMSplat::duplicateSplatGenInfo(const QString &name) {

    if (!m_splatModel.contains(name)) {
        setLastError("Splat not found in model");
        return false;
    }

    auto info = m_splatModel.getByName(name)->clone();
    QString newName = info->coverageName() + "_copy";
    info->setCoverageName(newName);
    info->setIsSavedInDb(false);

    auto inputInfo = m_inputModel.getByName(name)->clone();
    inputInfo->setTransmitterName(newName);
    info->setImageId(inputInfo->generateImageId());
    std::cout << "IMAGE ID dublicated: " << info->imageId().toStdString() << std::endl;
    std::cout << "IMAGE is null: " << info->image().isNull() << std::endl;
    updateImageProvider(info->imageId(), info->image());
    m_splatModel.append(info);

    return true;
}

bool QtSMSplat::duplicateSplatInputInfo(const QString &name) {

    if (!m_inputModel.contains(name)) {
        setLastError("Input not found in model");
        return false;
    }

    auto info = m_inputModel.getByName(name)->clone();
    QString newName = info->transmitterName() + "_copy";
    info->setTransmitterName(newName);
    m_inputModel.append(info);

    return true;
}

void QtSMSplat::updateImageProvider(const QString &name, const QImage &image) {
    if (!name.isEmpty() && !image.isNull()) {
        m_imageProvider->addImage(name, image);
    }
}