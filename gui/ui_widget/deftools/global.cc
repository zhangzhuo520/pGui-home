#include "global.h"

namespace ui {
QVector <QColor> UiStyle::ItemColorList;
QVector <int> UiStyle::ItemPetternList;
QColor UiStyle::DockTitleBarColor = QColor(145, 194, 242);
QColor UiStyle::ToolBarColor = QColor(216, 216, 216);
QColor UiStyle::ButtonPressColor = QColor(80, 183, 180);
QColor UiStyle::ButtonHoverColor = QColor(80, 183, 220);
bool Global::is_new_dbformat = false;

bool Global::delet_dir(QString path)
{
    if (path.isEmpty())
    {
        return false;
    }

    QDir dir(path);
    if(!dir.exists())
    {
        return true;
    }

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList)
    {
        if (fi.isFile())
        {
            fi.dir().remove(fi.fileName());
        }
        else
        {
            delet_dir(fi.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath());
}

//int Global::screen_width = QApplication::desktop()->screenGeometry().width();
//int Global::screen_height = QApplication::desktop()->screenGeometry().height();
}
