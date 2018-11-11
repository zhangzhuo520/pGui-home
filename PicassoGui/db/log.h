#ifndef LOG_H
#define LOG_H
#include <QDateTime>
#include <QMutex>
#include <QTextStream>
#include <QApplication>
#include <QDir>

namespace UI{
class Log
{
public:
    Log();

    void outputMessage(QString);

};
}
#endif // LOG_H
