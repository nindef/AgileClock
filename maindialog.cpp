#include "maindialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QTimer>
#include <QFont>
#include <QPainter>
#include <QImage>
#include <QPushButton>
#include <QPalette>
#include <QBrush>
#include <QStyle>
#include <QtSvg/QSvgRenderer>
#include <QGraphicsDropShadowEffect>

#include <stdio.h>

#include <QDebug>

inline void updateWidgetStyle (QWidget* lWidget)
{
   lWidget->style ()->unpolish (lWidget);
   lWidget->style ()->polish (lWidget);
   lWidget->update ();
}

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
{
    setContentsMargins(0,0,0,0);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.8);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);


    QGridLayout *lpGlobalLay = new QGridLayout;
    lpGlobalLay->setContentsMargins(0,0,0,0);
    lpGlobalLay->setSpacing(0);

    QPushButton *lpCloseButton = new QPushButton(this); lpCloseButton->setContentsMargins(0,0,0,0);
    lpCloseButton->setIcon(QIcon (QPixmap(":/images/resources/img_close.png")));
    lpCloseButton->setFlat(true);
    lpCloseButton->setObjectName("closeButton");
    connect (lpCloseButton, SIGNAL(clicked(bool)), this, SLOT(close()));

    MainFrame *lpMainFrame = new MainFrame(this);
    connect (this, SIGNAL(signalResetClock(bool)), lpMainFrame, SLOT(resetClock(bool)));

    lpGlobalLay->setRowStretch(0,1);
    lpGlobalLay->setRowStretch(3,1);
    lpGlobalLay->setColumnStretch(0,1);
    lpGlobalLay->setColumnStretch(2,1);
    lpGlobalLay->addWidget(lpCloseButton,1,1,1,1,Qt::AlignCenter);
    lpGlobalLay->addWidget(lpMainFrame,2,1,1,1,Qt::AlignRight);

    setLayout(lpGlobalLay);
}

MainDialog::~MainDialog()
{

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
    if(event->buttons() != Qt::LeftButton) return;

    QPoint position = event->pos();
    QPoint despl = position - oldPosition;

    move(pos () + despl);
    acumDespl += despl;
}

void MainDialog::mouseReleaseEvent(QMouseEvent * event)
{
    if(event->button() == Qt::RightButton) return;
    if(acumDespl.manhattanLength() < 3 && mbTimerOn)
    {
        emit signalResetClock(true);
    }
    mbTimerOn = false;
}

void MainDialog::mouseDoubleClickEvent(QMouseEvent * event)
{
    emit signalResetClock(false);
    mbTimerOn = false;
}





MainFrame::MainFrame(QWidget* parent):QFrame(parent)
{
    setContentsMargins(0,0,0,0);

    miMinutes = 2, miSeconds = 0;

    moTimeFont.setFamily("Ubuntu Mono");

    QHBoxLayout *lpGlobalHLay = new QHBoxLayout;
    lpGlobalHLay->setContentsMargins(0,0,0,0);
    lpGlobalHLay->setSpacing(0);

    mpMinFrame = new QFrame(this); mpMinFrame->setContentsMargins(0,0,0,0);
    QHBoxLayout *mpMinFrameLay = new QHBoxLayout(this); mpMinFrameLay->setContentsMargins(0,0,0,0); mpMinFrameLay->setSpacing(0);
    mpSecFrame = new QFrame(this); mpSecFrame->setContentsMargins(0,0,0,0);
    QHBoxLayout *mpSecFrameLay = new QHBoxLayout(this); mpSecFrameLay->setContentsMargins(0,0,0,0); mpSecFrameLay->setSpacing(0);

    QGraphicsDropShadowEffect* minEffect = new QGraphicsDropShadowEffect (this);
    minEffect->setOffset(QPoint(0,0));
    minEffect->setColor (QColor (0, 0, 0, 255));
    minEffect->setBlurRadius (16);
    QGraphicsDropShadowEffect* secEffect = new QGraphicsDropShadowEffect (this);
    secEffect->setOffset(QPoint(0,0));
    secEffect->setColor (QColor (0, 0, 0, 255));
    secEffect->setBlurRadius (16);
    QGraphicsDropShadowEffect* sepEffect = new QGraphicsDropShadowEffect (this);
    sepEffect->setOffset(QPoint(0,0));
    sepEffect->setColor (QColor (0, 0, 0, 255));
    sepEffect->setBlurRadius (16);

    mpMinutes = new QLabel(this);
    mpMinutes->setContentsMargins(0,0,0,0);
    mpMinutes->setText(QString::number (miMinutes).rightJustified(2,QChar('0')));
    mpMinutes->setObjectName("timeLabel");
    mpMinutes->setGraphicsEffect (minEffect);

    mpSeconds = new QLabel(this);
    mpSeconds->setContentsMargins(0,0,0,0);
    mpSeconds->setText(QString::number (miSeconds).rightJustified(2,QChar('0')));
    mpSeconds->setObjectName("timeLabel");
    mpSeconds->setGraphicsEffect (secEffect);

    mpMinFrameLay->addWidget(mpMinutes);
    mpMinFrame->setLayout(mpMinFrameLay);

    mpSecFrameLay->addWidget(mpSeconds);
    mpSecFrame->setLayout(mpSecFrameLay);

    separator = new QLabel(":",this);
    separator->setContentsMargins(0,0,0,0);
    separator->setObjectName("timeLabel");
    separator->setGraphicsEffect (sepEffect);

    mpMinutes->setFont(moTimeFont);
    mpSeconds->setFont(moTimeFont);
    separator->setFont(moTimeFont);

    mpMinutes->setProperty("ColorInterval", miColorInterval);
    mpSeconds->setProperty("ColorInterval", miColorInterval);
    separator->setProperty("ColorInterval", miColorInterval);
    mpMinutes->setProperty("FontSize", miFontSizeInterval);
    mpSeconds->setProperty("FontSize", miFontSizeInterval);
    separator->setProperty("FontSize", miFontSizeInterval);

    lpGlobalHLay->addWidget(mpMinFrame);
    lpGlobalHLay->addWidget(separator);
    lpGlobalHLay->addWidget(mpSecFrame);

    mpTimer = new QTimer(this);
    mpTimer->setInterval(1000);

    connect(mpTimer, SIGNAL(timeout()), this, SLOT(secLess()));

    setLayout(lpGlobalHLay);
}


