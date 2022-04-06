#pragma once

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
        void mousePressEvent(QMouseEvent * event) override;
        void mouseMoveEvent(QMouseEvent * event) override;
        void mouseReleaseEvent(QMouseEvent * event) override;
        void mouseDoubleClickEvent(QMouseEvent * event) override;
        void showEvent(QShowEvent *event) override;
        void enterEvent (QEnterEvent *event) override;
        void leaveEvent (QEvent *event) override;

        bool eventFilter(QObject *object, QEvent *event) override;

    private:
        QLabel * mCloseButton = nullptr;
        QTimer * mHideButtonsTimer = nullptr;
        QPoint oldPosition, acumDespl;
        bool mbTimerOn = false;
        QString msSettingsPath;

        QLayout* buildLayout ();
        QWidget* getButtonContainerAligned (QWidget* button, Qt::Alignment alignment);
        QWidget* getButtonContainerAligned (QLayout* buttonLayout);
        void setButtonsVisible (bool areVisible = true);

    signals:
        void signalResetClock(bool);
        void minutesEdited (bool plusOne);
        void fontSizeChanged (bool isLargerSize);
};

