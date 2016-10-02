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
    QString StyleSheet = QLatin1String(cssfile.readAll());
    a.setStyleSheet(StyleSheet);

//    int liFontIndex = QFontDatabase::addApplicationFont(":/fonts/resources/DS-DIGI.TTF");
    int liFontIndex = QFontDatabase::addApplicationFont(":/fonts/resources/UbuntuMono-B.ttf");
    QStringList families = QFontDatabase::applicationFontFamilies(liFontIndex);
    foreach (QString family, families)
        qDebug() << family;

//    liFontIndex = QFontDatabase::addApplicationFont(":/fonts/resources/DS-DIGIB.TTF");


    MainWindow w;

    return a.exec();
}
