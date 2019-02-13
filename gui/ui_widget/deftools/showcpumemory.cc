#include "showcpumemory.h"
#include <QTimer>
#include <QProcess>
#include <QLabel>

#ifdef Q_OS_WIN
#include "windows.h"
#endif
#define MB (1024 * 1024)
#define KB (1024)

namespace ui {

ShowCPUMemory::ShowCPUMemory(QObject *parent) : QObject(parent)
{
    totalNew = idleNew = totalOld = idleOld = 0;
    cpuPercent = 0;
    m_cpu_total = m_cpu_use = 0;
    memoryPercent = 0;
    memoryAll = 0;
    memoryUse = 0;
}

QString ShowCPUMemory::GetCPU()
{
#if 0
    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly))
        return QString("info error!");
    QString stat_file =  file.readAll();

    QStringList list = stat_file.split("\n");
    for (int i = 0; i < list.count(); i ++)
    {
        if (list.at(i).startsWith("cpu"))
        {
            QStringList line_list = list.at(i).split(" ");
            foreach (QString value, line_list) {
                totalNew += value.toInt();
            }
        }
    }
    totalOld = totalNew;

    QString msg = QString("CPU : %1")
            .arg(cpuPercent);
    return msg;
#else
    QProcess process;
    process.start("cat /proc/stat");
    process.waitForFinished();
    QString str = process.readLine();
    str.replace("\n","");
    str.replace(QRegExp("( ){1,}")," ");
    QStringList lst = str.split(" ");
    if(lst.size() > 3)
    {
        double use = lst.at(1).toDouble() + lst.at(2).toDouble() + lst.at(3).toDouble();
        double total = 0;
        for(int i = 1;i < lst.size();++i)
            total += lst.at(i).toDouble();
        if(total - m_cpu_total > 0)
        {
            QString msg = QString("Cpu rate: : %1 %")
                    .arg((use - m_cpu_use) / (total - m_cpu_total) * 100.0);
            m_cpu_total = total;
            m_cpu_use = use;
            return msg;
        }
    }
    return QString("");
#endif
}

QString ShowCPUMemory::GetMemory()
{
#if 0
    QFile file("/proc/meminfo");
    if (!file.open(QIODevice::ReadOnly))
        return QString("info error!");
    QString filedata = file.readAll();
    QStringList list = filedata.split("\n");
    for (int i = 0; i < list.count(); i ++)
    {
        QString s =  list.at(i);
        if (s.startsWith("MemTotal")) {
            s = s.replace(" ", "");
            s = s.split(":").at(1);
            memoryAll = s.left(s.length() - 3).toInt() / KB;
        } else if (s.startsWith("MemFree")) {
            s = s.replace(" ", "");
            s = s.split(":").at(1);
            memoryFree = s.left(s.length() - 3).toInt() / KB;
        } else if (s.startsWith("Buffers")) {
            s = s.replace(" ", "");
            s = s.split(":").at(1);
            memoryFree += s.left(s.length() - 3).toInt() / KB;
        } else if (s.startsWith("Cached")) {
            s = s.replace(" ", "");
            s = s.split(":").at(1);
            memoryFree += s.left(s.length() - 3).toInt() / KB;
            memoryUse = memoryAll - memoryFree;
            memoryPercent = 100 * memoryUse / memoryAll;
            break;
        }
    }
    QString msg = QString("Memory : %1% (  %2 MB /  %3 MB )")
            .arg(memoryPercent).arg(lst.at(1).arg(lst.at(6));
    return msg;
#else
    QProcess process;
    process.start("free -m");             //使用free完成获取
    process.waitForFinished();
    process.readLine();
    QString str = process.readLine();
    str.replace("\n","");
    str.replace(QRegExp("( ){1,}")," ");//将连续空格替换为单个空格 用于分割
    QStringList lst = str.split(" ");
    if(lst.size() > 6)
    {
        double mem_rate = lst.at(1).toDouble() / lst.at(6).toDouble();
        QString msg = QString("Memory rate: %1% (  %2 MB /  %3 MB )")
                .arg(mem_rate).arg(lst.at(1)).arg(lst.at(6));
        return msg;
    }
#endif
    return QString("");
}

}
