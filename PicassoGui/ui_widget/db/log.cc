#include "log.h"
namespace UI{
Log::Log()
{
}

void Log::outputMessage(QString Text)
{
        QStringList filelist;
        QString s_time = QDateTime::currentDateTime().toString("hh:mm:ss:zz");
        QString s_filename = QDate::currentDate().toString("yyyy.MM.dd") + ".log";
        QString curpath = qApp->applicationFilePath() + "/log";
        QDir *d  = new QDir(curpath);
        d->mkdir(curpath);
        QString path =  curpath +  s_filename;
        QFile file(path);
        if (!file.open( QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
            return;
        d->cd(curpath);
        QList <QFileInfo> *fileinfo = new QList<QFileInfo>(d->entryInfoList(filelist));
        if (d->count() > 9 )
        {
            QString deletefilename = d->path() + '/' + QString(fileinfo->at(2).fileName());
            d->remove(deletefilename);
        }

        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << s_time << "     " << Text << "\n";
        out.flush();
        file.close();
    }
}
