#include <QApplication>
#include "frame.h"
#include <QFont>
#include "deftools/defcontrols.h"
#include "deftools/datastruct.h"

// Only rigister Class,can use sigal and slot

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qApp->setStyle(new ProxyStyle);
    QFont font;
    font.setFamily("Sans Serif");
    font.setPointSize (10);
    a.setFont(font);
    Frame w;
    w.resize(1200, 800);
    w.show();    
    return a.exec();
}
