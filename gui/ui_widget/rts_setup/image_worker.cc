#include "image_worker.h"
#include <QMutex>
#include <QFile>
#include <QThread>
#include <thread>
#include "../../qt_logger/pgui_log_global.h"
namespace ui {

ImageDataThread::ImageDataThread(QObject *parent):
    QObject(parent),
    m_image_path(""),
    m_image_width(0),
    m_image_high(0)
{

}

ImageDataThread::~ImageDataThread()
{
}

void ImageDataThread::set_image_path(const QString &path)
{
    m_image_path = path;
}

QString ImageDataThread::image_path()
{
    return m_image_path;
}

void ImageDataThread::parse_image_data()
{
    QMutex Mutex;
    QString file_path = m_image_path;
    Mutex.lock();
    QFile image_file(file_path);
    if (!image_file.exists())
    {
        logger_console << QString("image data file not exists!(%1)").arg(file_path);
        return;
    }
    if (!image_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        logger_console << "image file open error!";
        return;
    }

    while (!image_file.atEnd()) {
        QByteArray line = image_file.readLine();
        m_all_data_list.append(line);
    }

    for (int i = 0; i < m_all_data_list.count(); i ++)
    {
        if (m_all_data_list.at(i).size() > 100)
        {
            m_image_data_list = m_all_data_list.at(i).split(QRegExp("\\s+"),  QString::SkipEmptyParts);
        }
        else
        {
            if (0 == i)
            {
              QStringList m_imagesize_list = m_all_data_list.at(i).split(QRegExp("\\s+"),  QString::SkipEmptyParts);
              if (m_imagesize_list.count() == 2)
              {
                  m_image_width = m_imagesize_list.at(0).toInt();
                  m_image_high = m_imagesize_list.at(1).toInt();
              }
            }
            else if (1 == i)
            {
                m_image_pos_list = m_all_data_list.at(i).split(QRegExp("\\s+"),  QString::SkipEmptyParts);
            }
        }
    }
    emit signal_finished();
}

const QStringList &ImageDataThread::image_data()
{
    return m_image_data_list;
}

const QStringList &ImageDataThread::image_pos()
{
    return m_image_pos_list;
}

u_int32_t ImageDataThread::image_width()
{
    return m_image_width;
}

u_int32_t ImageDataThread::image_high()
{
    return m_image_high;
}

void ImageDataThread::slot_start()
{
    parse_image_data();
}


ImageDataWorker::ImageDataWorker(QObject *parent):
    QObject(parent),
    m_image_path("")
{
    m_thread = new QThread();
    m_imagedata_thread = new ImageDataThread();
    m_imagedata_thread->moveToThread(m_thread);
    connect(m_thread, SIGNAL(started()), m_imagedata_thread, SLOT(slot_start()));
    connect(m_imagedata_thread, SIGNAL(signal_finished()), this, SLOT(slot_finished()));
}

ImageDataWorker::~ImageDataWorker()
{

}

void ImageDataWorker::start()
{
    m_thread->start();
    logger_widget(QString("image: %1 start parse!").arg(m_image_path));
}

void ImageDataWorker::set_image_path(const QString & path)
{
    m_image_path = path;
    m_imagedata_thread->set_image_path(path);
}

const QStringList &ImageDataWorker::image_data()
{
    return m_imagedata_thread->image_data();
}

const QStringList &ImageDataWorker::image_post()
{
    return m_imagedata_thread->image_pos();
}

u_int32_t ImageDataWorker::image_width()
{
    return m_imagedata_thread->image_width();
}

u_int32_t ImageDataWorker::image_high()
{
    return m_imagedata_thread->image_high();
}

void ImageDataWorker::slot_finished()
{
    m_thread->quit();
    logger_widget(QString("image: %1 parse finished!").arg(m_image_path));
}

}
