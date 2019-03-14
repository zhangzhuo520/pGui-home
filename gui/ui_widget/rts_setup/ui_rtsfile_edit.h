#ifndef UI_RTSFILE_EDIT_H
#define UI_RTSFILE_EDIT_H

#include <QObject>
#include <QSize>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QDebug>
namespace ui {

class LineNumberArea;
class RtsFileEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    typedef enum{
        BROWSE,
        EDIT
    }editorMode;

    RtsFileEdit(QWidget *parent = 0);
    ~RtsFileEdit();

    void set_mode(editorMode mode);
    void linenumberarea_paintevent(QPaintEvent *);
    int linenumberarea_width();

protected:
    virtual void resizeEvent(QResizeEvent *e);

private slots:
    void slot_update_linenumberarea_width(int);
    void slot_highlight_currentline();
    void slot_update_linenumberarea(const QRect &, int);

private:
    QWidget *m_linenumberarea;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(RtsFileEdit *editor)
    {
        setParent(editor);
        qDebug() << "00000000000";
        codeEditor = editor;
    }

protected:
    virtual QSize sizeHint() const
    {
                qDebug() << "1111111111111";
        return QSize(codeEditor->linenumberarea_width(), 0);
    }

    virtual void paintEvent(QPaintEvent *event)
    {
                    qDebug() << "2222222222";
        codeEditor->linenumberarea_paintevent(event);
    }

private:
    RtsFileEdit *codeEditor;
};

}
#endif // UI_RTSFILE_EDIT_H
