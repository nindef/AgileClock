#include "maindialog.h"
#include <QFont>
#include <QPainter>
#include <QImage>
#include <QPushButton>
#include <QPalette>
#include <QBrush>
#include <QDesktopWidget>
#include <QGridLayout>
#include <QDebug>
#include <QSettings>
#include <QApplication>

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
{
    setContentsMargins(0,0,0,0);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.8);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);

    auto lpGlobalLay = new QGridLayout;
    lpGlobalLay->setContentsMargins(0,0,0,0);
    lpGlobalLay->setSpacing(0);

    auto lpCloseButton = new QPushButton(this);
    lpCloseButton->setContentsMargins(0,0,0,0);
    lpCloseButton->setIcon(QIcon (QPixmap(":/images/resources/img_close.png")));
    lpCloseButton->setFlat(true);
    lpCloseButton->setObjectName("closeButton");
    connect (lpCloseButton, SIGNAL(clicked(bool)), this, SLOT(close()));

    auto lpMainFrame = new MainFrame(this);
    connect (this, SIGNAL(signalResetClock(bool)), lpMainFrame, SLOT(resetClock(bool)));

    lpGlobalLay->setRowStretch(0,1);
    lpGlobalLay->setRowStretch(3,1);
    lpGlobalLay->setColumnStretch(0,1);
    lpGlobalLay->setColumnStretch(2,1);
    lpGlobalLay->addWidget(lpCloseButton,1,1,1,1,Qt::AlignCenter);
    lpGlobalLay->addWidget(lpMainFrame,2,1,1,1,Qt::AlignRight);

    setLayout(lpGlobalLay);

    msSettingsPath = QApplication::applicationDirPath() + "/config.ini";
}

MainDialog::~MainDialog()
{
    //save current settings
    auto geom = geometry();

    QSettings settings(msSettingsPath, QSettings::IniFormat);
    settings.setValue("lastPos", geom.topLeft());
}

void MainDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    auto center = QApplication::desktop()->screen()->rect().center() - rect().center();
    QSettings settings(msSettingsPath, QSettings::IniFormat);
    auto lastPos = settings.value("lastPos", center).toPoint();

    move(lastPos);
}

void MainDialog::mousePressEvent(QMouseEvent * event)
{
    if(event->button() == Qt::LeftButton)
    {
        oldPosition= event->pos();
        acumDespl = QPoint(0,0);
    }

    mbTimerOn = true;
}

void MainDialog::mouseMoveEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        auto position = event->pos();
        auto despl = position - oldPosition;

        move(pos () + despl);
        acumDespl += despl;
    }
}

void MainDialog::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (acumDespl.manhattanLength() < 3 && mbTimerOn)
        emit signalResetClock(true);

        mbTimerOn = false;
    }
}

void MainDialog::mouseDoubleClickEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit signalResetClock(false);
        mbTimerOn = false;
    }
}


