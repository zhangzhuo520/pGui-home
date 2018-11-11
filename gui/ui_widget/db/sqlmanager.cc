#include "sqlmanager.h"

SQLManager::SQLManager(QObject *parent) :
    QObject(parent)
{
    if(QSqlDatabase::contains("qt_sql_default_connection"))
        db = QSqlDatabase::database("qt_sql_default_connection");
    else
        db = QSqlDatabase::addDatabase("QSQLITE");
}

void SQLManager::setDatabaseName(QString path)
{
    if(!path.isEmpty())
    {
        db.setDatabaseName(path);
    }
    else
    {

        qDebug() << " SQLPath is Empty!";
    }
}

bool SQLManager::openDB()
{
    if (!db.open())
    {
        qDebug() << "open db failed !";
        return false;
    }
    return true;
}

void SQLManager::closeDB()
{
    db.removeDatabase("QSQLITE");
    db.close();
}

QSqlDatabase SQLManager::getDatabase()
{
    return db;
}

QString SQLManager::getDatabaseName()
{
    return db.databaseName();
}
