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

void ImageWorker::set_file_path(const QString & filename)
{
    m_file_name = filename;
}

void ImageWorker::save_to_image()
{
    m_image = new QImage(QSize(m_image_width, m_image_high), QImage::Format_RGB32);
    if ((m_image_width * m_image_high) != m_image_data_list.count())
    {
        qDebug() << QString("Image_width: %1, Image_high: %2, Allpixel: %3, Image data pixel number Error !").arg(m_image_width).arg(m_image_high).arg(m_image_data_list.count());
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
    QString file_path = QDir::homePath() + "/.pangen_gui/pgui_rts/images/" + m_file_name.split(".").at(0) + + ".png";
    m_image->save(file_path, "PNG", 100);

    QSettings setting(QDir::homePath() + "/.pangen_gui/pgui_rts/images/" + m_file_name.split(".").at(0) + ".data", QSettings::IniFormat);
    setting.setValue("image_path", file_path);
}

void ImageWorker::get_text_data()
{
    QMutex Mutex;
    QString m_file_path = QDir::homePath() + "/.pangen_gui/pgui_rts/rts/middata/" + m_file_name;
    QFile image_file(m_file_path);
    if (!image_file.exists())
    {
        logger_console << QString("image data file not exists!(%1)").arg(m_file_path);
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
    QSettings setting(QDir::homePath() + "/.pangen_gui/pgui_rts/images/" + m_file_name.split(".").at(0) + ".data", QSettings::IniFormat);
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
                  setting.setValue("image width", m_image_width);
                  setting.setValue("image height", m_image_high);
              }
            }
            else if (1 == i)
            {
              QStringList m_image_pos_list = m_all_data_list->at(i).split(QRegExp("\\s+"),  QString::SkipEmptyParts);
              setting.setValue("image pos", m_image_pos_list);
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
    QObject(parent),
    m_thread_num(0)
{
}

RtsImageParsing::~RtsImageParsing()
{
}

void RtsImageParsing::parsing_file()
{
     QString path = QDir::homePath() + "/.pangen_gui/pgui_rts/rts/middata";
     QDir d(path);
     QFileInfoList file_list = d.entryInfoList();
     for (int i = 0; i < file_list.count(); i ++)
     {
         if (file_list.at(i).isFile())
         {
              m_filename_list.append(file_list.at(i).fileName());
         }
     }
     m_thread_num = 0;
     ceate_threads();
}

void RtsImageParsing::slot_parsing_finished()
{
    QMutex Mutex;
    Mutex.lock();
    int id = sender()->objectName().toInt();
    m_worker_thread_vector.at(id)->quit();
    m_thread_num ++;
    Mutex.unlock();

    if (m_thread_num == m_worker_thread_vector.count())
    {
        emit signal_parsing_finished();
    }
}

void RtsImageParsing::ceate_threads()
{
    for (int i = 0; i < m_filename_list.count(); i ++)
    {
        m_image_worker = new ImageWorker();
        m_worker_thread = new QThread();
        m_image_worker->setObjectName(QString::number(i));
        m_image_worker->set_file_path(m_filename_list.at(i));
        m_image_worker->moveToThread(m_worker_thread);
        m_image_worker_vector.append(m_image_worker);
        m_worker_thread_vector.append(m_worker_thread);

        connect(m_worker_thread, SIGNAL(started()), m_image_worker, SLOT(slot_start_work()));
        connect(m_image_worker, SIGNAL(signal_parsing_finished()), this, SLOT(slot_parsing_finished()));
        m_worker_thread->start();
    }
}

}
