#ifndef ICONHELPER_H
#define ICONHELPER_H

#include <QObject>
#include <QMutex>
#include <QPushButton>
#include <QApplication>
#include <QLabel>
#include <QFont>
#include <QFontDatabase>
#include <QApplication>
#include <QFile>
namespace ui {
class IconHelper : public QObject
{
    Q_OBJECT

public:
    explicit IconHelper(QObject *parent = 0);

    static IconHelper * getInstance()
    {
        static QMutex mutex;
        if (!_instance)
        {
            QMutexLocker locker(&mutex);
            if (! _instance)
            {
                _instance = new IconHelper;
            }
        }
        return _instance;
    }

    void setIcon(QLabel *, QChar);
    void setIcon(QPushButton *, QChar);

    void setStyle(const QString &);
signals:
    
public slots:

private:
    static IconHelper * _instance;
    QFont iconFont;
};
}
#endif // ICONHELPER_H
