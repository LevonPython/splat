#include "SMSplatDB.h"
#include <QBuffer>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>

SMSplatDB::SMSplatDB(QObject *parent) : QObject(parent), m_initialized(false) {}

SMSplatDB::~SMSplatDB() { close(); }

bool SMSplatDB::initialize(const QString &dbPath, const QString &connectionName) {
    if (m_initialized) {
        close();
    }

    m_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", connectionName));
    m_db->setDatabaseName(dbPath);

    if (!m_db->open()) {
        m_lastError = m_db->lastError().text();
        qWarning() << "Failed to open database:" << m_lastError;
        delete m_db;
        m_db = nullptr;
        return false;
    }

    if (!createTables()) {
        close();
        return false;
    }

    m_initialized = true;
    if (!migrateIfNeeded()) {
        close();
        return false;
    }

    return true;
}

bool SMSplatDB::isInitialized() const { return m_initialized; }

void SMSplatDB::close() {
    if (m_db) {
        QString connectionName = m_db->connectionName();
        m_db->close();
        delete m_db;
        m_db = nullptr;
        QSqlDatabase::removeDatabase(connectionName);
        m_initialized = false;
    }
}

bool SMSplatDB::createTables() {
    QSqlQuery query(*m_db);

    // Create schema_version table
    if (!query.exec("CREATE TABLE IF NOT EXISTS schema_version ("
                    "   version INTEGER PRIMARY KEY"
                    ")")) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to create schema_version table:" << m_lastError;
        return false;
    }

    // Check if schema_version table is empty
    query.exec("SELECT COUNT(*) FROM schema_version");
    if (query.next() && query.value(0).toInt() == 0) {
        // Insert initial version
        query.exec("INSERT INTO schema_version (version) VALUES (1)");
    }

    // Create splat_infos table matching QtSMSplatGenInfo structure
    if (!query.exec("CREATE TABLE IF NOT EXISTS splat_infos ("
                    "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "   coverage_name TEXT UNIQUE NOT NULL,"
                    "   image_file TEXT,"
                    "   image_width INTEGER,"
                    "   image_height INTEGER,"
                    "   north REAL,"
                    "   south REAL,"
                    "   east REAL,"
                    "   west REAL,"
                    "   zoom_level INTEGER,"
                    "   transmitter_lon REAL,"
                    "   transmitter_lat REAL,"
                    "   coverage_radius REAL,"
                    "   image BLOB,"
                    "   created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                    "   updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
                    ")")) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to create splat_infos table:" << m_lastError;
        return false;
    }

    // Add new table for splat_input_infos
    if (!query.exec("CREATE TABLE IF NOT EXISTS splat_input_infos ("
                    "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "   transmitter_name TEXT UNIQUE NOT NULL,"
                    "   transmitter_lat REAL,"
                    "   transmitter_lon REAL,"
                    "   transmitter_alt REAL,"
                    "   itm_cov_type TEXT,"
                    "   receiver_height REAL,"
                    "   start_angle REAL,"
                    "   end_angle REAL,"
                    "   radius REAL,"
                    "   frequency REAL,"
                    "   fresnel_zone REAL,"
                    "   created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                    "   updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
                    ")")) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to create splat_input_infos table:" << m_lastError;
        return false;
    }

    // Add table for linking input infos to gen infos
    if (!query.exec("CREATE TABLE IF NOT EXISTS splat_info_links ("
                    "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "   gen_info_name TEXT NOT NULL,"
                    "   input_info_name TEXT NOT NULL,"
                    "   created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                    "   FOREIGN KEY (gen_info_name) REFERENCES splat_infos(coverage_name) ON DELETE CASCADE,"
                    "   FOREIGN KEY (input_info_name) REFERENCES splat_input_infos(transmitter_name) ON DELETE CASCADE,"
                    "   UNIQUE(gen_info_name, input_info_name)"
                    ")")) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to create splat_info_links table:" << m_lastError;
        return false;
    }

    return true;
}

