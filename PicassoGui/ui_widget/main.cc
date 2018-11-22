#include <QApplication>
#include "frame.h"
#include <QFont>
#include "deftools/defcontrols.h"
#include "deftools/datastruct.h"


// Only rigister Class,can use sigal and slot

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qApp->setStyle(new UI::ProxyStyle);
    QFont font;
    font.setFamily("Sans Serif");
    font.setPointSize (9);
    a.setFont(font);

#ifdef SYS_TIPBOX
    UI::MainWindow w;

#else
    UI::Frame w;

#endif
    w.show();
    return a.exec();
}
