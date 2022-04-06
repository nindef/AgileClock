#include "maindialog.h"
#include <QFont>
#include <QPainter>
#include <QImage>
#include <QPushButton>
#include <QPalette>
#include <QBrush>
#include <QScreen>
#include <QGridLayout>
#include <QDebug>
#include <QSettings>
#include <QApplication>
#include <QStackedLayout>
#include <QLabel>
#include <QTimer>

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
{
    setContentsMargins(0,0,0,0);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowOpacity(0.8);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);

    msSettingsPath = QApplication::applicationDirPath() + "/config.ini";

    mHideButtonsTimer = new QTimer(this);
    mHideButtonsTimer->setSingleShot(true);
    mHideButtonsTimer->setInterval(1000);
    connect (mHideButtonsTimer, &QTimer::timeout, [this]() { setButtonsVisible (false); });

    setMouseTracking(true);

    setLayout(buildLayout ());
}

MainDialog::~MainDialog()
{
    //save current settings
    auto geom = geometry();

    QSettings settings(msSettingsPath, QSettings::IniFormat);
    settings.setValue("lastPos", geom.topLeft());
}

void MainDialog::showEvent([[maybe_unused]] QShowEvent *event)
{
    auto center = this->screen()->availableGeometry().center() - rect().center();
    QSettings settings(msSettingsPath, QSettings::IniFormat);
    auto lastPos = settings.value("lastPos", center).toPoint();

    move(lastPos);
}

void MainDialog::enterEvent([[maybe_unused]] QEnterEvent *event)
{
    mHideButtonsTimer->stop ();
    setButtonsVisible(true);
}

void MainDialog::leaveEvent([[maybe_unused]] QEvent *event)
{
    mHideButtonsTimer->start ();
}

bool MainDialog::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::Resize)
    {
        adjustSize();
        return true;
    }
    return QDialog::eventFilter(object, event);
}

QLayout *MainDialog::buildLayout()
{
    const auto layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignCenter);

    mCloseButton = new QLabel (this);
    mCloseButton->setContentsMargins(0,0,0,0);
    mCloseButton->setPixmap (QIcon (":/images/img_close").pixmap(QSize(20,20)));

    auto lpMainFrame = new MainFrame;
    connect (this, &MainDialog::signalResetClock, lpMainFrame, &MainFrame::resetClock);
    connect (this, &MainDialog::fontSizeChanged, lpMainFrame, &MainFrame::onFontSizeChanged, Qt::DirectConnection);
    connect (lpMainFrame, &MainFrame::fontColorChanged, this, &MainDialog::onFontColorChanged);

    lpMainFrame->installEventFilter(this);

    layout->addWidget(lpMainFrame);
    layout->setAlignment(lpMainFrame, Qt::AlignCenter);

    return layout;
}

QWidget *MainDialog::getButtonContainerAligned(QWidget* button, Qt::Alignment alignment)
{
    const auto buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(0,0,0,0);
    buttonLayout->setSpacing(0);
    buttonLayout->setAlignment(alignment);
    buttonLayout->addWidget(button);

    return getButtonContainerAligned (buttonLayout);
}

QWidget *MainDialog::getButtonContainerAligned(QLayout *buttonLayout)
{
    const auto buttonContainer = new QWidget;
    buttonContainer->setContentsMargins(0,0,0,0);
    buttonContainer->setLayout(buttonLayout);

    return buttonContainer;
}

void MainDialog::setButtonsVisible(bool areVisible)
{
    mCloseButton->setVisible(areVisible);
}

void MainDialog::onFontColorChanged(QColor newColor)
{
    const auto backgroundColor = QString (
                "MainFrame {"
                "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                "                            stop:0 rgba(20, 20, 20, 100), "
                "                            stop: 0.4 rgba(20, 20, 20, 40), "
                "                            stop:1 rgba(%1, %2, %3, 150));"
                "border: 0px;"
                "border-radius: 5px;"
                "}")
            .arg (newColor.red())
            .arg (newColor.green())
            .arg (newColor.blue());
    setStyleSheet (backgroundColor);
}

void MainDialog::mousePressEvent(QMouseEvent * event)
{
    if(event->buttons() & Qt::LeftButton)
        oldPosition = event->pos();

    mbTimerOn = true;
}

void MainDialog::mouseMoveEvent(QMouseEvent * event)
{
    if (event->buttons() & Qt::LeftButton)
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
        const auto closePressed = mCloseButton->geometry().contains(event->pos ());

        if (closePressed)
            close ();
        else if (acumDespl.manhattanLength() < 3 && mbTimerOn)
            emit signalResetClock(true);

        mbTimerOn = false;
        acumDespl = QPoint(0,0);
    }
}

void MainDialog::mouseDoubleClickEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        const auto closePressed = mCloseButton->geometry().contains(event->pos ());

        if (!closePressed)
        {
            emit signalResetClock(false);
            mbTimerOn = false;
        }
    }
}