bool SMSplatDB::migrateIfNeeded() {
    int currentVersion = getCurrentSchemaVersion();

    if (currentVersion < 0) {
        return false;  // Error already set
    }

    // Add migration to version 2 if needed (only if currentVersion is 1)
    if (currentVersion == 1) {
        QSqlQuery query(*m_db);
        
        // Begin transaction for migration
        if (!beginTransaction()) {
            m_lastError = "Failed to begin transaction for migration";
            return false;
        }
        
        // Create input info tables
        if (!query.exec("CREATE TABLE IF NOT EXISTS splat_input_infos ("
                       "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "   transmitter_name TEXT UNIQUE NOT NULL,"
                       "   transmitter_lat REAL,"
                       "   transmitter_lon REAL,"
                       "   transmitter_alt REAL,"
                       "   itm_cov_type TEXT,"
                       "   receiver_height REAL,"
                       "   start_angle REAL,"
                       "   end_angle REAL,"
                       "   radius REAL,"
                       "   frequency REAL,"
                       "   fresnel_zone REAL,"
                       "   created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                       "   updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
                       ")")) {
            m_lastError = query.lastError().text();
            rollbackTransaction();
            return false;
        }
        
        if (!query.exec("CREATE TABLE IF NOT EXISTS splat_info_links ("
                       "   id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "   gen_info_name TEXT NOT NULL,"
                       "   input_info_name TEXT NOT NULL,"
                       "   created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                       "   FOREIGN KEY (gen_info_name) REFERENCES splat_infos(coverage_name) ON DELETE CASCADE,"
                       "   FOREIGN KEY (input_info_name) REFERENCES splat_input_infos(transmitter_name) ON DELETE CASCADE,"
                       "   UNIQUE(gen_info_name, input_info_name)"
                       ")")) {
            m_lastError = query.lastError().text();
            rollbackTransaction();
            return false;
        }
        
        // Update schema version
        if (!updateSchemaVersion(2)) {
            rollbackTransaction();
            return false;
        }
        
        // Commit transaction
        if (!commitTransaction()) {
            m_lastError = "Failed to commit transaction for migration";
            return false;
        }
    }

    return true;
}

int SMSplatDB::getCurrentSchemaVersion() {
    QSqlQuery query(*m_db);

    if (!query.exec("SELECT version FROM schema_version LIMIT 1")) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to get schema version:" << m_lastError;
        return -1;
    }

    if (query.next()) {
        return query.value(0).toInt();
    } else {
        m_lastError = "No schema version found";
        return -1;
    }
}

bool SMSplatDB::updateSchemaVersion(int version) {
    QSqlQuery query(*m_db);
    query.prepare("UPDATE schema_version SET version = ?");
    query.addBindValue(version);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to update schema version:" << m_lastError;
        return false;
    }

    return true;
}

SplatInfoPtr SMSplatDB::createSplatInfo(const SplatInfoPtr &info) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return nullptr;
    }

    // Check if a record with this name already exists
    QSqlQuery checkQuery(*m_db);
    checkQuery.prepare("SELECT id FROM splat_infos WHERE coverage_name = ?");
    checkQuery.addBindValue(info->coverageName());

    if (!checkQuery.exec()) {
        m_lastError = checkQuery.lastError().text();
        qWarning() << "Failed to check existing splat info:" << m_lastError;
        return nullptr;
    }

    if (checkQuery.next()) {
        m_lastError = "A splat info with this name already exists";
        return nullptr;
    }

    // Prepare image data
    QByteArray imageData;
    if (!info->image().isNull()) {
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        info->image().save(&buffer, "PNG");
    }

    // Insert the record
    QSqlQuery query(*m_db);
    query.prepare(
        "INSERT INTO splat_infos ("
        "   coverage_name, image_file, image_width, image_height, "
        "   north, south, east, west, zoom_level, "
        "   transmitter_lon, transmitter_lat, coverage_radius, image"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(info->coverageName());
    query.addBindValue(info->imageFile());
    query.addBindValue(info->imageWidth());
    query.addBindValue(info->imageHeight());
    query.addBindValue(info->north());
    query.addBindValue(info->south());
    query.addBindValue(info->east());
    query.addBindValue(info->west());
    query.addBindValue(info->zoomLevel());
    query.addBindValue(info->transmitterLon());
    query.addBindValue(info->transmitterLat());
    query.addBindValue(info->coverageRadius());
    query.addBindValue(imageData);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to create splat info:" << m_lastError;
        return nullptr;
    }

    int newId = query.lastInsertId().toInt();

    return getSplatInfo(newId);
}

