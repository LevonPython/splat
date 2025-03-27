#include <QtTest/QtTest>
#include <QDir>
#include "SMSplatDB.h"
#include <QSqlDatabase>
#include <iostream>
#include <QBuffer>
#include <QStandardPaths>

class TestSMSplatDB : public QObject {
    Q_OBJECT

   private slots:
    void initTestCase();
    void cleanupTestCase();

    void testInitialization();
    void testCreateSplatInfo();
    void testGetSplatInfo();
    void testUpdateSplatInfo();
    void testDeleteSplatInfo();
    void testGetAllSplatInfos();
    void testFindSplatInfos();
    void testTransactions();
    void testNameUniqueness();
    void testGetSplatInfoByName();
    void testGetAllSplatNames();

   private:
    QString m_dbPath;
    SMSplatDB m_db;

    SplatInfoPtr createTestSplatInfo(const QString &name = "Test Splat");
};

void TestSMSplatDB::initTestCase() {
    if (!QSqlDatabase::isDriverAvailable("QSQLITE")) {
        QFAIL("SQLite driver not available");
    }

    // Create a directory in the user's cache location
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (cacheDir.isEmpty()) {
        // Fallback to ~/.cache/qt-smsplat if standard path not available
        cacheDir = QDir::homePath() + "/.cache/qt-smsplat";
    } else {
        // Make sure app name is included in the path
        if (!cacheDir.contains("qt-smsplat")) {
            cacheDir += "/qt-smsplat";
        }
    }

    QDir().mkpath(cacheDir);
    m_dbPath = cacheDir + "/test_smsplatdb.db";

    std::cout << "Database path: " << m_dbPath.toStdString() << std::endl;

    QVERIFY(m_db.initialize(m_dbPath, "test_connection"));
}

void TestSMSplatDB::cleanupTestCase() {
    m_db.close();
    // Uncomment to keep the database file for debugging
    // QFile::remove(m_dbPath);
}

void TestSMSplatDB::testInitialization() {
    QVERIFY(m_db.isInitialized());

    SMSplatDB invalidDb;
    QVERIFY(
        !invalidDb.initialize("/invalid/path/that/does/not/exist/db.sqlite", "invalid_connection"));
    QVERIFY(!invalidDb.isInitialized());
}

SplatInfoPtr TestSMSplatDB::createTestSplatInfo(const QString &name) {
    auto info = std::make_shared<QtSMSplatGenInfo>();

    info->setCoverageName(name);
    info->setImageFile(name + ".png");
    info->setImageWidth(10);
    info->setImageHeight(10);
    info->setNorth(40.7128);
    info->setSouth(40.6128);
    info->setEast(-73.9644);
    info->setWest(-74.0644);
    info->setZoomLevel(10);
    info->setTransmitterLon(-122.3);
    info->setTransmitterLat(47.6);
    info->setCoverageRadius(25.0);

    // Create a test image
    QImage testImage(10, 10, QImage::Format_RGB32);
    testImage.fill(Qt::red);
    info->setImage(testImage);

    return m_db.createSplatInfo(info);
}

void TestSMSplatDB::testCreateSplatInfo() {
    SplatInfoPtr info = createTestSplatInfo();

    QVERIFY(info != nullptr);
    QCOMPARE(info->coverageName(), QString("Test Splat"));
    QCOMPARE(info->imageFile(), QString("Test Splat.png"));
    QCOMPARE(info->imageWidth(), 10);
    QCOMPARE(info->imageHeight(), 10);
    QCOMPARE(info->north(), 40.7128);
    QCOMPARE(info->south(), 40.6128);
    QCOMPARE(info->east(), -73.9644);
    QCOMPARE(info->west(), -74.0644);
    QCOMPARE(info->zoomLevel(), 10);
    QCOMPARE(info->transmitterLon(), -122.3);
    QCOMPARE(info->transmitterLat(), 47.6);
    QCOMPARE(info->coverageRadius(), 25.0);
    QVERIFY(!info->image().isNull());
    QCOMPARE(info->image().pixel(5, 5), qRgb(255, 0, 0));
}

