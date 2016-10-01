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
    void wheelEvent(QWheelEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseDoubleClickEvent(QMouseEvent * event);

private slots:
    void secLess();
    void resetClock(bool lbTimerActive);

private:
    QTimer *mpTimer;
    QLabel *mpMinutes, *mpSeconds, *separator;
    QFrame *mpMinFrame, *mpSecFrame;
    QPoint oldPosition, acumDespl;
    int miMinutes, miSeconds;
    bool mbTimerOn = false, mbTimeout = false;
    int R = 25, G = 170, B = 25;
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

#endif // MAINDIALOG_H
