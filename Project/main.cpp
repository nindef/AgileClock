#include "maindialog.h"
#include <QApplication>
#include <QFontDatabase>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile cssfile(":/styles/resources/style01.css");
    cssfile.open(QFile::ReadOnly);

    auto StyleSheet = QLatin1String(cssfile.readAll());
    a.setStyleSheet(StyleSheet);

    QFontDatabase::addApplicationFont(":/fonts/resources/UbuntuMono-B.ttf");

    auto dlg = new MainDialog;
    dlg->show();

    return a.exec();
}
