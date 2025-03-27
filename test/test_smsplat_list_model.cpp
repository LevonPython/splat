#include <QtTest/QtTest>
#include "SMSplatListModel.h"
#include <QSignalSpy>

class TestSMSplatListModel : public QObject {
    Q_OBJECT

   private slots:
    void initTestCase();
    void cleanupTestCase();

    void testInitialState();
    void testAppendItem();
    void testInsertItem();
    void testRemoveItem();
    void testRemoveByName();
    void testGetItem();
    void testGetByName();
    void testIndexOf();
    void testContains();
    void testUpdate();
    void testUpdateByName();
    void testNames();
    void testClear();
    void testSetItems();
    void testCountProperty();
    void testRoleNames();
    void testDataRoles();

   private:
    SMSplatListModel m_model;

    SplatInfoPtr createTestSplatInfo(const QString &name = "Test Splat", double lon = -122.3,
                                     double lat = 47.6);
};

void TestSMSplatListModel::initTestCase() {}

void TestSMSplatListModel::cleanupTestCase() {}

void TestSMSplatListModel::testInitialState() {
    QCOMPARE(m_model.rowCount(), 0);
    QCOMPARE(m_model.count(), 0);
    QVERIFY(m_model.names().isEmpty());
}

SplatInfoPtr TestSMSplatListModel::createTestSplatInfo(const QString &name, double lon,
                                                       double lat) {
    auto info = std::make_shared<QtSMSplatGenInfo>();

    info->setCoverageName(name);
    info->setImageFile(name + ".png");
    info->setImageWidth(100);
    info->setImageHeight(100);
    info->setNorth(lat + 0.1);
    info->setSouth(lat - 0.1);
    info->setEast(lon + 0.1);
    info->setWest(lon - 0.1);
    info->setZoomLevel(10);
    info->setTransmitterLon(lon);
    info->setTransmitterLat(lat);
    info->setCoverageRadius(25.0);

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::red);
    info->setImage(testImage);

    return info;
}

void TestSMSplatListModel::testAppendItem() {
    QSignalSpy spy(&m_model, &SMSplatListModel::countChanged);

    SplatInfoPtr info = createTestSplatInfo("Append Test");
    m_model.append(info);

    QCOMPARE(m_model.count(), 1);
    QCOMPARE(m_model.rowCount(), 1);
    QCOMPARE(spy.count(), 1);

    SplatInfoPtr retrieved = m_model.get(0);
    QVERIFY(retrieved != nullptr);
    QCOMPARE(retrieved->coverageName(), QString("Append Test"));
    QCOMPARE(retrieved->imageFile(), QString("Append Test.png"));

    m_model.clear();
}

void TestSMSplatListModel::testInsertItem() {
    m_model.append(createTestSplatInfo("First Item"));
    m_model.append(createTestSplatInfo("Third Item"));

    QSignalSpy spy(&m_model, &SMSplatListModel::countChanged);
    m_model.insert(1, createTestSplatInfo("Second Item"));

    QCOMPARE(m_model.count(), 3);
    QCOMPARE(spy.count(), 1);

    QCOMPARE(m_model.get(0)->coverageName(), QString("First Item"));
    QCOMPARE(m_model.get(1)->coverageName(), QString("Second Item"));
    QCOMPARE(m_model.get(2)->coverageName(), QString("Third Item"));

    m_model.clear();
}

void TestSMSplatListModel::testRemoveItem() {
    m_model.append(createTestSplatInfo("Item One"));
    m_model.append(createTestSplatInfo("Item Two"));
    m_model.append(createTestSplatInfo("Item Three"));

    QSignalSpy spy(&m_model, &SMSplatListModel::countChanged);
    bool result = m_model.remove(1);

    QVERIFY(result);
    QCOMPARE(m_model.count(), 2);
    QCOMPARE(spy.count(), 1);

    QCOMPARE(m_model.get(0)->coverageName(), QString("Item One"));
    QCOMPARE(m_model.get(1)->coverageName(), QString("Item Three"));

    result = m_model.remove(99);
    QVERIFY(!result);
    QCOMPARE(m_model.count(), 2);

    m_model.clear();
}

void TestSMSplatListModel::testRemoveByName() {
    m_model.append(createTestSplatInfo("Alpha"));
    m_model.append(createTestSplatInfo("Beta"));
    m_model.append(createTestSplatInfo("Gamma"));

    QSignalSpy spy(&m_model, &SMSplatListModel::countChanged);
    bool result = m_model.removeByName("Beta");

    QVERIFY(result);
    QCOMPARE(m_model.count(), 2);
    QCOMPARE(spy.count(), 1);

    QCOMPARE(m_model.get(0)->coverageName(), QString("Alpha"));
    QCOMPARE(m_model.get(1)->coverageName(), QString("Gamma"));

    result = m_model.removeByName("Delta");
    QVERIFY(!result);
    QCOMPARE(m_model.count(), 2);

    m_model.clear();
}