SplatInfoPtr SMSplatDB::getSplatInfo(int id) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return nullptr;
    }

    QSqlQuery query(*m_db);
    query.prepare("SELECT * FROM splat_infos WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec() || !query.next()) {
        m_lastError = query.lastError().text();
        if (m_lastError.isEmpty()) {
            m_lastError = "Splat info not found";
        }
        return nullptr;
    }

    // Create a new QtSMSplatGenInfo and populate it from the query
    SplatInfoPtr info = std::make_shared<QtSMSplatGenInfo>();

    info->setCoverageName(query.value("coverage_name").toString());
    info->setImageFile(query.value("image_file").toString());
    info->setImageWidth(query.value("image_width").toInt());
    info->setImageHeight(query.value("image_height").toInt());
    info->setNorth(query.value("north").toDouble());
    info->setSouth(query.value("south").toDouble());
    info->setEast(query.value("east").toDouble());
    info->setWest(query.value("west").toDouble());
    info->setZoomLevel(query.value("zoom_level").toDouble());
    info->setTransmitterLon(query.value("transmitter_lon").toDouble());
    info->setTransmitterLat(query.value("transmitter_lat").toDouble());
    info->setCoverageRadius(query.value("coverage_radius").toDouble());

    QByteArray imageData = query.value("image").toByteArray();
    if (!imageData.isEmpty()) {
        QImage image;
        if (image.loadFromData(imageData, "PNG")) {
            info->setImage(image);
        }
    }

    return info;
}

SplatInfoPtr SMSplatDB::getSplatInfoByName(const QString &name) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return nullptr;
    }

    QSqlQuery query(*m_db);
    query.prepare("SELECT * FROM splat_infos WHERE coverage_name = ?");
    query.addBindValue(name);

    if (!query.exec() || !query.next()) {
        m_lastError = query.lastError().text();
        if (m_lastError.isEmpty()) {
            m_lastError = "Splat info not found";
        }
        return nullptr;
    }

    // Create a new QtSMSplatGenInfo and populate it from the query
    SplatInfoPtr info = std::make_shared<QtSMSplatGenInfo>();

    info->setCoverageName(query.value("coverage_name").toString());
    info->setImageFile(query.value("image_file").toString());
    info->setImageWidth(query.value("image_width").toInt());
    info->setImageHeight(query.value("image_height").toInt());
    info->setNorth(query.value("north").toDouble());
    info->setSouth(query.value("south").toDouble());
    info->setEast(query.value("east").toDouble());
    info->setWest(query.value("west").toDouble());
    info->setZoomLevel(query.value("zoom_level").toDouble());
    info->setTransmitterLon(query.value("transmitter_lon").toDouble());
    info->setTransmitterLat(query.value("transmitter_lat").toDouble());
    info->setCoverageRadius(query.value("coverage_radius").toDouble());

    QByteArray imageData = query.value("image").toByteArray();
    if (!imageData.isEmpty()) {
        QImage image;
        if (image.loadFromData(imageData, "PNG")) {
            info->setImage(image);

        }
    }

    return info;
}

QStringList SMSplatDB::getAllSplatNames() {
    QStringList names;

    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return names;
    }

    QSqlQuery query(*m_db);
    query.prepare("SELECT coverage_name FROM splat_infos ORDER BY coverage_name");

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to get splat names:" << m_lastError;
        return names;
    }

    while (query.next()) {
        names.append(query.value(0).toString());
    }

    return names;
}

QList<SplatInfoPtr> SMSplatDB::getAllSplatInfos() {
    QList<SplatInfoPtr> result;

    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return result;
    }

    QSqlQuery query(*m_db);
    query.prepare("SELECT id FROM splat_infos ORDER BY coverage_name");

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to get splat infos:" << m_lastError;
        return result;
    }

    while (query.next()) {
        int id = query.value(0).toInt();
        SplatInfoPtr info = getSplatInfo(id);
        if (info) {
            result.append(info);
        }
    }

    return result;
}

QList<SplatInfoPtr> SMSplatDB::findSplatInfos(const QString &searchTerm) {
    QList<SplatInfoPtr> result;

    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return result;
    }

    QString likePattern = "%" + searchTerm + "%";

    QSqlQuery query(*m_db);
    query.prepare("SELECT id FROM splat_infos WHERE coverage_name LIKE ? ORDER BY coverage_name");
    query.addBindValue(likePattern);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to search splat infos:" << m_lastError;
        return result;
    }

    while (query.next()) {
        int id = query.value(0).toInt();
        SplatInfoPtr info = getSplatInfo(id);
        if (info) {
            result.append(info);
        }
    }

    return result;
}
bool SMSplatDB::hasSplatInfo(const QString &name) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return false;
    }

    QSqlQuery query(*m_db);
    query.prepare("SELECT id FROM splat_infos WHERE coverage_name = ?");
    query.addBindValue(name);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to check if splat info exists:" << m_lastError;
        return false;
    }

    return query.next();
}

