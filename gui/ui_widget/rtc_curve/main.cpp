#include "ui_rtscurve.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ui::RtsCurve w;
    w.show();
    
    return a.exec();
}
