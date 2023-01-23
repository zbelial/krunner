/*
    SPDX-FileCopyrightText: 2021 Alexander Lohnau <alexander.lohnau@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "abstractrunner.h"
#include "fakerunner.h"
#include "runnermanager.h"

#include <QAction>
#include <QObject>
#include <QStandardPaths>
#include <QTest>

#include "kpluginmetadata_utils_p.h"

using namespace KRunner;

namespace
{
inline QueryMatch createMatch(const QString &id, AbstractRunner *r = nullptr)
{
    QueryMatch m(r);
    m.setId(id);
    return m;
}
}

class RunnerContextMatchMethodsTest : public QObject
{
    Q_OBJECT
public:
    RunnerContextMatchMethodsTest();
    ~RunnerContextMatchMethodsTest() override;

    std::unique_ptr<RunnerContext> ctx;
    FakeRunner *runner1;
    FakeRunner *runner2;
private Q_SLOTS:
    void init();
    void testAdd();
    void testAddMulti();
    void testDuplicateIds();
};

RunnerContextMatchMethodsTest::RunnerContextMatchMethodsTest()
    : QObject()
    , runner1(new FakeRunner())
    , runner2(new FakeRunner())
{
    QStandardPaths::setTestModeEnabled(true);
    const QByteArray defaultDataDirs = qEnvironmentVariableIsSet("XDG_DATA_DIRS") ? qgetenv("XDG_DATA_DIRS") : QByteArray("/usr/local:/usr");
    const QByteArray modifiedDataDirs = QFile::encodeName(QCoreApplication::applicationDirPath()) + QByteArrayLiteral("/data:") + defaultDataDirs;
    qputenv("XDG_DATA_DIRS", modifiedDataDirs);
    KPluginMetaData data1 = parseMetaDataFromDesktopFile(QFINDTESTDATA("metadatafile1.desktop"));
    KPluginMetaData data2 = parseMetaDataFromDesktopFile(QFINDTESTDATA("metadatafile2.desktop"));
    QVERIFY(data1.isValid());
    QVERIFY(data2.isValid());
    runner1 = new FakeRunner(data1);
    runner2 = new FakeRunner(data2);
}

RunnerContextMatchMethodsTest::~RunnerContextMatchMethodsTest()
{
    delete runner1;
    delete runner2;
}

void RunnerContextMatchMethodsTest::init()
{
    ctx.reset(new RunnerContext());
}

void RunnerContextMatchMethodsTest::testAdd()
{
    QVERIFY(ctx->matches().isEmpty());
    QVERIFY(ctx->addMatch(createMatch(QStringLiteral("m1"))));
    QVERIFY(ctx->addMatch(createMatch(QStringLiteral("m2"))));
    QCOMPARE(ctx->matches().count(), 2);
    QVERIFY(ctx->addMatch(createMatch(QStringLiteral("m3"))));
    QCOMPARE(ctx->matches().count(), 3);
}

void RunnerContextMatchMethodsTest::testAddMulti()
{
    QVERIFY(ctx->matches().isEmpty());
    QVERIFY(ctx->addMatches({createMatch(QStringLiteral("m1")), createMatch(QStringLiteral("m2"))}));
    QCOMPARE(ctx->matches().count(), 2);
}

void RunnerContextMatchMethodsTest::testDuplicateIds()
{
    const QueryMatch match1 = createMatch(QStringLiteral("id1"), runner1);
    QVERIFY(ctx->addMatch(match1));
    const QueryMatch match2 = createMatch(QStringLiteral("id1"), runner2);
    QVERIFY(ctx->addMatch(match2));
    const QueryMatch match3 = createMatch(QStringLiteral("id2"), runner1);
    QVERIFY(ctx->addMatch(match3));
    const QueryMatch match4 = createMatch(QStringLiteral("id3"), runner2);
    QVERIFY(ctx->addMatch(match4));
    const QueryMatch match5 = createMatch(QStringLiteral("id3"), runner2);
    QVERIFY(ctx->addMatch(match5));

    const QList<QueryMatch> matches = ctx->matches();
    QCOMPARE(matches.size(), 3);
    // match2 should have replaced match1
    QCOMPARE(matches.at(0), match2);
    QCOMPARE(matches.at(1), match3);
    // match4 should not have been replaced, the runner does not have the weak property set
    QCOMPARE(matches.at(2), match4);
}


QTEST_MAIN(RunnerContextMatchMethodsTest)

#include "runnermatchmethodstest.moc"
