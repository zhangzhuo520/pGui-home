#include "rts_imageparsing.h"
namespace ui {
ImageWorker::ImageWorker(QObject *parent):
    QObject(parent),
    m_image_width(0),
    m_image_high(0)
{
    m_all_data_list = new QStringList();
}

ImageWorker::~ImageWorker()
{
    delete m_all_data_list;
    m_all_data_list = 0;
}

void ImageWorker::save_to_image()
{
    m_image = new QImage(QSize(m_image_width, m_image_high), QImage::Format_RGB32);
    if ((m_image_width * m_image_high) != m_image_data_list.count())
    {
        qDebug() << "Image data pixel number Error !";
        return;
    }
    for (int i = 0; i < m_image_width; i ++)
    {
        for (int j = 0; j < m_image_high; j ++)
        {
            qreal color_data = m_image_data_list.at(i * m_image_width + j).toDouble();
            int data = ((color_data + 2) / 4) * 255;
            m_image->setPixel(i, j, qRgb(data, data, data));
        }
    }
    QString file_path = QDir::homePath() + "/.picasso_gui/pgui_rts/AiImage.png";
    m_image->save(file_path, "PNG", 100);
}

void ImageWorker::get_text_data()
{
    QString fpath = QDir::homePath() + "/.picasso_gui/pgui_rts/rts/middata/0-rts1_nc_resist_image.txt";
    QMutex Mutex;
    QFile image_file(fpath);
    if (!image_file.exists())
    {
        qDebug() << "file not exists!";
        return;
    }
    if (!image_file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    Mutex.lock();
    while (!image_file.atEnd()) {
        QByteArray line = image_file.readLine();
        m_all_data_list->append(line);
    }
    Mutex.unlock();
}

void ImageWorker::get_image_data()
{
    QMutex Mutex;
    Mutex.lock();
    for (int i = 0; i < m_all_data_list->count(); i ++)
    {
        if (m_all_data_list->at(i).size() > 100)
        {
            m_image_data_list = m_all_data_list->at(i).split(QRegExp("\\s+"),  QString::SkipEmptyParts);
        }
        else
        {
            if (0 == i)
            {
              QStringList m_imagesize_list =  m_all_data_list->at(i).split(QRegExp("\\s+"),  QString::SkipEmptyParts);
              if (m_imagesize_list.count() == 2)
              {
                  m_image_width = m_imagesize_list.at(0).toInt();
                  m_image_high = m_imagesize_list.at(1).toInt();
                  qDebug() << m_image_width << m_image_high;
              }
            }
            else if (1 == i)
            {
              QStringList m_image_pos_list = m_all_data_list->at(i).split(QRegExp("\\s+"),  QString::SkipEmptyParts);
               qDebug() << m_image_pos_list;
            }
        }
    }
    Mutex.unlock();
}

void ImageWorker::slot_start_work()
{
    get_text_data();
    get_image_data();
    save_to_image();
    emit signal_parsing_finished();
}

RtsImageParsing::RtsImageParsing(QObject *parent):
    QObject(parent)
{
    m_image_worker = new ImageWorker();
    m_worker_thread = new QThread();
    m_image_worker->moveToThread(m_worker_thread);

    connect(m_worker_thread, SIGNAL(started()), m_image_worker, SLOT(slot_start_work()));
    connect(m_image_worker, SIGNAL(signal_parsing_finished()), this, SLOT(slot_parsing_finished()));
}

RtsImageParsing::~RtsImageParsing()
{
}

void RtsImageParsing::parsing_file()
{
    m_worker_thread->start();
}

void RtsImageParsing::slot_parsing_finished()
{
    m_worker_thread->quit();
    qDebug() << "image file parsing end";
    emit signal_parsing_finished();
}
}
