#include <QtTest/QtTest>
#include <QtQuickTest/QtQuickTest>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCoreApplication>
#include <QQmlContext>

#include "QtSMSplat.h"
#include "Type.h"

class TestQtSMSplatUI : public QObject {
    Q_OBJECT

   private slots:
    void initTestCase();
    void cleanupTestCase();

    void testInitialState();
    void testCreateDialog();
    void testImportDialog();
    void testDeleteConfirmation();
    void testClearList();
    void testSaveAndUpdate();

   private:
    QQmlApplicationEngine engine;
    QQuickWindow* window = nullptr;
    QtSMSplat* smsplat = nullptr;
    QTemporaryDir tempDir;

    // Helper methods
    QQuickItem* findItemByObjectName(QQuickItem* parent, const QString& name);
    void clickButton(const QString& buttonObjectName);
    QVariant evaluateQml(QVariant expression);
    void waitForQmlSignal(QObject* obj, const char* signal, int timeout = 1000);
};

void TestQtSMSplatUI::initTestCase() {
    // Register QtSMSplat types
    QtSMSplat::registerQtSMSplatQmlTypes();
    smsplat = qobject_cast<QtSMSplat*>(QtSMSplat::createQtSMSplatSingleton(nullptr, nullptr));

    engine.addImageProvider("memory", smsplat->imageProvider());
    // Get QtSMSplat singleton instance
    smsplat = qobject_cast<QtSMSplat*>(QtSMSplat::createQtSMSplatSingleton(nullptr, nullptr));
    QVERIFY(smsplat != nullptr);

    // Initialize the database with a temporary directory
    QVERIFY(tempDir.isValid());
    QString dbPath = tempDir.path() + "/test.db";
    QVERIFY(smsplat->initDatabase(dbPath));

    // Load the QML file
    engine.load(QUrl::fromLocalFile(QFINDTESTDATA("../qml/Test.qml")));

    QObject* topLevel = engine.rootObjects().value(0);
    QVERIFY(topLevel != nullptr);

    window = qobject_cast<QQuickWindow*>(topLevel);
    QVERIFY(window != nullptr);
    window->show();

    // Wait for window to be shown
    QVERIFY(QTest::qWaitForWindowExposed(window));
}

void TestQtSMSplatUI::cleanupTestCase() {
    window->close();
    engine.clearComponentCache();
}

void TestQtSMSplatUI::testInitialState() {
    // Verify the model is empty
    QCOMPARE(smsplat->splatModel()->count(), 0);

    // Verify the list view is empty
    QQuickItem* splatList = window->findChild<QQuickItem*>("splatList");
    QVERIFY(splatList != nullptr);
    QCOMPARE(evaluateQml(QVariant("SMSplat.splatModel.count")).toInt(), 0);
}

void TestQtSMSplatUI::testCreateDialog() {
    // Open the create dialog
    clickButton("createNewButton");

    // Find the dialog
    QQuickItem* createDialog = window->findChild<QQuickItem*>("createDialog");
    QVERIFY(createDialog != nullptr);
    QVERIFY(evaluateQml("createDialog.visible").toBool());

    // Fill in some test data
    evaluateQml("createDialog.inputInfo.transmitterSite = 'Test Site'");
    evaluateQml("createDialog.inputInfo.itmCovType = 'full'");
    evaluateQml("createDialog.inputInfo.receiverHeight = 10.0");
    evaluateQml("createDialog.inputInfo.startAngle = 0.0");
    evaluateQml("createDialog.inputInfo.endAngle = 360.0");
    evaluateQml("createDialog.inputInfo.radius = 25.0");

    // Click the generate button
    QSignalSpy generateSpy(smsplat, SIGNAL(splatGenerated(const QString&)));
    clickButton("generateButton");

    // Wait for the signal or timeout
    QTRY_VERIFY_WITH_TIMEOUT(generateSpy.count() > 0, 5000);

    // Verify the model has been updated
    QCOMPARE(smsplat->splatModel()->count(), 1);

    // Close the dialog
    QVERIFY(!evaluateQml("createDialog.visible").toBool());
}

void TestQtSMSplatUI::testImportDialog() {
    // First save an item to the database
    QString name = smsplat->splatModel()->get(0)->coverageName();
    QVERIFY(smsplat->save(name));

    // Clear the model
    smsplat->clearList();
    QCOMPARE(smsplat->splatModel()->count(), 0);

    // Open the import dialog
    clickButton("importButton");

    // Find the dialog
    QQuickItem* importDialog = window->findChild<QQuickItem*>("importDialog");
    QVERIFY(importDialog != nullptr);
    QVERIFY(evaluateQml("importDialog.visible").toBool());

    // Wait for the available splats to be populated
    QTRY_VERIFY_WITH_TIMEOUT(evaluateQml("importDialog.availableSplats.length").toInt() > 0, 1000);

    // Select the first item
    evaluateQml("importDialog.selectedSplats = [importDialog.availableSplats[0]]");

    // Click import button
    QSignalSpy importSpy(smsplat, SIGNAL(splatImported(const QStringList&)));
    clickButton("importSelectedButton");

    // Wait for the signal or timeout
    QTRY_VERIFY_WITH_TIMEOUT(importSpy.count() > 0, 1000);

    // Verify the model has been updated
    QCOMPARE(smsplat->splatModel()->count(), 1);

    // Close the dialog
    QVERIFY(!evaluateQml("importDialog.visible").toBool());
}

