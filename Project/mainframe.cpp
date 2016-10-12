#include "mainframe.h"

#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QSound>
#include <QStyle>


inline void updateWidgetStyle (QWidget* lWidget)
{
   lWidget->style ()->unpolish (lWidget);
   lWidget->style ()->polish (lWidget);
   lWidget->update ();
}


MainFrame::MainFrame(QWidget* parent):QFrame(parent)
{
    setContentsMargins(0,0,0,0);

    loadSettings();

    QHBoxLayout *lpGlobalHLay = new QHBoxLayout;
    lpGlobalHLay->setContentsMargins(0,0,0,0);
    lpGlobalHLay->setSpacing(0);

    mpMinFrame->setContentsMargins(0,0,0,0);
    QHBoxLayout *mpMinFrameLay = new QHBoxLayout(this);
    mpMinFrameLay->setContentsMargins(0,0,0,0);
    mpMinFrameLay->setSpacing(0);

    mpSecFrame->setContentsMargins(0,0,0,0);
    QHBoxLayout *mpSecFrameLay = new QHBoxLayout(this);
    mpSecFrameLay->setContentsMargins(0,0,0,0);
    mpSecFrameLay->setSpacing(0);

    QGraphicsDropShadowEffect *minEffect = new QGraphicsDropShadowEffect (this);
    minEffect->setOffset(QPoint(0,0));
    minEffect->setColor (QColor (0, 0, 0, 255));
    minEffect->setBlurRadius (16);

    QGraphicsDropShadowEffect *secEffect = new QGraphicsDropShadowEffect (this);
    secEffect->setOffset(QPoint(0,0));
    secEffect->setColor (QColor (0, 0, 0, 255));
    secEffect->setBlurRadius (16);

    QGraphicsDropShadowEffect *sepEffect = new QGraphicsDropShadowEffect (this);
    sepEffect->setOffset(QPoint(0,0));
    sepEffect->setColor (QColor (0, 0, 0, 255));
    sepEffect->setBlurRadius (16);

    mpMinutes->setContentsMargins(0,0,0,0);
    mpMinutes->setText(QString::number (miMinutes).rightJustified(2,QChar('0')));
    mpMinutes->setObjectName("timeLabel");
    mpMinutes->setGraphicsEffect (minEffect);

    mpSeconds->setContentsMargins(0,0,0,0);
    mpSeconds->setText(QString::number (miSeconds).rightJustified(2,QChar('0')));
    mpSeconds->setObjectName("timeLabel");
    mpSeconds->setGraphicsEffect (secEffect);

    mpMinFrameLay->addWidget(mpMinutes);
    mpMinFrame->setLayout(mpMinFrameLay);

    mpSecFrameLay->addWidget(mpSeconds);
    mpSecFrame->setLayout(mpSecFrameLay);

    mpSeparator->setContentsMargins(0,0,0,0);
    mpSeparator->setObjectName("timeLabel");
    mpSeparator->setGraphicsEffect (sepEffect);

    mpMinutes->setProperty("ColorInterval", miColorInterval);
    mpSeconds->setProperty("ColorInterval", miColorInterval);
    mpSeparator->setProperty("ColorInterval", miColorInterval);

    mpMinutes->setProperty("FontSize", miFontSizeInterval);
    mpSeconds->setProperty("FontSize", miFontSizeInterval);
    mpSeparator->setProperty("FontSize", miFontSizeInterval);

    lpGlobalHLay->addWidget(mpMinFrame);
    lpGlobalHLay->addWidget(mpSeparator);
    lpGlobalHLay->addWidget(mpSecFrame);

    mpTimer->setInterval(1000);

    connect(mpTimer, SIGNAL(timeout()), this, SLOT(secLess()));

    setLayout(lpGlobalHLay);
}

MainFrame::~MainFrame()
{
    saveSettings();
}

void MainFrame::loadSettings()
{
    QSettings settings (msSettingsPath, QSettings::IniFormat);

    miMinutes = settings.value("minutes", 2).toInt();
    miSeconds = settings.value("seconds", 0).toInt();

    miFontSizeInterval = settings.value("sizefont", 4).toInt();
}

void MainFrame::saveSettings()
{
    msSettingsPath = QApplication::applicationDirPath() + "/config.ini";

    QFile settingsFile (msSettingsPath);
    if(!settingsFile.exists()) settingsFile.open(QFile::ReadWrite);

    QSettings settings (msSettingsPath, QSettings::IniFormat);

    settings.setValue("minutes", miMinutes);
    settings.setValue("seconds", miSeconds);

    settings.setValue("sizefont", miFontSizeInterval);
}


void MainFrame::secLess()
{
    int mins = mpMinutes->text ().toInt();
    int secs = mpSeconds->text ().toInt() + ((mbTimeout)? 1 : - 1);

    if(secs < 0 || secs > 59)
    {
        mins += ((mbTimeout)? 1 : - 1);
        if(mins > 99)
        {
            resetClock(false);
            return;
        }

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

    if(mins == 0 && secs == 4 && !mbTimeout) QSound::play(":/sounds/resources/Metronome.wav");
    else if (mins == 0 && secs == 0) QSound::play(":/sounds/resources/AirHorn-mike_koenig.wav");

    if(miColorInterval < 9)
    {
        int liOldInterval = miColorInterval;
        miColorInterval = (miMinutes * 60 - mins * 60 + miSeconds - secs) / mfColorIntervalJump;

        if(liOldInterval != miColorInterval)
        {
            mpMinutes->setProperty("ColorInterval", miColorInterval);
            mpSeconds->setProperty("ColorInterval", miColorInterval);
            mpSeparator->setProperty("ColorInterval", miColorInterval);

            updateWidgetStyle (mpMinutes);
            updateWidgetStyle (mpSeconds);
            updateWidgetStyle (mpSeparator);
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
        mpSeparator->setProperty("FontSize", miFontSizeInterval);

        updateWidgetStyle (mpMinutes);
        updateWidgetStyle (mpSeconds);
        updateWidgetStyle (mpSeparator);
    }
}

void MainFrame::resetClock(bool lbTimerActive)
{
    mpMinutes->setText(QString::number (miMinutes).rightJustified(2,QChar('0')));
    mpSeconds->setText(QString::number (miSeconds).rightJustified(2,QChar('0')));

    mfColorIntervalJump = (qreal)(miMinutes * 60 + miSeconds) / 10.0;
    miColorInterval = 0;

    mpMinutes->setProperty("ColorInterval", miColorInterval);
    mpSeconds->setProperty("ColorInterval", miColorInterval);
    mpSeparator->setProperty("ColorInterval", miColorInterval);

    updateWidgetStyle (mpMinutes);
    updateWidgetStyle (mpSeconds);
    updateWidgetStyle (mpSeparator);

    mbTimeout = false;

    if (lbTimerActive) mpTimer->start ();
    else mpTimer->stop();

    mbTimerOn = lbTimerActive;
}
