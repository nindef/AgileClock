#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QFrame>
#include <QTimer>
#include <QLabel>
#include <QWheelEvent>
#include <QApplication>
#include <QSettings>

class QSound;

class MainFrame : public QFrame
{
    Q_OBJECT

public:
    MainFrame(QWidget* parent = 0);
    ~MainFrame();

protected:
    void wheelEvent(QWheelEvent * event);

private:
    void loadSettings();
    void saveSettings();

public slots:
    void resetClock(bool lbTimerActive);

private slots:
    void secLess();

private:
    static const int Alert = 10;
    static const int Warning = 25;
    static const int Timeout = 0;
    QTimer *mpTimer = new QTimer(this);
    QLabel *mpMinutes = new QLabel(this);
    QLabel *mpSeconds = new QLabel(this);
    QLabel *mpSeparator = new QLabel(":",this);
    QFrame *mpMinFrame = new QFrame(this);
    QFrame *mpSecFrame = new QFrame(this);
    int miMinutes = 0;
    int miSeconds = 0;
    int miFontSize = 0;
    bool mbTimerOn = false;
    bool mbTimeout = false;
    int miColorInterval = 0;
    int miFontSizeInterval = 4;
    qreal mfColorIntervalJump = 0;
    QString msSettingsPath = QApplication::applicationDirPath() + "/config.ini";
    QSound *mpLessThan3Secs = Q_NULLPTR, *mp0Secs = Q_NULLPTR;
};

#endif // MAINFRAME_H
