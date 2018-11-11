#include <QApplication>
#include "frame.h"
#include <QFont>
#include "deftools/defcontrols.h"
#include "deftools/datastruct.h"

// Only rigister Class,can use sigal and slot

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#if 1
    qApp->setStyle(new UI::ProxyStyle);
    QFont font;
    font.setFamily("Sans Serif");
    font.setPointSize (10);
    a.setFont(font);

    UI::Frame w;
    w.resize(1200, 800);
#else
    UI::FlexWidget w;
#endif
    w.show();
    return a.exec();
}
