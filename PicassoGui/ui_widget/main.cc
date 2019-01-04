#include <QApplication>
#include <QFont>
#include "ui_frame.h"
#include "deftools/defcontrols.h"
#include "deftools/datastruct.h"
#include "ui_application.h"

// Only rigister Class,can use sigal and slot

int main(int argc, char *argv[])
{

//    qInstallMsgHandler(ui::outputMessage);
    ui::GuiApplication a(argc, argv);
#if 1
    qApp->setStyle(new ui::ProxyStyle);
    QFont font;
    font.setFamily("Sans Serif");
    font.setPointSize (9);
    a.setFont(font);

    ui::MainWindow w;
#else
    ui::FlexWidget w;
#endif
    w.show();
    return a.exec();
}
