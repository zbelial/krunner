#include <QApplication>
#include <QLineEdit>
#include <QListView>
#include <QVBoxLayout>
#include <QWidget>

#include "resultsmodel.h"

using namespace KRunner;

class TestObject : public QWidget
{
    Q_OBJECT
public:
    explicit TestObject()
        : QWidget()
    {
        ResultsModel *smodel = new ResultsModel(this);
        // Connemt this line out to load all the system runners
        smodel->runnerManager()->loadRunner(KPluginMetaData::findPluginById(QStringLiteral("krunnertest"), QStringLiteral("fakerunnerplugin")));

        QListView *view = new QListView(this);
        view->setModel(smodel);
        view->setAlternatingRowColors(true);

        QLineEdit *edit = new QLineEdit(this);
        connect(edit, &QLineEdit::textChanged, smodel, &ResultsModel::setQueryString);
        edit->setText("foo");

        QVBoxLayout *l = new QVBoxLayout(this);
        l->addWidget(edit);
        l->addWidget(view);
    }
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    TestObject obj;
    obj.show();

    return app.exec();
}

#include "modelwidgettest.moc"
