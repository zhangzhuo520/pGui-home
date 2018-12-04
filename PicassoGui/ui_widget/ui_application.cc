#include "ui_application.h"
#include <QMessageBox>

namespace ui
{

GuiApplication::GuiApplication(int& argc, char** argv):QApplication(argc, argv)
{

}

bool GuiApplication::notify(QObject *receiver, QEvent *e)
{
    try
    {
        return QApplication::notify(receiver, e);
    }
    catch(...)
    {
        QMessageBox box(QMessageBox::Critical, "Crtical Warning", "Your format of specified file is invalid.", QMessageBox::Yes);
        if(box.exec() == QMessageBox::Yes)
        {
            return true;
        }
        return false;
    }

}

}
