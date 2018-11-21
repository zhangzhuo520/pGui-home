#ifndef SQLMANAGER_H
#define SQLMANAGER_H

#include <QObject>
#include <QCoreApplication>
#include <QtSql/QSqlDatabase>
#include <QDebug>
#include <QStringList>
#include <QtSql/QSqlRecord>
namespace UI {
class SQLManager : public QObject
{
    Q_OBJECT
public:
    explicit SQLManager(QObject *parent = 0);

    void setDatabaseName(QString);
    QString getDatabaseName();

    bool openDB();
    void closeDB();

    QSqlDatabase getDatabase();
signals:
    
public slots:

private:
    QSqlDatabase db;
    
};
}
#endif // SQLMANAGER_H