bool SMSplatDB::hasInputInfo(const QString &name) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return false;
    }

    QSqlQuery query(*m_db);
    query.prepare("SELECT id FROM splat_input_infos WHERE transmitter_name = ?");
    query.addBindValue(name);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to check if input info exists:" << m_lastError;
        return false;
    }

    return query.next();
}

InputInfoPtr SMSplatDB::createInputInfo(const InputInfoPtr &info) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return nullptr;
    }

    QSqlQuery checkQuery(*m_db);
    checkQuery.prepare("SELECT id FROM splat_input_infos WHERE transmitter_name = ?");
    checkQuery.addBindValue(info->transmitterName());

    if (!checkQuery.exec()) {
        m_lastError = checkQuery.lastError().text();
        qWarning() << "Failed to check existing input info:" << m_lastError;
        return nullptr;
    }

    if (checkQuery.next()) {
        m_lastError = "An input info with this name already exists";
        return nullptr;
    }

    // Insert the record
    QSqlQuery query(*m_db);
    query.prepare(
        "INSERT INTO splat_input_infos ("
        "   transmitter_name, transmitter_lat, transmitter_lon, transmitter_alt, "
        "   itm_cov_type, receiver_height, start_angle, end_angle, "
        "   radius, frequency, fresnel_zone"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(info->transmitterName());
    query.addBindValue(info->transmitterLat());
    query.addBindValue(info->transmitterLon());
    query.addBindValue(info->transmitterAlt());
    query.addBindValue(info->itmCovType());
    query.addBindValue(info->receiverHeight());
    query.addBindValue(info->startAngle());
    query.addBindValue(info->endAngle());
    query.addBindValue(info->radius());
    query.addBindValue(info->frequency());
    query.addBindValue(info->fresnelZone());

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to create input info:" << m_lastError;
        return nullptr;
    }

    int newId = query.lastInsertId().toInt();

    return getInputInfo(newId);
}

InputInfoPtr SMSplatDB::getInputInfo(int id) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return nullptr;
    }

    QSqlQuery query(*m_db);
    query.prepare("SELECT * FROM splat_input_infos WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec() || !query.next()) {
        m_lastError = query.lastError().text();
        if (m_lastError.isEmpty()) {
            m_lastError = "Input info not found";
        }
        return nullptr;
    }

    InputInfoPtr info = new QtSMSplatInputInfo();

    info->setTransmitterName(query.value("transmitter_name").toString());
    info->setTransmitterLat(query.value("transmitter_lat").toDouble());
    info->setTransmitterLon(query.value("transmitter_lon").toDouble());
    info->setTransmitterAlt(query.value("transmitter_alt").toDouble());
    info->setItmCovType(query.value("itm_cov_type").toString());
    info->setReceiverHeight(query.value("receiver_height").toDouble());
    info->setStartAngle(query.value("start_angle").toDouble());
    info->setEndAngle(query.value("end_angle").toDouble());
    info->setRadius(query.value("radius").toDouble());
    info->setFrequency(query.value("frequency").toDouble());
    info->setFresnelZone(query.value("fresnel_zone").toDouble());

    return info;
}

InputInfoPtr SMSplatDB::getInputInfoByName(const QString &name) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return nullptr;
    }

    QSqlQuery query(*m_db);
    query.prepare("SELECT * FROM splat_input_infos WHERE transmitter_name = ?");
    query.addBindValue(name);

    if (!query.exec() || !query.next()) {
        m_lastError = query.lastError().text();
        if (m_lastError.isEmpty()) {
            m_lastError = "Input info not found";
        }
        return nullptr;
    }

    InputInfoPtr info = new QtSMSplatInputInfo();

    info->setTransmitterName(query.value("transmitter_name").toString());
    info->setTransmitterLat(query.value("transmitter_lat").toDouble());
    info->setTransmitterLon(query.value("transmitter_lon").toDouble());
    info->setTransmitterAlt(query.value("transmitter_alt").toDouble());
    info->setItmCovType(query.value("itm_cov_type").toString());
    info->setReceiverHeight(query.value("receiver_height").toDouble());
    info->setStartAngle(query.value("start_angle").toDouble());
    info->setEndAngle(query.value("end_angle").toDouble());
    info->setRadius(query.value("radius").toDouble());
    info->setFrequency(query.value("frequency").toDouble());
    info->setFresnelZone(query.value("fresnel_zone").toDouble());

    return info;
}

