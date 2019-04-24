#include "rts_manager.h"
#include <QDir>
#include <QFileInfoList>

namespace ui {

RtsManager::RtsManager()
{
}

RtsManager::~RtsManager()
{
    m_rts_vector.clear();
}

void RtsManager::delete_rts_dir(const int & rts_id)
{
    QDir d = QDir::homePath() + "/.pangen_gui/pangen_rts";
    if(!d.exists())
    {
        logger_widget("No Found rts dictory!");
        return;
    }

    QFileInfoList file_list = d.entryInfoList(QDir::Dirs);
    foreach (QFileInfo file_info, file_list)
    {
        QString dir_name = file_info.fileName();
        if(dir_name.size() > 3) //filter hidden directory
        {
            int id = dir_name.right(dir_name.count() - 3).toInt();
            if (id == rts_id)
            {
                delete_dir(file_info.filePath() + "/" + file_info.fileName());
            }
        }
    }
}

bool RtsManager::delete_dir(const QString & dir_path)
{
    if (dir_path.isEmpty())
    {
        return false;
    }

    QDir dir(dir_path);
    if(!dir.exists())
    {
        return true;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);

    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo file, fileList)
    {
        if (file.isFile())
        {
            file.dir().remove(file.fileName());
        }
        else
        {
            delete_dir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath());
}

void RtsManager::add_rts(const RtsInfo &rts_info)
{
    m_rts_vector.append(rts_info);
}

RtsInfo RtsManager::rts_info(int rts_id)
{
    for(int i = 0; i < m_rts_vector.count(); i ++)
    {
        if (rts_id == m_rts_vector[i].rts_id())
        {
            qDebug() << "TIME" << m_rts_vector[i].image_list().count();
            return m_rts_vector[i];
        }
    }
    return RtsInfo(-1);
}

int RtsManager::rts_count()
{
    return m_rts_vector.count();
}

const QVector<RtsInfo> &RtsManager::rts_vector()
{
    return m_rts_vector;
}

void RtsManager::delete_rts(const int & rts_id)
{
    for(int i = 0; i < m_rts_vector.count(); i ++)
    {
        if (rts_id == m_rts_vector[i].rts_id())
        {
            emit signal_clear_rts_image(rts_id);
            emit signal_colse_rts_file(rts_id);
            m_rts_vector.remove(i);
            delete_rts_dir(i);
        }
    }
}

void RtsManager::delete_all_rts()
{
    for(int i = 0; i < m_rts_vector.count(); i ++)
    {
        emit signal_clear_rts_image(m_rts_vector[i].rts_id());
        emit signal_colse_rts_file(m_rts_vector[i].rts_id());
        delete_rts(m_rts_vector[i].rts_id());
    }
    m_rts_vector.clear();
}

RtsInfo::RtsInfo(const int &rts_id):
    m_rts_id(rts_id),
    m_rts_file("")
{
}

RtsInfo::~RtsInfo()
{
    m_rts_imgae_list.clear();
}
}
