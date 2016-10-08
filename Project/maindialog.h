#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QMainWindow>
#include <QDialog>
#include <QFrame>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPoint>
#include <QLabel>
#include <QTimer>
#include <QApplication>

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    MainDialog(QWidget *parent = 0);
    ~MainDialog();

protected:
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseDoubleClickEvent(QMouseEvent * event);
    void showEvent(QShowEvent *event);

signals:
    void signalResetClock(bool);

private:
    QPoint oldPosition, acumDespl;
    bool mbTimerOn = false;
    QString msSettingsPath;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0):QMainWindow(parent)
    {
        MainDialog* mainDlg = new MainDialog(this);
        mainDlg->setAttribute(Qt::WA_DeleteOnClose);
        mainDlg->show();
    }
    ~MainWindow() {}
};

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
    bool mbIntervalChanged = true;
    QTimer *mpTimer = new QTimer(this);
    QLabel *mpSign = new QLabel("+", this);
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
    int miColorInterval = 100;
    int miFontSizeInterval = 4;
    qreal mfColorIntervalJump = 0;
    QString msSettingsPath = QApplication::applicationDirPath() + "/config.ini";
};

#endif // MAINDIALOG_H
