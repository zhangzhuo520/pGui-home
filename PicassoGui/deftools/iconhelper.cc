#include "iconhelper.h"
#include <QDebug>
namespace UI {
IconHelper *IconHelper::_instance = 0;

IconHelper::IconHelper(QObject *parent) :
    QObject(parent)
{
    QStringList list = QCoreApplication::applicationDirPath().split("/");
    QString path;
    for (int i = 0; i < list.count() - 1; i ++)
    {
        path = path + list.at(i) + "/";
    }

//    int fontId = QFontDatabase::addApplicationFont(path + "images/fontawesome-webfont.ttf");
//    qDebug() << fontId;
//    QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
//    iconFont = QFont(fontName);
}

void IconHelper::setIcon(QLabel *label, QChar chr)
{
    iconFont.setPointSize(11);
//    label->setFont(iconFont);
    label->setText(chr);
}

void IconHelper::setIcon(QPushButton *button, QChar chr)
{
    iconFont.setPointSize(11);
//    button->setFont(iconFont);
    button->setText(chr);
}

void IconHelper::setStyle(const QString &qssFile) {
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        qApp->setStyleSheet(qss);
        QString PaletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(PaletteColor)));
        file.close();
    }
}
}