void TestSMSplatDB::testGetSplatInfo() {
    SplatInfoPtr createdInfo = createTestSplatInfo("Get Test");
    QVERIFY(createdInfo != nullptr);

    // Get the info by name since we don't have direct access to the ID
    SplatInfoPtr retrievedInfo = m_db.getSplatInfoByName("Get Test");

    QVERIFY(retrievedInfo != nullptr);
    QCOMPARE(retrievedInfo->coverageName(), QString("Get Test"));
    QCOMPARE(retrievedInfo->imageFile(), createdInfo->imageFile());
    QCOMPARE(retrievedInfo->north(), createdInfo->north());
    QCOMPARE(retrievedInfo->transmitterLon(), createdInfo->transmitterLon());

    SplatInfoPtr nonExistent = m_db.getSplatInfoByName("This Does Not Exist");
    QVERIFY(nonExistent == nullptr);
}

void TestSMSplatDB::testUpdateSplatInfo() {
    SplatInfoPtr info = createTestSplatInfo("Update Test");
    QVERIFY(info != nullptr);

    info->setCoverageName("Update Test");  // Keep the same name for lookup
    info->setImageFile("updated.png");
    info->setNorth(48.8566);
    info->setSouth(48.7566);
    info->setEast(2.3522);
    info->setWest(2.2522);
    info->setZoomLevel(12);

    QImage updatedImage(20, 20, QImage::Format_RGB32);
    updatedImage.fill(Qt::blue);
    info->setImage(updatedImage);

    QVERIFY(m_db.updateSplatInfo(info));

    SplatInfoPtr updatedInfo = m_db.getSplatInfoByName("Update Test");
    QVERIFY(updatedInfo != nullptr);

    QCOMPARE(updatedInfo->coverageName(), QString("Update Test"));
    QCOMPARE(updatedInfo->imageFile(), QString("updated.png"));
    QCOMPARE(updatedInfo->north(), 48.8566);
    QCOMPARE(updatedInfo->east(), 2.3522);
    QCOMPARE(updatedInfo->zoomLevel(), 12);
    QCOMPARE(updatedInfo->image().width(), 20);
    QCOMPARE(updatedInfo->image().height(), 20);
    QCOMPARE(updatedInfo->image().pixel(10, 10), qRgb(0, 0, 255));
}

void TestSMSplatDB::testDeleteSplatInfo() {
    SplatInfoPtr info = createTestSplatInfo("Delete Test");
    QVERIFY(info != nullptr);
    QString name = info->coverageName();

    QVERIFY(m_db.deleteSplatInfoByName(name));
    QVERIFY(m_db.getSplatInfoByName(name) == nullptr);

    QVERIFY(!m_db.deleteSplatInfoByName("This Name Does Not Exist"));
}

void TestSMSplatDB::testGetAllSplatInfos() {
    // First clean existing data
    QStringList existingNames = m_db.getAllSplatNames();
    for (const QString &name : existingNames) {
        m_db.deleteSplatInfoByName(name);
    }

    createTestSplatInfo("Splat A");
    createTestSplatInfo("Splat B");
    createTestSplatInfo("Splat C");

    QList<SplatInfoPtr> allInfos = m_db.getAllSplatInfos();

    QCOMPARE(allInfos.size(), 3);

    // Sort the names alphabetically to ensure consistent test results
    QStringList names;
    for (const auto &info : allInfos) {
        names.append(info->coverageName());
    }
    names.sort();

    QCOMPARE(names[0], QString("Splat A"));
    QCOMPARE(names[1], QString("Splat B"));
    QCOMPARE(names[2], QString("Splat C"));
}

