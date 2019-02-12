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

        QTextBrowser *view() const {return m_view;}

public slots:
        void slot_append_log(const QString &);
private:
        QMutex m_mutex;
        QTextBrowser *m_view;
};

}
#endif // UI_LOG_WIDGET_H