void TestQtSMSplatUI::testDeleteConfirmation() {
    // Assuming we have at least one item in the model
    QVERIFY(smsplat->splatModel()->count() > 0);

    // Get the name of the first item
    QString name = smsplat->splatModel()->get(0)->coverageName();

    // Select the first item in the list
    evaluateQml("splatList.currentIndex = 0");

    // Click the delete button
    clickButton("deleteButton");

    // Find the confirmation dialog
    QQuickItem* confirmDialog = window->findChild<QQuickItem*>("confirmDeleteDialog");
    QVERIFY(confirmDialog != nullptr);
    QVERIFY(evaluateQml("confirmDeleteDialog.visible").toBool());

    // Click the "Delete from List Only" button
    QSignalSpy deleteSpy(smsplat, SIGNAL(splatDeleted(const QString&)));
    clickButton("deleteListOnlyButton");

    // Wait for the signal or timeout
    QTRY_VERIFY_WITH_TIMEOUT(deleteSpy.count() > 0, 1000);

    // Verify the model has been updated
    QCOMPARE(smsplat->splatModel()->count(), 0);

    // Verify the dialog is closed
    QVERIFY(!evaluateQml("confirmDeleteDialog.visible").toBool());
}

void TestQtSMSplatUI::testClearList() {
    // Add a test item
    auto info = std::make_shared<QtSMSplatGenInfo>();
    info->setCoverageName("Clear Test");
    smsplat->splatModel()->append(info);

    QCOMPARE(smsplat->splatModel()->count(), 1);

    // Click the clear button
    clickButton("clearListButton");

    // Verify the model is empty
    QCOMPARE(smsplat->splatModel()->count(), 0);
}

void TestQtSMSplatUI::testSaveAndUpdate() {
    // Add a test item
    auto info = std::make_shared<QtSMSplatGenInfo>();
    info->setCoverageName("Save Test");
    smsplat->splatModel()->append(info);

    // Select the item
    evaluateQml("splatList.currentIndex = 0");

    // Click the save button
    QSignalSpy saveSpy(smsplat, SIGNAL(splatSaved(const QString&)));
    clickButton("saveButton");

    // Wait for the signal or timeout
    QTRY_VERIFY_WITH_TIMEOUT(saveSpy.count() > 0, 1000);

    // Verify the item is marked as saved
    QVERIFY(smsplat->splatModel()->getByName("Save Test")->isSavedInDb());

    // Now the button should show "Update" instead of "Save"
    QCOMPARE(evaluateQml("saveButton.text").toString(), QString("Update"));

    // Change something in the item
    smsplat->splatModel()->getByName("Save Test")->setImageWidth(200);

    // Click the update button
    saveSpy.clear();
    clickButton("saveButton");

    // Wait for the signal or timeout
    QTRY_VERIFY_WITH_TIMEOUT(saveSpy.count() > 0, 1000);

    // Verify the update was successful
    QVERIFY(smsplat->splatModel()->getByName("Save Test")->isSavedInDb());
}

// Helper methods implementation
QQuickItem* TestQtSMSplatUI::findItemByObjectName(QQuickItem* parent, const QString& name) {
    if (!parent) return nullptr;

    if (parent->objectName() == name) return parent;

    foreach (QQuickItem* child, parent->childItems()) {
        QQuickItem* found = findItemByObjectName(child, name);
        if (found) return found;
    }

    return nullptr;
}

void TestQtSMSplatUI::clickButton(const QString& buttonObjectName) {
    QQuickItem* button = window->findChild<QQuickItem*>(buttonObjectName);
    if (!button) {
        // Try to find it recursively
        button = findItemByObjectName(window->contentItem(), buttonObjectName);
    }

    QVERIFY2(button != nullptr, qPrintable(QString("Button %1 not found").arg(buttonObjectName)));

    // Get the center point of the button
    QPoint center =
        button->mapToScene(QPointF(button->width() / 2, button->height() / 2)).toPoint();

    // Simulate mouse click
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, center);
}

QVariant TestQtSMSplatUI::evaluateQml(QVariant expression) {
    QVariant result;
    QMetaObject::invokeMethod(window, "evaluateQml", Q_RETURN_ARG(QVariant, result),
                              Q_ARG(QVariant, expression));
    return result;
}

void TestQtSMSplatUI::waitForQmlSignal(QObject* obj, const char* signal, int timeout) {
    QSignalSpy spy(obj, signal);
    QVERIFY(spy.wait(timeout));
}

QTEST_MAIN(TestQtSMSplatUI)
#include "test_QtSMSplat_UI.moc"