void TestSMSplatListModel::testGetItem() {
    m_model.append(createTestSplatInfo("Item X", -122.3, 47.6));
    m_model.append(createTestSplatInfo("Item Y", -74.0, 40.7));

    SplatInfoPtr item = m_model.get(1);
    QVERIFY(item != nullptr);
    QCOMPARE(item->coverageName(), QString("Item Y"));
    QCOMPARE(item->transmitterLon(), -74.0);
    QCOMPARE(item->transmitterLat(), 40.7);

    item = m_model.get(99);
    QVERIFY(item == nullptr);

    m_model.clear();
}

void TestSMSplatListModel::testGetByName() {
    m_model.append(createTestSplatInfo("Tokyo", 139.7, 35.7));
    m_model.append(createTestSplatInfo("Paris", 2.3, 48.9));
    m_model.append(createTestSplatInfo("New York", -74.0, 40.7));

    SplatInfoPtr item = m_model.getByName("Paris");
    QVERIFY(item != nullptr);
    QCOMPARE(item->coverageName(), QString("Paris"));
    QCOMPARE(item->transmitterLon(), 2.3);
    QCOMPARE(item->transmitterLat(), 48.9);

    item = m_model.getByName("London");
    QVERIFY(item == nullptr);

    m_model.clear();
}

void TestSMSplatListModel::testIndexOf() {
    m_model.append(createTestSplatInfo("Apple"));
    m_model.append(createTestSplatInfo("Banana"));
    m_model.append(createTestSplatInfo("Cherry"));

    int index = m_model.indexOf("Banana");
    QCOMPARE(index, 1);

    index = m_model.indexOf("Durian");
    QCOMPARE(index, -1);

    m_model.clear();
}

void TestSMSplatListModel::testContains() {
    m_model.append(createTestSplatInfo("Apple"));
    m_model.append(createTestSplatInfo("Cherry"));

    QVERIFY(m_model.contains("Apple"));
    QVERIFY(m_model.contains("Cherry"));
    QVERIFY(!m_model.contains("Banana"));

    m_model.clear();
}

void TestSMSplatListModel::testUpdate() {
    m_model.append(createTestSplatInfo("Update Test", -122.3, 47.6));

    SplatInfoPtr updated = createTestSplatInfo("Update Test", -74.0, 40.7);
    updated->setZoomLevel(15);
    updated->setCoverageRadius(50.0);

    bool result = m_model.update(0, updated);
    QVERIFY(result);

    SplatInfoPtr item = m_model.get(0);
    QVERIFY(item != nullptr);
    QCOMPARE(item->coverageName(), QString("Update Test"));
    QCOMPARE(item->transmitterLon(), -74.0);
    QCOMPARE(item->transmitterLat(), 40.7);
    QCOMPARE(item->zoomLevel(), 15);
    QCOMPARE(item->coverageRadius(), 50.0);

    result = m_model.update(99, updated);
    QVERIFY(!result);

    m_model.clear();
}

void TestSMSplatListModel::testUpdateByName() {
    m_model.append(createTestSplatInfo("Alpha", -122.3, 47.6));
    m_model.append(createTestSplatInfo("Beta", -118.2, 34.0));

    SplatInfoPtr updated = createTestSplatInfo("Alpha", -74.0, 40.7);
    updated->setZoomLevel(12);

    bool result = m_model.updateByName("Alpha", updated);
    QVERIFY(result);

    SplatInfoPtr item = m_model.getByName("Alpha");
    QVERIFY(item != nullptr);
    QCOMPARE(item->transmitterLon(), -74.0);
    QCOMPARE(item->transmitterLat(), 40.7);
    QCOMPARE(item->zoomLevel(), 12);

    result = m_model.updateByName("Gamma", updated);
    QVERIFY(!result);

    m_model.clear();
}

void TestSMSplatListModel::testNames() {
    m_model.append(createTestSplatInfo("First"));
    m_model.append(createTestSplatInfo("Second"));
    m_model.append(createTestSplatInfo("Third"));

    QStringList names = m_model.names();
    QCOMPARE(names.size(), 3);
    QCOMPARE(names[0], QString("First"));
    QCOMPARE(names[1], QString("Second"));
    QCOMPARE(names[2], QString("Third"));

    m_model.clear();
}

