#include "ui_application.h"
#include <QMessageBox>
#include <QDir>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
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
    return QApplication::notify(receiver, e);

}


void outputMessage(QtMsgType type, const char* msg)
{
    static QMutex mutex;
    mutex.lock();

    QString txt;
    switch(type)
    {
        case QtDebugMsg:
            txt = QString("Debug:");
            break;

        case QtWarningMsg:
            txt = QString("Warning:");
            break;

        case QtCriticalMsg:
            txt = QString("Critical:");
            break;

        case QtFatalMsg:
            txt = QString("Fatal:");
    }

    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3").arg(txt).arg(current_date).arg(msg);

    QString debug_path = QDir::homePath() + "/.PguiDebug";
    QDir dir(debug_path);
    if (!dir.exists())
    {
         if(!dir.mkdir(debug_path))
        {
            return;
        }
    }

    QString filePath = debug_path + "/log.txt";
    QFile log(filePath);
    if(!log.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        return ;
    }

    QTextStream ts(&log);
    ts << endl << message << endl;
    log.flush();
    log.close();

    mutex.unlock();
}
}
