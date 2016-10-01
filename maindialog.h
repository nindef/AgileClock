#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QMainWindow>
#include <QDialog>
#include <QFrame>
#include <QLabel>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPoint>

class QTimer;
class QLabel;
class QFrame;


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

signals:
    void signalResetClock(bool);

private:
    QPoint oldPosition, acumDespl;
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

protected:
    void wheelEvent(QWheelEvent * event);
    void mouseDoubleClickEvent(QMouseEvent * event);

public slots:
    void resetClock(bool lbTimerActive);

private slots:
    void secLess();

private:
    QTimer *mpTimer;
    QLabel *mpMinutes, *mpSeconds, *separator;
    QFrame *mpMinFrame, *mpSecFrame;
    int miMinutes, miSeconds;
    bool mbTimerOn = false, mbTimeout = false;
    int R = 25, G = 170, B = 25;
};

#endif // MAINDIALOG_H
