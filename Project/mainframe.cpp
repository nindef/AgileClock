#include "mainframe.h"
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QSoundEffect>
#include <QStyle>
#include <QFile>
#include <QTimer>
#include <QLabel>
#include <QWheelEvent>
#include <QApplication>
#include <QSettings>

inline void updateWidgetStyle (QWidget* lWidget)
{
   lWidget->style ()->unpolish (lWidget);
   lWidget->style ()->polish (lWidget);
   lWidget->update ();
}

MainFrame::MainFrame(QWidget* parent):QFrame(parent)
{
    msSettingsPath = QApplication::applicationDirPath() + "/config.ini";
    setContentsMargins(0,0,0,0);

    loadSettings();

    mpMinutes = new QLabel(this);
    mpMinutes->setContentsMargins(0,0,0,0);
    mpMinutes->setText(QString::number (miMinutes).rightJustified(2,QChar('0')));
    mpMinutes->setObjectName("timeLabel");
    mpMinutes->setProperty("ColorInterval", miColorInterval);
    mpMinutes->setProperty("FontSize", miFontSizeInterval);

    mpSeconds = new QLabel(this);
    mpSeconds->setContentsMargins(0,0,0,0);
    mpSeconds->setText(QString::number (miSeconds).rightJustified(2,QChar('0')));
    mpSeconds->setObjectName("timeLabel");
    mpSeconds->setProperty("ColorInterval", miColorInterval);
    mpSeconds->setProperty("FontSize", miFontSizeInterval);

    auto mpMinFrameLay = new QHBoxLayout(this);
    mpMinFrameLay->setContentsMargins(0,0,0,0);
    mpMinFrameLay->setSpacing(0);
    mpMinFrameLay->addWidget(mpMinutes);

    mpMinFrame = new QFrame(this);
    mpMinFrame->setContentsMargins(0,0,0,0);
    mpMinFrame->setLayout(mpMinFrameLay);

    auto mpSecFrameLay = new QHBoxLayout();
    mpSecFrameLay->setContentsMargins(0,0,0,0);
    mpSecFrameLay->setSpacing(0);
    mpSecFrameLay->addWidget(mpSeconds);

    mpSecFrame = new QFrame(this);
    mpSecFrame->setContentsMargins(0,0,0,0);
    mpSecFrame->setLayout(mpSecFrameLay);

    mpSeparator = new QLabel(":",this);
    mpSeparator->setContentsMargins(0,0,0,0);
    mpSeparator->setObjectName("timeLabel");
    mpSeparator->setProperty("ColorInterval", miColorInterval);
    mpSeparator->setProperty("FontSize", miFontSizeInterval);

    addDropShadowEffect (mpMinutes);
    addDropShadowEffect (mpSeconds);
    addDropShadowEffect (mpSeparator);

    auto lpGlobalHLay = new QHBoxLayout;
    lpGlobalHLay->setContentsMargins(0,0,0,0);
    lpGlobalHLay->setSpacing(0);
    lpGlobalHLay->addWidget(mpMinFrame);
    lpGlobalHLay->addWidget(mpSeparator);
    lpGlobalHLay->addWidget(mpSecFrame);

    mpTimer = new QTimer(this);
    mpTimer->setInterval(1000);
    mpTimer->setTimerType(Qt::TimerType::PreciseTimer);

    connect(mpTimer, SIGNAL(timeout()), this, SLOT(secLess()));

    setLayout(lpGlobalHLay);

    mpLessThan3Secs = new QSoundEffect;
    mpLessThan3Secs->setSource(QUrl::fromLocalFile(":/sounds/Metronome"));
    mp0Secs = new QSoundEffect;
    mp0Secs->setSource(QUrl::fromLocalFile(":/sounds/Bell"));
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

    if (!settingsFile.exists())
        settingsFile.open(QFile::ReadWrite);

    QSettings settings (msSettingsPath, QSettings::IniFormat);
    settings.setValue("minutes", miMinutes);
    settings.setValue("seconds", miSeconds);
    settings.setValue("sizefont", miFontSizeInterval);
}

void MainFrame::addDropShadowEffect(QWidget * widget)
{
    if (widget != nullptr)
    {
        const auto effect = new QGraphicsDropShadowEffect (this);
        effect->setOffset(QPoint(0,0));
        effect->setColor (QColor (0, 0, 0, 255));
        effect->setBlurRadius (16);
        widget->setGraphicsEffect (effect);
    }
}

void MainFrame::updateFontColor()
{
    mpMinutes->setProperty("ColorInterval", miColorInterval);
    mpSeconds->setProperty("ColorInterval", miColorInterval);
    mpSeparator->setProperty("ColorInterval", miColorInterval);

    updateWidgetStyle (mpMinutes);
    updateWidgetStyle (mpSeconds);
    updateWidgetStyle (mpSeparator);

    if (miColorInterval > 6)
        fontColorChanged (mpMinutes->palette().brush(QPalette::ColorRole::Text).color());
    else if (miColorInterval == 0)
        fontColorChanged(QColor(Qt::white));
}