QStringList SMSplatDB::getAllInputNames() {
    QStringList names;

    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return names;
    }

    QSqlQuery query(*m_db);
    query.prepare("SELECT transmitter_name FROM splat_input_infos ORDER BY transmitter_name");

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to get input names:" << m_lastError;
        return names;
    }

    while (query.next()) {
        names.append(query.value(0).toString());
    }

    return names;
}

QList<InputInfoPtr> SMSplatDB::getAllInputInfos() {
    QList<InputInfoPtr> result;

    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return result;
    }

    QSqlQuery query(*m_db);
    query.prepare("SELECT id FROM splat_input_infos ORDER BY transmitter_name");

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to get input infos:" << m_lastError;
        return result;
    }

    while (query.next()) {
        int id = query.value(0).toInt();
        InputInfoPtr info = getInputInfo(id);
        if (info) {
            result.append(info);
        }
    }

    return result;
}

bool SMSplatDB::updateInputInfo(const InputInfoPtr &info) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        qWarning() << "Database not initialized";
        return false;
    }

    QSqlQuery findQuery(*m_db);
    findQuery.prepare("SELECT id FROM splat_input_infos WHERE transmitter_name = ?");
    findQuery.addBindValue(info->transmitterName());

    if (!findQuery.exec()) {
        m_lastError = findQuery.lastError().text();
        qWarning() << "Failed to find input info:" << m_lastError;
        return false;
    }

    if (!findQuery.next()) {
        m_lastError = "Input info not found";
        qWarning() << "Input info not found";
        return false;
    }

    int id = findQuery.value(0).toInt();

    QSqlQuery query(*m_db);
    query.prepare(
        "UPDATE splat_input_infos SET "
        "   transmitter_lat = ?, transmitter_lon = ?, transmitter_alt = ?, "
        "   itm_cov_type = ?, receiver_height = ?, start_angle = ?, end_angle = ?, "
        "   radius = ?, frequency = ?, fresnel_zone = ?, updated_at = CURRENT_TIMESTAMP "
        "WHERE id = ?");

    query.addBindValue(info->transmitterLat());
    query.addBindValue(info->transmitterLon());
    query.addBindValue(info->transmitterAlt());
    query.addBindValue(info->itmCovType());
    query.addBindValue(info->receiverHeight());
    query.addBindValue(info->startAngle());
    query.addBindValue(info->endAngle());
    query.addBindValue(info->radius());
    query.addBindValue(info->frequency());
    query.addBindValue(info->fresnelZone());
    query.addBindValue(id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to update input info:" << m_lastError;
        return false;
    }
    
    qDebug() << "Updated input info for:" << info->transmitterName();
    return true;
}

bool SMSplatDB::deleteInputInfo(int id) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return false;
    }

    QSqlQuery query(*m_db);
    query.prepare("DELETE FROM splat_input_infos WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to delete input info:" << m_lastError;
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool SMSplatDB::deleteInputInfoByName(const QString &name) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return false;
    }

    QSqlQuery query(*m_db);
    query.prepare("DELETE FROM splat_input_infos WHERE transmitter_name = ?");
    query.addBindValue(name);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to delete input info:" << m_lastError;
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool SMSplatDB::linkInputToGenInfo(const QString &inputName, const QString &genInfoName) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return false;
    }

    QSqlQuery checkInputQuery(*m_db);
    checkInputQuery.prepare("SELECT id FROM splat_input_infos WHERE transmitter_name = ?");
    checkInputQuery.addBindValue(inputName);

    if (!checkInputQuery.exec() || !checkInputQuery.next()) {
        m_lastError = "Input info not found";
        return false;
    }

    QSqlQuery checkGenQuery(*m_db);
    checkGenQuery.prepare("SELECT id FROM splat_infos WHERE coverage_name = ?");
    checkGenQuery.addBindValue(genInfoName);

    if (!checkGenQuery.exec() || !checkGenQuery.next()) {
        m_lastError = "Gen info not found";
        return false;
    }

    QSqlQuery query(*m_db);
    query.prepare(
        "INSERT OR REPLACE INTO splat_info_links (gen_info_name, input_info_name) "
        "VALUES (?, ?)");
    query.addBindValue(genInfoName);
    query.addBindValue(inputName);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to link input to gen info:" << m_lastError;
        return false;
    }

    return true;
}

