#include "sqlindex_thread.h"
#include "sqlmanager.h"

namespace ui {

SqlIndexWorker::SqlIndexWorker(QObject *parent):
    QObject(parent),
    m_sqlmanager(NULL)
{

}

SqlIndexWorker::~SqlIndexWorker()
{
}

void SqlIndexWorker::set_database(SQLManager* sqlmanager)
{
    m_sqlmanager = sqlmanager;
}

void SqlIndexWorker::slot_create_index_start()
{
    if (m_sqlmanager == NULL)
    {
        logger_widget("database pointer is null!");
        return;
    }

    if (m_sqlmanager->openDB())
    {
//        QSqlQuery query;
//        query.exec("create index check_id_index on defect(check_id);");
//        query.exec("create index defect_category_id_index on defect(category_id);");
//        query.exec("create index category_id_index on category(category_id);");
    }
    else
    {
        return;
    }
    emit signal_finished();
}



SqlIndexThread::SqlIndexThread(QObject *parent):
    QObject(parent),
    m_thread(NULL),
    m_sqlindex_worker(NULL),
    m_sqlmanager(NULL),
    m_database_path("")
{

}

SqlIndexThread::~SqlIndexThread()
{
    if (m_thread != NULL)
    {
        delete m_thread;
        m_thread = 0;
    }

    if (m_sqlindex_worker != NULL)
    {
        delete m_sqlindex_worker;
        m_sqlindex_worker = 0;
    }

    if (m_sqlmanager != NULL)
    {
        delete m_sqlmanager;
        m_sqlindex_worker = 0;
    }
}

void SqlIndexThread::set_database_path(const QString &db_path)
{
    m_database_path = db_path;
}

void SqlIndexThread::start_work()
{
    m_thread = new QThread();
    m_sqlindex_worker = new SqlIndexWorker();
    m_sqlindex_worker->moveToThread(m_thread);

    connect(m_thread, SIGNAL(started()), m_sqlindex_worker, SLOT(slot_create_index_start()));
    connect(m_sqlindex_worker, SIGNAL(signal_finished()), this, SLOT(slot_finish_work()));

    m_sqlmanager = new SQLManager();

    if (m_database_path == "")
    {
        logger_console << "database path is null, please set database name!";
        return;
    }

    m_sqlmanager->setDatabaseName(m_database_path + "/defectDB.sqlite");
    m_sqlindex_worker->set_database(m_sqlmanager);
//    m_thread->start();
//    logger_widget("Create sql index, Please wait for a moment!");
}

void SqlIndexThread::slot_finish_work()
{
    m_sqlmanager->closeDB();
    m_thread->quit();
    delete m_thread;
    m_thread = 0;
    delete m_sqlindex_worker;
    m_sqlindex_worker = 0;
    delete m_sqlmanager;
    m_sqlmanager = 0;
//    logger_widget("index creare finished!");
}


}