void MainFrame::secLess()
{
    int mins = mpMinutes->text ().toInt();
    int secs = mpSeconds->text ().toInt() + ((mbTimeout)? 1 : - 1);
    if(secs < 0 || secs > 59)
    {
        mins += ((mbTimeout)? 1 : - 1);
        if (mins < 0)
        {
            mins = 0;
            mbTimeout = true;
            secs = 1;
        }
        else
        {
            if(mbTimeout && secs > 59) secs = 0;
            else secs = 59;
        }
    }
    mpMinutes->setText(QString::number (mins).rightJustified(2,QChar('0')));
    mpSeconds->setText(QString::number (secs).rightJustified(2,QChar('0')));

    if(mins == 0 && secs == 4) QSound::play(":/sounds/resources/Metronome.wav");
    else if (mins == 0 && secs == 0) QSound::play(":/sounds/resources/AirHorn-mike_koenig.wav");

    if(miColorInterval < 5)
    {
        int liOldInterval = miColorInterval;
        miColorInterval = (miMinutes * 60 - mins * 60 + miSeconds - secs) / mfColorIntervalJump;

        if(liOldInterval != miColorInterval)
        {
            mpMinutes->setProperty("ColorInterval", miColorInterval);
            mpSeconds->setProperty("ColorInterval", miColorInterval);
            separator->setProperty("ColorInterval", miColorInterval);

            updateWidgetStyle (mpMinutes);
            updateWidgetStyle (mpSeconds);
            updateWidgetStyle (separator);
        }
    }
}

void MainFrame::wheelEvent(QWheelEvent * event)
{
    if(event->modifiers() != Qt::CTRL)
    {
        if(mpTimer->isActive()) resetClock(false);

        if(mpMinFrame->geometry().contains(event->pos()))
        {
            int mins = mpMinutes->text ().toInt();
            if(event->delta() < 0) mins = qMax(--mins,0);
            else mins = qMin(++mins,99);
            miMinutes = mins;
            mpMinutes->setText(QString::number (mins).rightJustified(2,QChar('0')));
        }
        else if(mpSecFrame->geometry().contains(event->pos()))
        {
            int secs = mpSeconds->text ().toInt();
            if(event->delta() < 0)
            {
                secs -= 1;
                if(secs < 0) secs = 59;
            }
            else
            {
                secs += 1;
                if(secs > 59) secs = 0;
            }
            miSeconds = secs;
            mpSeconds->setText(QString::number (secs).rightJustified(2,QChar('0')));
        }
    }
    else
    {
        if(event->delta() < 0)
        {
            //mes petita
            miFontSizeInterval--;
            if(miFontSizeInterval < 0) miFontSizeInterval = 0;
        }
        else
        {
            //mes gran
            miFontSizeInterval++;
            if(miFontSizeInterval > 8) miFontSizeInterval = 8;
        }
        mpMinutes->setProperty("FontSize", miFontSizeInterval);
        mpSeconds->setProperty("FontSize", miFontSizeInterval);
        separator->setProperty("FontSize", miFontSizeInterval);
        updateWidgetStyle (mpMinutes);
        updateWidgetStyle (mpSeconds);
        updateWidgetStyle (separator);
    }
}

void MainFrame::resetClock(bool lbTimerActive)
{
    mpMinutes->setText(QString::number (miMinutes).rightJustified(2,QChar('0')));
    mpSeconds->setText(QString::number (miSeconds).rightJustified(2,QChar('0')));

    mfColorIntervalJump = (qreal)(miMinutes * 60 + miSeconds) / 5.0;
    miColorInterval = 0;
    mpMinutes->setProperty("ColorInterval", miColorInterval);
    mpSeconds->setProperty("ColorInterval", miColorInterval);
    separator->setProperty("ColorInterval", miColorInterval);
    updateWidgetStyle (mpMinutes);
    updateWidgetStyle (mpSeconds);
    updateWidgetStyle (separator);

    mbTimeout = false;

    if (lbTimerActive) mpTimer->start ();
    else mpTimer->stop();

    mbTimerOn = lbTimerActive;
}
