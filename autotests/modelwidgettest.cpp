/*
    SPDX-FileCopyrightText: 2023 Alexander Lohnau <alexander.lohnau@gmx.de>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QApplication>
#include <QLineEdit>
#include <QListView>
#include <QVBoxLayout>
#include <QWidget>

#include <KRunner/ResultsModel>

using namespace KRunner;

class TestObject : public QWidget
{
    Q_OBJECT
public:
    explicit TestObject()
        : QWidget()
    {
        ResultsModel *model = new ResultsModel(this);
        // Comment this line out to load all the system runners
        if (KPluginMetaData data(QStringLiteral("kf6/krunner/krunner_services")); data.isValid()) {
            model->runnerManager()->loadRunner(data);
        } else {
            qFatal("Invalid metadata object");
        }

        QListView *view = new QListView(this);
        view->setModel(model);
        view->setAlternatingRowColors(true);

        QLineEdit *edit = new QLineEdit(this);
        connect(edit, &QLineEdit::textChanged, model, &ResultsModel::setQueryString);
        edit->setFocus(Qt::OtherFocusReason);

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