void TestSMSplatListModel::testClear() {
    m_model.append(createTestSplatInfo("Item 1"));
    m_model.append(createTestSplatInfo("Item 2"));
    QCOMPARE(m_model.count(), 2);

    QSignalSpy spy(&m_model, &SMSplatListModel::countChanged);
    m_model.clear();

    QCOMPARE(m_model.count(), 0);
    QCOMPARE(spy.count(), 1);
    QVERIFY(m_model.names().isEmpty());
}

void TestSMSplatListModel::testSetItems() {
    QList<SplatInfoPtr> items;
    items.append(createTestSplatInfo("Bulk 1"));
    items.append(createTestSplatInfo("Bulk 2"));
    items.append(createTestSplatInfo("Bulk 3"));

    QSignalSpy spy(&m_model, &SMSplatListModel::countChanged);
    m_model.setItems(items);

    QCOMPARE(m_model.count(), 3);
    QCOMPARE(spy.count(), 1);

    QCOMPARE(m_model.get(0)->coverageName(), QString("Bulk 1"));
    QCOMPARE(m_model.get(1)->coverageName(), QString("Bulk 2"));
    QCOMPARE(m_model.get(2)->coverageName(), QString("Bulk 3"));

    QList<SplatInfoPtr> retrievedItems = m_model.items();
    QCOMPARE(retrievedItems.size(), 3);
    QCOMPARE(retrievedItems[0]->coverageName(), QString("Bulk 1"));

    m_model.clear();
}

void TestSMSplatListModel::testCountProperty() {
    QCOMPARE(m_model.property("count").toInt(), 0);

    m_model.append(createTestSplatInfo("Count Test 1"));
    QCOMPARE(m_model.property("count").toInt(), 1);

    m_model.append(createTestSplatInfo("Count Test 2"));
    QCOMPARE(m_model.property("count").toInt(), 2);

    m_model.remove(0);
    QCOMPARE(m_model.property("count").toInt(), 1);

    m_model.clear();
    QCOMPARE(m_model.property("count").toInt(), 0);
}

void TestSMSplatListModel::testRoleNames() {
    QHash<int, QByteArray> roles = m_model.roleNames();

    QVERIFY(roles.contains(SMSplatListModel::NameRole));
    QVERIFY(roles.contains(SMSplatListModel::ImagePathRole));
    QVERIFY(roles.contains(SMSplatListModel::TransmitterLonRole));
    QVERIFY(roles.contains(SMSplatListModel::TransmitterLatRole));
    QVERIFY(roles.contains(SMSplatListModel::SplatInfoRole));

    QCOMPARE(roles[SMSplatListModel::NameRole], QByteArray("name"));
    QCOMPARE(roles[SMSplatListModel::ImagePathRole], QByteArray("imagePath"));
    QCOMPARE(roles[SMSplatListModel::TransmitterLonRole], QByteArray("transmitterLon"));
    QCOMPARE(roles[SMSplatListModel::TransmitterLatRole], QByteArray("transmitterLat"));
    QCOMPARE(roles[SMSplatListModel::SplatInfoRole], QByteArray("splatInfo"));
}

void TestSMSplatListModel::testDataRoles() {
    SplatInfoPtr info = createTestSplatInfo("Data Roles Test", -122.3, 47.6);
    info->setZoomLevel(12);
    info->setCoverageRadius(35.0);
    m_model.append(info);

    QModelIndex index = m_model.index(0, 0);

    QCOMPARE(m_model.data(index, SMSplatListModel::NameRole).toString(),
             QString("Data Roles Test"));
    QCOMPARE(m_model.data(index, SMSplatListModel::ImagePathRole).toString(),
             QString("Data Roles Test.png"));
    QCOMPARE(m_model.data(index, SMSplatListModel::TransmitterLonRole).toDouble(), -122.3);
    QCOMPARE(m_model.data(index, SMSplatListModel::TransmitterLatRole).toDouble(), 47.6);
    QCOMPARE(m_model.data(index, SMSplatListModel::ZoomLevelRole).toInt(), 12);
    QCOMPARE(m_model.data(index, SMSplatListModel::CoverageRadiusRole).toDouble(), 35.0);

    QVariant variant = m_model.data(index, SMSplatListModel::SplatInfoRole);
    QVERIFY(variant.isValid());
    SplatInfoPtr retrievedInfo = variant.value<SplatInfoPtr>();
    QVERIFY(retrievedInfo != nullptr);
    QCOMPARE(retrievedInfo->coverageName(), QString("Data Roles Test"));
    QCOMPARE(retrievedInfo->transmitterLon(), -122.3);

    QModelIndex invalidIndex = m_model.index(99, 0);
    QVERIFY(!m_model.data(invalidIndex, SMSplatListModel::NameRole).isValid());

    m_model.clear();
}

QTEST_MAIN(TestSMSplatListModel)
#include "test_smsplat_list_model.moc"
