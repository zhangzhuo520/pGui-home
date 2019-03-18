#ifndef UI_RTSFILE_EDIT_H
#define UI_RTSFILE_EDIT_H

#include <QObject>
#include <QSize>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPlainTextEdit>
#include <QEvent>
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
    void row_number_update();

protected:
    virtual void resizeEvent(QResizeEvent *e);

private slots:
    void slot_update_linenumberarea_width(int);
    void slot_highlight_currentline();
    void slot_update_linenumberarea(const QRect &, int);
    void slot_rownumber_update();

private:
    LineNumberArea *m_linenumberarea;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(RtsFileEdit *editor)
    {
        setParent(editor);
        codeEditor = editor;
    }

protected:
    virtual QSize sizeHint() const
    {
        return QSize(codeEditor->linenumberarea_width(), 0);
    }

    virtual void paintEvent(QPaintEvent *event)
    {
        codeEditor->linenumberarea_paintevent(event);
    }

private:
    RtsFileEdit *codeEditor;
};

}
#endif // UI_RTSFILE_EDIT_H
