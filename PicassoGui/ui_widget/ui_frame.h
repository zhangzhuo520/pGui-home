#ifndef FRAME_H
#define FRAME_H

#include <QDialog>
#include <QLabel>
#include <QPoint>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>
#include <QBitmap>
#include "ui_mainwindow.h"
#include "deftools/iconhelper.h"
#include "deftools/defcontrols.h"
#include "deftools/global.h"
#include "deftools/framelesshelper.h"

namespace ui{

class Frame : public QFrame
{
    Q_OBJECT
//    Q_PROPERTY(sizeState widowSizeState READ widowSizeState WRITE setwidowSizeState)
public:

//    Q_DECLARE_METATYPE(UI::Frame::sizeState)
    explicit Frame(QFrame *parent = 0);
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
}
#endif // FRAME_H