InputInfoPtr SMSplatDB::getInputInfoForGenInfo(const QString &genInfoName) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return nullptr;
    }

    QSqlQuery query(*m_db);
    query.prepare(
        "SELECT i.* FROM splat_input_infos i "
        "JOIN splat_info_links l ON i.transmitter_name = l.input_info_name "
        "WHERE l.gen_info_name = ?");
    query.addBindValue(genInfoName);

    if (!query.exec() || !query.next()) {
        m_lastError = query.lastError().text();
        if (m_lastError.isEmpty()) {
            m_lastError = "No input info linked to this gen info";
        }
        return nullptr;
    }

    InputInfoPtr info = new QtSMSplatInputInfo();

    info->setTransmitterName(query.value("transmitter_name").toString());
    info->setTransmitterLat(query.value("transmitter_lat").toDouble());
    info->setTransmitterLon(query.value("transmitter_lon").toDouble());
    info->setTransmitterAlt(query.value("transmitter_alt").toDouble());
    info->setItmCovType(query.value("itm_cov_type").toString());
    info->setReceiverHeight(query.value("receiver_height").toDouble());
    info->setStartAngle(query.value("start_angle").toDouble());
    info->setEndAngle(query.value("end_angle").toDouble());
    info->setRadius(query.value("radius").toDouble());
    info->setFrequency(query.value("frequency").toDouble());
    info->setFresnelZone(query.value("fresnel_zone").toDouble());

    return info;
}

bool SMSplatDB::beginTransaction() {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return false;
    }

    return m_db->transaction();
}

bool SMSplatDB::commitTransaction() {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return false;
    }

    return m_db->commit();
}

bool SMSplatDB::rollbackTransaction() {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return false;
    }

    return m_db->rollback();
}

QString SMSplatDB::lastError() const { return m_lastError; }

bool SMSplatDB::updateSplatInfo(const SplatInfoPtr &info) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return false;
    }

    QSqlQuery findQuery(*m_db);
    findQuery.prepare("SELECT id FROM splat_infos WHERE coverage_name = ?");
    findQuery.addBindValue(info->coverageName());

    if (!findQuery.exec()) {
        m_lastError = findQuery.lastError().text();
        qWarning() << "Failed to find splat info:" << m_lastError;
        return false;
    }

    if (!findQuery.next()) {
        m_lastError = "Splat info not found";
        qWarning() << "Splat info not found";
        return false;
    }

    int id = findQuery.value(0).toInt();

    QByteArray imageData;
    if (!info->image().isNull()) {
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        info->image().save(&buffer, "PNG");
    }

    QSqlQuery query(*m_db);
    query.prepare(
        "UPDATE splat_infos SET "
        "   image_file = ?, image_width = ?, image_height = ?, "
        "   north = ?, south = ?, east = ?, west = ?, "
        "   zoom_level = ?, transmitter_lon = ?, transmitter_lat = ?, "
        "   coverage_radius = ?, image = ?, updated_at = CURRENT_TIMESTAMP "
        "WHERE id = ?");

    query.addBindValue(info->imageFile());
    query.addBindValue(info->imageWidth());
    query.addBindValue(info->imageHeight());
    query.addBindValue(info->north());
    query.addBindValue(info->south());
    query.addBindValue(info->east());
    query.addBindValue(info->west());
    query.addBindValue(info->zoomLevel());
    query.addBindValue(info->transmitterLon());
    query.addBindValue(info->transmitterLat());
    query.addBindValue(info->coverageRadius());
    query.addBindValue(imageData);
    query.addBindValue(id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to update splat info:" << m_lastError;
        return false;
    }

    qDebug() << "Updated splat info for:" << info->coverageName();
    return true;
}

bool SMSplatDB::deleteSplatInfo(int id) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return false;
    }

    QSqlQuery query(*m_db);
    query.prepare("DELETE FROM splat_infos WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to delete splat info:" << m_lastError;
        return false;
    }

    return query.numRowsAffected() > 0;
}

bool SMSplatDB::deleteSplatInfoByName(const QString &name) {
    if (!m_initialized) {
        m_lastError = "Database not initialized";
        return false;
    }

    QSqlQuery query(*m_db);
    query.prepare("DELETE FROM splat_infos WHERE coverage_name = ?");
    query.addBindValue(name);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        qWarning() << "Failed to delete splat info:" << m_lastError;
        return false;
    }

    return query.numRowsAffected() > 0;
}
