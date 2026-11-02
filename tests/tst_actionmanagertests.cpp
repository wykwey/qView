#include <QtTest>
#include <QToolBar>
#include <QSettings>
#include <QTemporaryDir>

#include "qvapplication.h"

class ActionManagerTests : public QObject
{
    Q_OBJECT

public:
    ActionManagerTests();
    ~ActionManagerTests();

private slots:
    void testClonedActionsUntracked();
    void testBottomToolbar();
};

ActionManagerTests::ActionManagerTests() { }

ActionManagerTests::~ActionManagerTests() { }

void ActionManagerTests::testClonedActionsUntracked()
{
    // Get initial counts of certain actions
    int fullscreenCount = qvApp->getActionManager().getAllInstancesOfAction("fullscreen").length();
    int openCount = qvApp->getActionManager().getAllInstancesOfAction("open").length();
    qDebug() << fullscreenCount;

    // Have window clone actions
    MainWindow window;
    window.setAttribute(Qt::WA_DeleteOnClose, false);
    window.show();
    // Make sure they were cloned
    QVERIFY(qvApp->getActionManager().getAllInstancesOfAction("fullscreen").length()
            != fullscreenCount);
    QVERIFY(qvApp->getActionManager().getAllInstancesOfAction("open").length() != openCount);
    // Untrack them
    window.close();

    // Make sure the count has not changed from the initial
    QCOMPARE(qvApp->getActionManager().getAllInstancesOfAction("fullscreen").length(),
             fullscreenCount);
    QCOMPARE(qvApp->getActionManager().getAllInstancesOfAction("open").length(), openCount);
}

void ActionManagerTests::testBottomToolbar()
{
    MainWindow window;
    window.setAttribute(Qt::WA_DeleteOnClose, false);
    window.resize(1000, 600);
    window.show();

    auto *toolbar = window.findChild<QToolBar *>("bottomToolBar");
    auto *view = window.findChild<QVGraphicsView *>();
    auto *hideTimer = window.findChild<QTimer *>("bottomToolBarHideTimer");
    QVERIFY(toolbar);
    QVERIFY(view);
    QVERIFY(hideTimer);
    QVERIFY(toolbar->isVisible());
    QCOMPARE(toolbar->parentWidget(), window.centralWidget());

    const QStringList expectedKeys = { "previousfile", "zoomin", "rotateright", "zoomout",
                                       "nextfile" };
    QStringList actualKeys;
    for (auto *action : toolbar->actions()) {
        actualKeys.append(action->data().toStringList().first());
        QVERIFY(!action->isEnabled());
    }
    QCOMPARE(actualKeys, expectedKeys);

    // Hiding and revealing the overlay must not resize the image.
    const QSize viewportSize = view->viewport()->size();
    hideTimer->setInterval(50);
    QTest::mouseMove(view->viewport(), QPoint(10, 10));
    hideTimer->start();
    QTRY_VERIFY(toolbar->isHidden());
    QCOMPARE(view->viewport()->size(), viewportSize);

    QTest::mouseMove(view->viewport(),
                     QPoint(view->viewport()->width() / 2, view->viewport()->height() - 1));
    QTRY_VERIFY(toolbar->isVisible());
    QCOMPARE(view->viewport()->size(), viewportSize);

    QTest::mouseMove(toolbar, toolbar->rect().center());
    QTest::qWait(100);
    QVERIFY(toolbar->isVisible());
    QTest::mouseMove(view->viewport(), QPoint(10, 10));
    QTRY_VERIFY(toolbar->isHidden());

    window.close();
}

int main(int argc, char *argv[])
{
    QTemporaryDir settingsDir;
    if (!settingsDir.isValid())
        return 1;
    QCoreApplication::setOrganizationName("qViewTests");
    QCoreApplication::setApplicationName("qViewTests");
    QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, settingsDir.path());
    QSettings settings;
    settings.setValue("firstlaunch", true);
    settings.setValue("options/updatenotifications", false);
    QVApplication app(argc, argv);
    ActionManagerTests actionManagerTests;
    return QTest::qExec(&actionManagerTests, argc, argv);
}

#include "tst_actionmanagertests.moc"
