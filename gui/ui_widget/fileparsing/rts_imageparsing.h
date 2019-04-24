#ifndef RTS_IMAGEPARSING_H
#define RTS_IMAGEPARSING_H
#include <QFile>
#include <QDir>
#include <QThread>
#include <QDebug>
#include <QStringList>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QImage>
#include <QObject>
#include <iostream>
#include <QColor>
#include <QMutex>
#include <QSettings>

#include "../qt_logger/pgui_log_global.h"

namespace ui {
class ImageWorker : public QObject
{
    Q_OBJECT
 public:
    explicit ImageWorker(const QString& path = "");
    ~ImageWorker();
    void set_file_name(const QString&);

public slots:
    void slot_start_work();

signals:
    void signal_parsing_finished();

private:
    void get_text_data();
    void get_image_data();
    void save_to_image();

    QStringList *m_all_data_list;
    QStringList m_image_data_list;

    QImage *m_image;
    QImage *m_color_bar_image;
    int m_image_width;
    int m_image_high;

    QString m_file_name;
    QString m_file_path;
    QList <int > m_colorbar_list;
};

class RtsImageParsing : public QObject
{
    Q_OBJECT
public:
    RtsImageParsing(QObject *parent = 0);
    ~RtsImageParsing();

    void parsing_file(QString);

    void clear_image();

public slots:
    void slot_parsing_finished();

signals:
    void signal_parsing_finished();

private:
    void ceate_threads();

    void delet_threads();

    ImageWorker *m_image_worker;
    QThread *m_worker_thread;
    QVector <ImageWorker *> m_image_worker_vector;
    QVector <QThread *> m_worker_thread_vector;

    QStringList m_filename_list;
    int m_thread_num;
    QString m_file_path;
};
}

#endif // RTS_IMAGEPARSING_H
