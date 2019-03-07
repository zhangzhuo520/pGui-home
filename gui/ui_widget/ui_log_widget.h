#ifndef UI_LOG_WIDGET_H
#define UI_LOG_WIDGET_H

#include <QWidget>
#include <QTextBrowser>
#include <QLayout>
#include <QTextCursor>
#include <QMutex>

namespace ui {
class LogWidget : public QWidget
{
        Q_OBJECT
public:
        LogWidget(QWidget *parent = 0);
        ~LogWidget();

public slots:
        void slot_append_log(const QString &);
private:
        QTextBrowser *m_text_browser;
};

}
#endif // UI_LOG_WIDGET_H
