#ifndef IMAGE_WORKER_H
#define IMAGE_WORKER_H

#include <QStringList>
#include <QObject>

class QThread;
namespace ui {

class ImageDataThread : public QObject
{
    Q_OBJECT
public:
    explicit ImageDataThread(QObject *parent = 0);
    ~ImageDataThread();

    void set_image_path(const QString& path);
    QString image_path();

    void parse_image_data();

    const QStringList &image_data();
    const QStringList &image_pos();
    u_int32_t image_width();
    u_int32_t image_high();

public slots:
    void slot_start();

signals:
    void signal_finished();

private:
    QString m_image_path;
    QStringList m_all_data_list;
    QStringList m_image_data_list;
    u_int32_t m_image_width;
    u_int32_t m_image_high;
    QStringList m_image_pos_list;
};

class ImageDataWorker : public QObject
{
    Q_OBJECT
public:
    explicit ImageDataWorker(QObject *parent = 0);
    ~ImageDataWorker();

    void start();
    void set_image_path(const QString&);

    const QStringList &image_data();
    const QStringList &image_post();
    u_int32_t image_width();
    u_int32_t image_high();

public slots:
    void slot_finished();

private:
    QThread *m_thread;
    ImageDataThread *m_imagedata_thread;
    QString m_image_path;
};
}
#endif // IMAGE_WORKER_H
