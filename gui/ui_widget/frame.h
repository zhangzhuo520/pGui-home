#ifndef FRAME_H
#define FRAME_H

#include <QDialog>
#include <QLabel>
#include <QPoint>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>
#include <QBitmap>
#include "mainwindow.h"
#include "deftools/iconhelper.h"
#include "deftools/defcontrols.h"
#include "deftools/global.h"
#include "deftools/framelesshelper.h"

class Frame : public QDialog
{
    Q_OBJECT
public:
    explicit Frame(QDialog *parent = 0);
    void initTitleBar();
    void setWindowsSizeChange();

signals:
    
public slots:
    void slot_changeWindowSize();

private:
    QWidget *TitleBar;
    QLabel *signLable;
    QLabel *spaceLabel;
    PushButton *MaxButton;
    PushButton *MinButton;
    PushButton *CloseButton;
    QPoint startPos;
    QPoint dragPosition;
    MainWindow *mianwindow;
    FramelessHelper *pHelper;    
};

#endif // FRAME_H
