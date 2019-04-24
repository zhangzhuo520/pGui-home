#include "ui_rtsfile_edit.h"
namespace ui {

RtsFileEdit::RtsFileEdit(QWidget *parent):
    QPlainTextEdit(parent)
{
    m_linenumberarea = new LineNumberArea(this);
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(slot_update_linenumberarea_width(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(slot_update_linenumberarea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(slot_highlight_currentline()));

    slot_update_linenumberarea_width(0);
    set_mode(EDIT);
}

RtsFileEdit::~RtsFileEdit()
{
}

void RtsFileEdit::set_mode(RtsFileEdit::editorMode mode)
{
    if(mode == BROWSE)
    {
        this->setReadOnly(true);
        this->setStyleSheet("background:#f2f2f3;");
        slot_highlight_currentline();
    }
    else if(mode == EDIT)
    {
        this->setReadOnly(false);
        this->setStyleSheet("background:#ffffff;");
        slot_highlight_currentline();
    }
}

void RtsFileEdit::linenumberarea_paintevent(QPaintEvent *event)
{
    QPainter painter(m_linenumberarea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(-2, top, m_linenumberarea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int RtsFileEdit::linenumberarea_width()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
    return space;
}

void RtsFileEdit::row_number_update()
{
    m_linenumberarea->update();
    m_linenumberarea->repaint();
}

void RtsFileEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    m_linenumberarea->setGeometry(QRect(cr.left(), cr.top(), linenumberarea_width(), cr.height()));
}

void RtsFileEdit::slot_update_linenumberarea_width(int)
{
      setViewportMargins(linenumberarea_width(), 0, 0, 0);
}

void RtsFileEdit::slot_highlight_currentline()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void RtsFileEdit::slot_update_linenumberarea(const QRect &rect, int dy)
{
    if (dy)
    {
        m_linenumberarea->scroll(0, dy);
    }
    else
    {
        m_linenumberarea->update(0, rect.y(), m_linenumberarea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect()))
    {
        slot_update_linenumberarea_width(0);
    }
}

void RtsFileEdit::slot_rownumber_update()
{
    row_number_update();
}

}