void MainFrame::secLess()
{
    auto mins = mpMinutes->text ().toInt();
    auto secs = mpSeconds->text ().toInt() + (mbTimeout ? 1 : - 1);

    if (secs < 0 || secs > 59)
    {
        mins += ((mbTimeout)? 1 : - 1);

        if (mins > 99)
        {
            resetClock (false);
            return;
        }

        if (mins < 0)
        {
            mins = 0;
            mbTimeout = true;
            secs = 1;
        }
        else
            secs = mbTimeout && secs > 59 ? 0 : 59;
    }

    mpMinutes->setText(QString::number (mins).rightJustified(2,QChar('0')));
    mpSeconds->setText(QString::number (secs).rightJustified(2,QChar('0')));

    if (mins == 0 && secs == 3 && !mbTimeout)
        mpLessThan3Secs->play();
    else if (mins == 0 && secs == 0)
        mp0Secs->play();

    if (miColorInterval < 9)
    {
        auto liOldInterval = miColorInterval;
        miColorInterval = (miMinutes * 60 - mins * 60 + miSeconds - secs) / mfColorIntervalJump;

        if (liOldInterval != miColorInterval)
            updateFontColor ();
    }
}

void MainFrame::wheelEvent(QWheelEvent * event)
{
    if (event->modifiers() != Qt::CTRL)
    {
        if (mpTimer->isActive())
            resetClock(false);

        if (mpMinFrame->geometry().contains(event->position().toPoint()))
        {
            auto mins = mpMinutes->text ().toInt();
            miMinutes = event->angleDelta().y() < 0 ? static_cast<int>(qMax(--mins,0)) : static_cast<int>(qMin(++mins,99));
            mpMinutes->setText(QString::number (miMinutes).rightJustified(2,QChar('0')));
        }
        else if (mpSecFrame->geometry().contains(event->position().toPoint()))
        {
            auto secs = mpSeconds->text ().toInt();

            if (event->angleDelta().y() < 0)
            {
                secs -= 1;

                if (secs < 0)
                    secs = 59;
            }
            else
            {
                secs += 1;

                if(secs > 59)
                    secs = 0;
            }
            miSeconds = secs;
            mpSeconds->setText(QString::number (secs).rightJustified(2,QChar('0')));
        }
    }
    else
    {
        if (event->angleDelta().y() < 0)
        {
            //smaller font
            miFontSizeInterval--;

            if (miFontSizeInterval < 0)
                miFontSizeInterval = 0;
        }
        else
        {
            //larger font
            miFontSizeInterval++;

            if (miFontSizeInterval > 8)
                miFontSizeInterval = 8;
        }

        mpMinutes->setProperty("FontSize", miFontSizeInterval);
        mpSeconds->setProperty("FontSize", miFontSizeInterval);
        mpSeparator->setProperty("FontSize", miFontSizeInterval);

        updateWidgetStyle (mpMinutes);
        updateWidgetStyle (mpSeconds);
        updateWidgetStyle (mpSeparator);
    }
}

bool MainFrame::eventFilter(QObject *obj, QEvent *event)
{
    qDebug () << event->type();
    return QFrame::eventFilter(obj, event);
}

void MainFrame::resetClock(bool lbTimerActive)
{
    mpLessThan3Secs->stop();
    mp0Secs->stop();

    mpMinutes->setText(QString::number (miMinutes).rightJustified(2,QChar('0')));
    mpSeconds->setText(QString::number (miSeconds).rightJustified(2,QChar('0')));

    mfColorIntervalJump = static_cast<qreal>((miMinutes * 60 + miSeconds) / 10.0);
    miColorInterval = 0;

    updateFontColor ();

    mbTimeout = false;

    if (lbTimerActive)
        mpTimer->start ();
    else
        mpTimer->stop();

    mbTimerOn = lbTimerActive;
}

void MainFrame::onMinutesEdited(bool plusOne)
{
    if (mpTimer->isActive())
        resetClock(false);

    auto mins = mpMinutes->text ().toInt();
    miMinutes = plusOne ? static_cast<int>(qMin(++mins,99)) : static_cast<int>(qMax(--mins,0));
    mpMinutes->setText(QString::number (miMinutes).rightJustified(2,QChar('0')));
}

void MainFrame::onFontSizeChanged(bool isLargerSize)
{
    if (isLargerSize)
        miFontSizeInterval = std::min(miFontSizeInterval+1, 8);
    else
        miFontSizeInterval = std::max(miFontSizeInterval-1, 0);

    mpMinutes->setProperty("FontSize", miFontSizeInterval);
    mpSeconds->setProperty("FontSize", miFontSizeInterval);
    mpSeparator->setProperty("FontSize", miFontSizeInterval);

    updateWidgetStyle (mpMinutes);
    updateWidgetStyle (mpSeconds);
    updateWidgetStyle (mpSeparator);

    adjustSize();
}
