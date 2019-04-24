#ifndef RTS_MANAGER_H
#define RTS_MANAGER_H
#include <QObject>
#include <QVector>
#include "../qt_logger/pgui_log_global.h"

namespace ui {

class RtsInfo
{
public:
    explicit RtsInfo(const int& RtsIn = 0);
    ~RtsInfo();

    void set_rts_id(const int& rts_id) {m_rts_id = rts_id;}
    int rts_id() const {return m_rts_id;}

    void set_file_path(const QString& file_path) {m_rts_file = file_path;}
    QString file_path() const {return m_rts_file;}

    void set_image_list(const QStringList& image_list)
    {
        m_rts_imgae_list.clear();
        for (int i = 0; i < image_list.count(); i ++)
        {
            m_rts_imgae_list.append(image_list.at(i));
        }
    }
    QStringList image_list() const {return m_rts_imgae_list;}

    bool operator ==(const RtsInfo& obj)
    {
        if (obj.m_rts_file == m_rts_file &&
                obj.m_rts_id ==  m_rts_id &&
                obj.m_rts_imgae_list == m_rts_imgae_list)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void operator =(const RtsInfo& obj)
    {
        if (this == &obj)
        {
            return;
        }

        m_rts_file = obj.m_rts_file;
        m_rts_id = obj.m_rts_id;
        m_rts_imgae_list.clear();
        m_rts_imgae_list = obj.m_rts_imgae_list;
    }


private:
    int m_rts_id;
    QStringList m_rts_imgae_list;
    QString m_rts_file;
};

class RtsManager : public QObject
{
    Q_OBJECT
public:
    explicit RtsManager();
    ~RtsManager();
    void delete_rts(const int&);
    void delete_all_rts();

    void add_rts(const RtsInfo&);
    RtsInfo rts_info(int);

    int rts_count();
    const QVector <RtsInfo> &rts_vector();

signals:
    void signal_colse_rts_file(const int&);
    void signal_clear_rts_image(const int&);

private:
    bool delete_dir(const QString &);
    void delete_rts_dir(const int &);
    QVector <RtsInfo> m_rts_vector;
};
}
#endif // RTS_MANAGER_H