void TestSMSplatDB::testFindSplatInfos() {
    // First clean existing data
    QStringList existingNames = m_db.getAllSplatNames();
    for (const QString &name : existingNames) {
        m_db.deleteSplatInfoByName(name);
    }

    auto alpha = std::make_shared<QtSMSplatGenInfo>();
    alpha->setCoverageName("Alpha Splat");
    alpha->setImageFile("alpha.png");
    m_db.createSplatInfo(alpha);

    auto beta = std::make_shared<QtSMSplatGenInfo>();
    beta->setCoverageName("Beta Splat");
    beta->setImageFile("beta.png");
    m_db.createSplatInfo(beta);

    auto gamma = std::make_shared<QtSMSplatGenInfo>();
    gamma->setCoverageName("Gamma Mountain");
    gamma->setImageFile("gamma.png");
    m_db.createSplatInfo(gamma);

    QList<SplatInfoPtr> alphaResults = m_db.findSplatInfos("Alpha");
    QCOMPARE(alphaResults.size(), 1);
    QCOMPARE(alphaResults[0]->coverageName(), QString("Alpha Splat"));

    QList<SplatInfoPtr> mountainResults = m_db.findSplatInfos("Mountain");
    QCOMPARE(mountainResults.size(), 1);
    QCOMPARE(mountainResults[0]->coverageName(), QString("Gamma Mountain"));

    QList<SplatInfoPtr> splatResults = m_db.findSplatInfos("Splat");
    QCOMPARE(splatResults.size(), 2);  // Should find both Alpha Splat and Beta Splat

    QList<SplatInfoPtr> noResults = m_db.findSplatInfos("XYZ123");
    QCOMPARE(noResults.size(), 0);
}

void TestSMSplatDB::testTransactions() {
    QVERIFY(m_db.beginTransaction());

    SplatInfoPtr info = createTestSplatInfo("Transaction Test");
    QVERIFY(info != nullptr);
    QString name = info->coverageName();

    QVERIFY(m_db.rollbackTransaction());

    QVERIFY(m_db.getSplatInfoByName(name) == nullptr);

    QVERIFY(m_db.beginTransaction());

    info = createTestSplatInfo("Committed Transaction Test");
    QVERIFY(info != nullptr);
    name = info->coverageName();

    QVERIFY(m_db.commitTransaction());

    SplatInfoPtr retrievedInfo = m_db.getSplatInfoByName(name);
    QVERIFY(retrievedInfo != nullptr);
    QCOMPARE(retrievedInfo->coverageName(), QString("Committed Transaction Test"));
}

void TestSMSplatDB::testNameUniqueness() {
    auto info1 = std::make_shared<QtSMSplatGenInfo>();
    info1->setCoverageName("Unique Name Test");
    info1->setImageFile("unique1.png");

    SplatInfoPtr created1 = m_db.createSplatInfo(info1);
    QVERIFY(created1 != nullptr);

    auto info2 = std::make_shared<QtSMSplatGenInfo>();
    info2->setCoverageName("Unique Name Test");
    info2->setImageFile("unique2.png");

    SplatInfoPtr created2 = m_db.createSplatInfo(info2);
    QVERIFY(created2 == nullptr);
    QVERIFY(!m_db.lastError().isEmpty());

    SplatInfoPtr anotherInfo = createTestSplatInfo("Another Test Splat");
    QVERIFY(anotherInfo != nullptr);

    anotherInfo->setCoverageName("Unique Name Test");
    QVERIFY(!m_db.updateSplatInfo(anotherInfo));
    QVERIFY(!m_db.lastError().isEmpty());
}

void TestSMSplatDB::testGetSplatInfoByName() {
    QString uniqueName = "Get By Name Test";
    SplatInfoPtr createdInfo = createTestSplatInfo(uniqueName);
    QVERIFY(createdInfo != nullptr);

    SplatInfoPtr retrievedInfo = m_db.getSplatInfoByName(uniqueName);

    QVERIFY(retrievedInfo != nullptr);
    QCOMPARE(retrievedInfo->coverageName(), uniqueName);

    SplatInfoPtr nonExistent = m_db.getSplatInfoByName("This Name Does Not Exist");
    QVERIFY(nonExistent == nullptr);
}

void TestSMSplatDB::testGetAllSplatNames() {
    // First clean existing data
    QStringList existingNames = m_db.getAllSplatNames();
    auto tmp_size = existingNames.size();
    for (const QString &name : existingNames) {
        //    m_db.deleteSplatInfoByName(name);
    }

    createTestSplatInfo("Name A");
    createTestSplatInfo("Name B");
    createTestSplatInfo("Name C");

    QStringList allNames = m_db.getAllSplatNames();

    // Sort the names to ensure consistent test results
    allNames.sort();

    QCOMPARE(allNames.size() - tmp_size, 3);
    QCOMPARE(allNames[0], QString("Name A"));
    QCOMPARE(allNames[1], QString("Name B"));
    QCOMPARE(allNames[2], QString("Name C"));
}

QTEST_MAIN(TestSMSplatDB)
#include "test_smsplatdb.moc"