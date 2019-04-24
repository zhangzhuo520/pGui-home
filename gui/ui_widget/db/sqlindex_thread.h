#ifndef SQLINDEX_THREAD_H
#define SQLINDEX_THREAD_H
#include <QThread>
#include <QObject>
#include <QSqlQuery>

#include "../qt_logger/pgui_log_global.h"
namespace ui {
class SQLManager;
class SqlIndexWorker : public QObject
{
    Q_OBJECT
public:
    explicit SqlIndexWorker(QObject * parent = 0);
    ~SqlIndexWorker();

    void set_database(SQLManager *);
public slots:
    void slot_create_index_start();

signals:
    void signal_finished();

private:
    SQLManager *m_sqlmanager;
};

class SqlIndexThread :public QObject
{
    Q_OBJECT
public:
    explicit SqlIndexThread(QObject *parent = 0);
    ~SqlIndexThread();

    void set_database_path(const QString&);

    void start_work();

private slots:
    void slot_finish_work();

private:
    QThread *m_thread;
    SqlIndexWorker *m_sqlindex_worker;
    SQLManager *m_sqlmanager;
    QString m_database_path;
};

}
#endif // SQLINDEX_THREAD_H
