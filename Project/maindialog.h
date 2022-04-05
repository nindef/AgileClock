#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QMainWindow>
#include <QDialog>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPoint>

#include "mainframe.h"

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

    private:
        QPoint oldPosition, acumDespl;
        bool mbTimerOn = false;
        QString msSettingsPath;

    signals:
        void signalResetClock(bool);
};

#endif // MAINDIALOG_H
