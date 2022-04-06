#pragma once

#include <QFrame>

class QLabel;
class QTimer;
class QSoundEffect;

class MainFrame : public QFrame
{
    Q_OBJECT

    public:
        MainFrame(QWidget* parent = 0);
        ~MainFrame();

    public slots:
        void resetClock(bool lbTimerActive);
        void onMinutesEdited (bool plusOne);
        void onFontSizeChanged (bool isLargerSize);

    protected:
        void wheelEvent(QWheelEvent * event) override;
        bool eventFilter(QObject *obj, QEvent *event) override;

    private:
        static const int Alert = 10;
        static const int Warning = 25;
        static const int Timeout = 0;
        QTimer *mpTimer = nullptr;
        QLabel *mpMinutes = nullptr;
        QLabel *mpSeconds = nullptr;
        QLabel *mpSeparator = nullptr;
        QFrame *mpMinFrame = nullptr;
        QFrame *mpSecFrame = nullptr;
        int miMinutes = 0;
        int miSeconds = 0;
        int miFontSize = 0;
        bool mbTimerOn = false;
        bool mbTimeout = false;
        int miColorInterval = 0;
        int miFontSizeInterval = 4;
        qreal mfColorIntervalJump = 0;
        QString msSettingsPath;
        QSoundEffect *mpLessThan3Secs = Q_NULLPTR;
        QSoundEffect *mp0Secs = Q_NULLPTR;

        void loadSettings();
        void saveSettings();

        void addDropShadowEffect(QWidget * widget);

        void updateFontColor();

    private slots:
        void secLess();
};

